// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Regres is a tool that detects test regressions with SwiftShader changes.
//
// Regres monitors changes that have been put up for review with Gerrit.
// Once a new patchset has been found, regres will checkout, build and test the
// change against the parent changelist. Any differences in results are reported
// as a review comment on the change.
//
// Once a day regres will also test another, larger set of tests, and post the
// full test results as a Gerrit changelist. The CI test lists can be based from
// this daily test list, so testing can be limited to tests that were known to
// pass.
package main

import (
	"crypto/sha1"
	"encoding/hex"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"log"
	"math"
	"math/rand"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"regexp"
	"runtime"
	"sort"
	"strings"
	"sync"
	"time"

	"./cause"
	"./consts"
	"./git"
	"./shell"
	"./testlist"

	gerrit "github.com/andygrunwald/go-gerrit"
)

const (
	gitURL                  = "https://swiftshader.googlesource.com/SwiftShader"
	gerritURL               = "https://swiftshader-review.googlesource.com/"
	reportHeader            = "Regres report:"
	dataVersion             = 1
	changeUpdateFrequency   = time.Minute * 5
	changeQueryFrequency    = time.Minute * 5
	testTimeout             = time.Minute * 2  // timeout for a single test
	buildTimeout            = time.Minute * 10 // timeout for a build
	dailyUpdateTestListHour = 5                // 5am
	fullTestListRelPath     = "tests/regres/full-tests.json"
	ciTestListRelPath       = "tests/regres/ci-tests.json"
	deqpConfigRelPath       = "tests/regres/deqp.json"
)

var (
	numParallelTests = runtime.NumCPU()

	cacheDir      = flag.String("cache", "cache", "path to the output cache directory")
	gerritEmail   = flag.String("email", "$SS_REGRES_EMAIL", "gerrit email address for posting regres results")
	gerritUser    = flag.String("user", "$SS_REGRES_USER", "gerrit username for posting regres results")
	gerritPass    = flag.String("pass", "$SS_REGRES_PASS", "gerrit password for posting regres results")
	keepCheckouts = flag.Bool("keep", false, "don't delete checkout directories after use")
	dryRun        = flag.Bool("dry", false, "don't post regres reports to gerrit")
	maxProcMemory = flag.Uint64("max-proc-mem", shell.MaxProcMemory, "maximum virtual memory per child process")
	dailyNow      = flag.Bool("dailynow", false, "Start by running the daily pass")
	priority      = flag.String("priority", "", "Prioritize a single change with the given id")
)

func main() {
	if runtime.GOOS != "linux" {
		log.Fatal("regres only currently runs on linux")
	}

	flag.ErrHelp = errors.New("regres is a tool to detect regressions between versions of SwiftShader")
	flag.Parse()

	shell.MaxProcMemory = *maxProcMemory

	r := regres{
		cacheRoot:     *cacheDir,
		gerritEmail:   os.ExpandEnv(*gerritEmail),
		gerritUser:    os.ExpandEnv(*gerritUser),
		gerritPass:    os.ExpandEnv(*gerritPass),
		keepCheckouts: *keepCheckouts,
		dryRun:        *dryRun,
		dailyNow:      *dailyNow,
		priority:      *priority,
	}

	if err := r.run(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(-1)
	}
}

type regres struct {
	cmake         string // path to cmake executable
	make          string // path to make executable
	python        string // path to python executable
	cacheRoot     string // path to the regres cache directory
	gerritEmail   string // gerrit email address used for posting results
	gerritUser    string // gerrit username used for posting results
	gerritPass    string // gerrit password used for posting results
	keepCheckouts bool   // don't delete source & build checkouts after testing
	dryRun        bool   // don't post any reviews
	maxProcMemory uint64 // max virtual memory for child processes
	dailyNow      bool   // start with a daily run
	priority      string // Prioritize a single change with the given id
}

// resolveDirs ensures that the necessary directories used can be found, and
// expands them to absolute paths.
func (r *regres) resolveDirs() error {
	allDirs := []*string{
		&r.cacheRoot,
	}

	for _, path := range allDirs {
		abs, err := filepath.Abs(*path)
		if err != nil {
			return cause.Wrap(err, "Couldn't find path '%v'", *path)
		}
		*path = abs
	}

	if err := os.MkdirAll(r.cacheRoot, 0777); err != nil {
		return cause.Wrap(err, "Couldn't create cache root directory")
	}

	for _, path := range allDirs {
		if _, err := os.Stat(*path); err != nil {
			return cause.Wrap(err, "Couldn't find path '%v'", *path)
		}
	}

	return nil
}

// resolveExes resolves all external executables used by regres.
func (r *regres) resolveExes() error {
	type exe struct {
		name string
		path *string
	}
	for _, e := range []exe{
		{"cmake", &r.cmake},
		{"make", &r.make},
		{"python", &r.python},
	} {
		path, err := exec.LookPath(e.name)
		if err != nil {
			return cause.Wrap(err, "Couldn't find path to %s", e.name)
		}
		*e.path = path
	}
	return nil
}

// run performs the main processing loop for the regress tool. It:
// * Scans for open and recently updated changes in gerrit using queryChanges()
//   and changeInfo.update().
// * Builds the most recent patchset and the commit's parent CL using
//   r.newTest(<hash>).lazyRun().
// * Compares the results of the tests using compare().
// * Posts the results of the compare to gerrit as a review.
// * Repeats the above steps until the process is interrupted.
func (r *regres) run() error {
	if err := r.resolveExes(); err != nil {
		return cause.Wrap(err, "Couldn't resolve all exes")
	}

	if err := r.resolveDirs(); err != nil {
		return cause.Wrap(err, "Couldn't resolve all directories")
	}

	client, err := gerrit.NewClient(gerritURL, nil)
	if err != nil {
		return cause.Wrap(err, "Couldn't create gerrit client")
	}
	if r.gerritUser != "" {
		client.Authentication.SetBasicAuth(r.gerritUser, r.gerritPass)
	}

	changes := map[string]*changeInfo{} // Change ID -> changeInfo
	lastUpdatedTestLists := toDate(time.Now())
	lastQueriedChanges := time.Time{}

	if r.dailyNow {
		lastUpdatedTestLists = date{}
	}

	for {
		if now := time.Now(); toDate(now) != lastUpdatedTestLists && now.Hour() >= dailyUpdateTestListHour {
			lastUpdatedTestLists = toDate(now)
			if err := r.updateTestLists(client); err != nil {
				log.Println(err.Error())
			}
		}

		// Update list of tracked changes.
		if time.Since(lastQueriedChanges) > changeQueryFrequency {
			lastQueriedChanges = time.Now()
			if err := queryChanges(client, changes); err != nil {
				log.Println(err.Error())
			}
		}

		// Update change info.
		for _, change := range changes {
			if time.Since(change.lastUpdated) > changeUpdateFrequency {
				change.lastUpdated = time.Now()
				err := change.update(client)
				if err != nil {
					log.Println(cause.Wrap(err, "Couldn't update info for change '%s'", change.id))
				}
			}
		}

		for _, c := range changes {
			if c.pending && r.priority == c.id {
				log.Printf("Prioritizing change '%s'\n", c.id)
				c.priority = 1e6
			}
		}

		// Find the change with the highest priority.
		var change *changeInfo
		numPending := 0
		for _, c := range changes {
			if c.pending {
				numPending++
				if change == nil || c.priority > change.priority {
					change = c
				}
			}
		}

		if change == nil {
			// Everything up to date. Take a break.
			log.Println("Nothing to do. Sleeping")
			time.Sleep(time.Minute)
			continue
		}

		log.Printf("%d changes queued for testing\n", numPending)

		log.Printf("Testing change '%s'\n", change.id)

		// Test the latest patchset in the change, diff against parent change.
		msg, err := r.test(change)
		if err != nil {
			log.Println(cause.Wrap(err, "Failed to test changelist '%s'", change.latest))
			time.Sleep(time.Minute)
			change.pending = false
			continue
		}

		// Always include the reportHeader in the message.
		// changeInfo.update() uses this header to detect whether a patchset has
		// already got a test result.
		msg = reportHeader + "\n\n" + msg

		if r.dryRun {
			log.Printf("DRY RUN: add review to change '%v':\n%v\n", change.id, msg)
		} else {
			log.Printf("Posting review to '%s'\n", change.id)
			_, _, err = client.Changes.SetReview(change.id, change.latest.String(), &gerrit.ReviewInput{
				Message: msg,
				Tag:     "autogenerated:regress",
			})
			if err != nil {
				return cause.Wrap(err, "Failed to post comments on change '%s'", change.id)
			}
		}
		change.pending = false
	}
}

func (r *regres) test(change *changeInfo) (string, error) {
	latest := r.newTest(change.latest)
	defer latest.cleanup()

	if err := latest.checkout(); err != nil {
		return "", cause.Wrap(err, "Failed to checkout '%s'", change.latest)
	}

	deqp, err := r.getOrBuildDEQP(latest)
	if err != nil {
		return "", cause.Wrap(err, "Failed to build dEQP '%v' for change", change.id)
	}

	log.Printf("Testing latest patchset for change '%s'\n", change.id)
	latestResults, testlists, err := r.testLatest(change, latest, deqp)
	if err != nil {
		return "", cause.Wrap(err, "Failed to test latest change of '%v'", change.id)
	}

	log.Printf("Testing parent of change '%s'\n", change.id)
	parentResults, err := r.testParent(change, testlists, deqp)
	if err != nil {
		return "", cause.Wrap(err, "Failed to test parent change of '%v'", change.id)
	}

	log.Println("Comparing latest patchset's results with parent")
	msg := compare(parentResults, latestResults)

	return msg, nil
}

type deqp struct {
	path string // path to deqp directory
	hash string // hash of the deqp config
}

func (r *regres) getOrBuildDEQP(test *test) (deqp, error) {
	srcDir := test.srcDir
	if p := path.Join(srcDir, deqpConfigRelPath); !isFile(p) {
		srcDir, _ = os.Getwd()
		log.Printf("Couldn't open dEQP config file from change (%v), falling back to internal version\n", p)
	} else {
		log.Println("Using dEQP config file from change")
	}
	file, err := os.Open(path.Join(srcDir, deqpConfigRelPath))
	if err != nil {
		return deqp{}, cause.Wrap(err, "Couldn't open dEQP config file")
	}
	defer file.Close()

	cfg := struct {
		Remote  string   `json:"remote"`
		Branch  string   `json:"branch"`
		SHA     string   `json:"sha"`
		Patches []string `json:"patches"`
	}{}
	if err := json.NewDecoder(file).Decode(&cfg); err != nil {
		return deqp{}, cause.Wrap(err, "Couldn't parse %s", deqpConfigRelPath)
	}

	hasher := sha1.New()
	if err := json.NewEncoder(hasher).Encode(&cfg); err != nil {
		return deqp{}, cause.Wrap(err, "Couldn't re-encode %s", deqpConfigRelPath)
	}
	hash := hex.EncodeToString(hasher.Sum(nil))
	cacheDir := path.Join(r.cacheRoot, "deqp", hash)
	buildDir := path.Join(cacheDir, "build")
	if !isDir(cacheDir) {
		if err := os.MkdirAll(cacheDir, 0777); err != nil {
			return deqp{}, cause.Wrap(err, "Couldn't make deqp cache directory '%s'", cacheDir)
		}

		success := false
		defer func() {
			if !success {
				os.RemoveAll(cacheDir)
			}
		}()

		if cfg.Branch != "" {
			// If a branch is specified, then fetch the branch then checkout the
			// commit by SHA. This is a workaround for git repos that error when
			// attempting to directly checkout a remote commit.
			log.Printf("Checking out deqp %v branch %v into %v\n", cfg.Remote, cfg.Branch, cacheDir)
			if err := git.CheckoutRemoteBranch(cacheDir, cfg.Remote, cfg.Branch); err != nil {
				return deqp{}, cause.Wrap(err, "Couldn't checkout deqp branch %v @ %v", cfg.Remote, cfg.Branch)
			}
			log.Printf("Checking out deqp %v commit %v \n", cfg.Remote, cfg.SHA)
			if err := git.CheckoutCommit(cacheDir, git.ParseHash(cfg.SHA)); err != nil {
				return deqp{}, cause.Wrap(err, "Couldn't checkout deqp commit %v @ %v", cfg.Remote, cfg.SHA)
			}
		} else {
			log.Printf("Checking out deqp %v @ %v into %v\n", cfg.Remote, cfg.SHA, cacheDir)
			if err := git.CheckoutRemoteCommit(cacheDir, cfg.Remote, git.ParseHash(cfg.SHA)); err != nil {
				return deqp{}, cause.Wrap(err, "Couldn't checkout deqp commit %v @ %v", cfg.Remote, cfg.SHA)
			}
		}

		log.Println("Fetching deqp dependencies")
		if err := shell.Shell(buildTimeout, r.python, cacheDir, "external/fetch_sources.py"); err != nil {
			return deqp{}, cause.Wrap(err, "Couldn't fetch deqp sources %v @ %v", cfg.Remote, cfg.SHA)
		}

		log.Println("Applying deqp patches")
		for _, patch := range cfg.Patches {
			fullPath := path.Join(srcDir, patch)
			if err := git.Apply(cacheDir, fullPath); err != nil {
				return deqp{}, cause.Wrap(err, "Couldn't apply deqp patch %v for %v @ %v", patch, cfg.Remote, cfg.SHA)
			}
		}

		log.Printf("Building deqp into %v\n", buildDir)
		if err := os.MkdirAll(buildDir, 0777); err != nil {
			return deqp{}, cause.Wrap(err, "Couldn't make deqp build directory '%v'", buildDir)
		}

		if err := shell.Shell(buildTimeout, r.cmake, buildDir,
			"-DDEQP_TARGET=x11_egl",
			"-DCMAKE_BUILD_TYPE=Release",
			".."); err != nil {
			return deqp{}, cause.Wrap(err, "Couldn't generate build rules for deqp %v @ %v", cfg.Remote, cfg.SHA)
		}

		if err := shell.Shell(buildTimeout, r.make, buildDir, fmt.Sprintf("-j%d", runtime.NumCPU())); err != nil {
			return deqp{}, cause.Wrap(err, "Couldn't build deqp %v @ %v", cfg.Remote, cfg.SHA)
		}

		success = true
	}

	return deqp{
		path: cacheDir,
		hash: hash,
	}, nil
}

var additionalTestsRE = regexp.MustCompile(`\n\s*Test[s]?:\s*([^\s]+)[^\n]*`)

func (r *regres) testLatest(change *changeInfo, test *test, d deqp) (*CommitTestResults, testlist.Lists, error) {
	// Get the test results for the latest patchset in the change.
	testlists, err := test.loadTestLists(ciTestListRelPath)
	if err != nil {
		return nil, nil, cause.Wrap(err, "Failed to load '%s'", change.latest)
	}

	if matches := additionalTestsRE.FindAllStringSubmatch(change.commitMessage, -1); len(matches) > 0 {
		log.Println("Change description contains additional test patterns")

		// Change specifies additional tests to try. Load the full test list.
		fullTestLists, err := test.loadTestLists(fullTestListRelPath)
		if err != nil {
			return nil, nil, cause.Wrap(err, "Failed to load '%s'", change.latest)
		}

		// Add any tests in the full list that match the pattern to the list to test.
		for _, match := range matches {
			if len(match) > 1 {
				pattern := match[1]
				log.Printf("Adding custom tests with pattern '%s'\n", pattern)
				filtered := fullTestLists.Filter(func(name string) bool {
					ok, _ := filepath.Match(pattern, name)
					return ok
				})
				testlists = append(testlists, filtered...)
			}
		}
	}

	cachePath := test.resultsCachePath(testlists, d)

	if results, err := loadCommitTestResults(cachePath); err == nil {
		return results, testlists, nil // Use cached results
	}

	// Build the change and test it.
	results := test.buildAndRun(testlists, d)

	// Cache the results for future tests
	if err := results.save(cachePath); err != nil {
		log.Printf("Warning: Couldn't save results of test to '%v'\n", cachePath)
	}

	return results, testlists, nil
}

func (r *regres) testParent(change *changeInfo, testlists testlist.Lists, d deqp) (*CommitTestResults, error) {
	// Get the test results for the changes's parent changelist.
	test := r.newTest(change.parent)
	defer test.cleanup()

	cachePath := test.resultsCachePath(testlists, d)

	if results, err := loadCommitTestResults(cachePath); err == nil {
		return results, nil // Use cached results
	}

	// Couldn't load cached results. Have to build them.
	if err := test.checkout(); err != nil {
		return nil, cause.Wrap(err, "Failed to checkout '%s'", change.parent)
	}

	// Build the parent change and test it.
	results := test.buildAndRun(testlists, d)

	// Store the results of the parent change to the cache.
	if err := results.save(cachePath); err != nil {
		log.Printf("Warning: Couldn't save results of test to '%v'\n", cachePath)
	}

	return results, nil
}

func (r *regres) updateTestLists(client *gerrit.Client) error {
	log.Println("Updating test lists")

	headHash, err := git.FetchRefHash("HEAD", gitURL)
	if err != nil {
		return cause.Wrap(err, "Could not get hash of master HEAD")
	}

	// Get the full test results for latest master.
	test := r.newTest(headHash)
	defer test.cleanup()

	// Always need to checkout the change.
	if err := test.checkout(); err != nil {
		return cause.Wrap(err, "Failed to checkout '%s'", headHash)
	}

	d, err := r.getOrBuildDEQP(test)
	if err != nil {
		return cause.Wrap(err, "Failed to build deqp for '%s'", headHash)
	}

	// Load the test lists.
	testLists, err := test.loadTestLists(fullTestListRelPath)
	if err != nil {
		return cause.Wrap(err, "Failed to load full test lists for '%s'", headHash)
	}

	// Build the change.
	if err := test.build(); err != nil {
		return cause.Wrap(err, "Failed to build '%s'", headHash)
	}

	// Run the tests on the change.
	results, err := test.run(testLists, d)
	if err != nil {
		return cause.Wrap(err, "Failed to test '%s'", headHash)
	}

	// Write out the test list status files.
	filePaths, err := test.writeTestListsByStatus(testLists, results)
	if err != nil {
		return cause.Wrap(err, "Failed to write test lists by status")
	}

	// Stage all the updated test files.
	for _, path := range filePaths {
		log.Println("Staging", path)
		git.Add(test.srcDir, path)
	}

	log.Println("Checking for existing test list")
	existingChange, err := r.findTestListChange(client)
	if err != nil {
		return err
	}

	commitMsg := strings.Builder{}
	commitMsg.WriteString(consts.TestListUpdateCommitSubjectPrefix + headHash.String()[:8])
	if existingChange != nil {
		// Reuse gerrit change ID if there's already a change up for review.
		commitMsg.WriteString("\n\n")
		commitMsg.WriteString("Change-Id: " + existingChange.ChangeID + "\n")
	}

	if err := git.Commit(test.srcDir, commitMsg.String(), git.CommitFlags{
		Name:  "SwiftShader Regression Bot",
		Email: r.gerritEmail,
	}); err != nil {
		return cause.Wrap(err, "Failed to commit test results")
	}

	if r.dryRun {
		log.Printf("DRY RUN: post results for review")
	} else {
		log.Println("Pushing test results for review")
		if err := git.Push(test.srcDir, gitURL, "HEAD", "refs/for/master", git.PushFlags{
			Username: r.gerritUser,
			Password: r.gerritPass,
		}); err != nil {
			return cause.Wrap(err, "Failed to push test results for review")
		}
		log.Println("Test results posted for review")
	}

	change, err := r.findTestListChange(client)
	if err != nil {
		return err
	}

	if err := r.postMostCommonFailures(client, change, results); err != nil {
		return err
	}

	return nil
}

// postMostCommonFailures posts the most common failure cases as a review
// comment on the given change.
func (r *regres) postMostCommonFailures(client *gerrit.Client, change *gerrit.ChangeInfo, results *CommitTestResults) error {
	const limit = 25

	failures := results.commonFailures()
	if len(failures) > limit {
		failures = failures[:limit]
	}
	sb := strings.Builder{}
	sb.WriteString(fmt.Sprintf("Top %v most common failures:\n", len(failures)))
	for _, f := range failures {
		lines := strings.Split(f.error, "\n")
		if len(lines) == 1 {
			line := lines[0]
			if line != "" {
				sb.WriteString(fmt.Sprintf(" • %d occurrences: %v: %v\n", f.count, f.status, line))
			} else {
				sb.WriteString(fmt.Sprintf(" • %d occurrences: %v\n", f.count, f.status))
			}
		} else {
			sb.WriteString(fmt.Sprintf(" • %d occurrences: %v:\n", f.count, f.status))
			for _, l := range lines {
				sb.WriteString("    > ")
				sb.WriteString(l)
				sb.WriteString("\n")
			}
		}
		sb.WriteString(fmt.Sprintf("    Example test: %v\n", f.exampleTest))

	}
	msg := sb.String()

	if r.dryRun {
		log.Printf("DRY RUN: add most common failures to '%v':\n%v\n", change.ChangeID, msg)
	} else {
		log.Printf("Posting most common failures to '%s'\n", change.ChangeID)
		_, _, err := client.Changes.SetReview(change.ChangeID, change.CurrentRevision, &gerrit.ReviewInput{
			Message: msg,
			Tag:     "autogenerated:regress",
		})
		if err != nil {
			return cause.Wrap(err, "Failed to post comments on change '%s'", change.ChangeID)
		}
	}
	return nil
}

func (r *regres) findTestListChange(client *gerrit.Client) (*gerrit.ChangeInfo, error) {
	log.Println("Checking for existing test list change")
	changes, _, err := client.Changes.QueryChanges(&gerrit.QueryChangeOptions{
		QueryOptions: gerrit.QueryOptions{
			Query: []string{fmt.Sprintf(`status:open+owner:"%v"`, r.gerritEmail)},
			Limit: 1,
		},
		ChangeOptions: gerrit.ChangeOptions{
			AdditionalFields: []string{"CURRENT_REVISION"},
		},
	})
	if err != nil {
		return nil, cause.Wrap(err, "Failed to checking for existing test list")
	}
	if len(*changes) > 0 {
		// TODO: This currently assumes that only change changes from
		// gerritEmail are test lists updates. This may not always be true.
		return &(*changes)[0], nil
	}
	return nil, nil
}

// changeInfo holds the important information about a single, open change in
// gerrit.
type changeInfo struct {
	id            string    // Gerrit change ID.
	pending       bool      // Is this change waiting a test for the latest patchset?
	priority      int       // Calculated priority based on Gerrit labels.
	latest        git.Hash  // Git hash of the latest patchset in the change.
	parent        git.Hash  // Git hash of the changelist this change is based on.
	lastUpdated   time.Time // Time the change was last fetched.
	commitMessage string
}

// queryChanges updates the changes map by querying gerrit for the latest open
// changes.
func queryChanges(client *gerrit.Client, changes map[string]*changeInfo) error {
	log.Println("Checking for latest changes")
	results, _, err := client.Changes.QueryChanges(&gerrit.QueryChangeOptions{
		QueryOptions: gerrit.QueryOptions{
			Query: []string{"status:open+-age:3d"},
			Limit: 100,
		},
	})
	if err != nil {
		return cause.Wrap(err, "Failed to get list of changes")
	}

	ids := map[string]bool{}
	for _, r := range *results {
		ids[r.ChangeID] = true
	}

	// Add new changes
	for id := range ids {
		if _, found := changes[id]; !found {
			log.Printf("Tracking new change '%v'\n", id)
			changes[id] = &changeInfo{id: id}
		}
	}

	// Remove old changes
	for id := range changes {
		if found := ids[id]; !found {
			log.Printf("Untracking change '%v'\n", id)
			delete(changes, id)
		}
	}

	return nil
}

// update queries gerrit for information about the given change.
func (c *changeInfo) update(client *gerrit.Client) error {
	change, _, err := client.Changes.GetChange(c.id, &gerrit.ChangeOptions{
		AdditionalFields: []string{"CURRENT_REVISION", "CURRENT_COMMIT", "MESSAGES", "LABELS"},
	})
	if err != nil {
		return cause.Wrap(err, "Getting info for change '%s'", c.id)
	}

	current, ok := change.Revisions[change.CurrentRevision]
	if !ok {
		return fmt.Errorf("Couldn't find current revision for change '%s'", c.id)
	}

	if len(current.Commit.Parents) == 0 {
		return fmt.Errorf("Couldn't find current commit for change '%s' has no parents(?)", c.id)
	}

	kokoroPresubmit := change.Labels["Kokoro-Presubmit"].Approved.AccountID != 0
	codeReviewScore := change.Labels["Code-Review"].Value
	codeReviewApproved := change.Labels["Code-Review"].Approved.AccountID != 0
	presubmitReady := change.Labels["Presubmit-Ready"].Approved.AccountID != 0

	c.priority = 0
	if presubmitReady {
		c.priority += 10
	}
	c.priority += codeReviewScore
	if codeReviewApproved {
		c.priority += 2
	}
	if kokoroPresubmit {
		c.priority++
	}

	// Is the change from a Googler?
	canTest := strings.HasSuffix(current.Commit.Committer.Email, "@google.com")

	// Has the latest patchset already been tested?
	if canTest {
		for _, msg := range change.Messages {
			if msg.RevisionNumber == current.Number &&
				strings.Contains(msg.Message, reportHeader) {
				canTest = false
				break
			}
		}
	}

	c.pending = canTest
	c.latest = git.ParseHash(change.CurrentRevision)
	c.parent = git.ParseHash(current.Commit.Parents[0].Commit)
	c.commitMessage = current.Commit.Message

	return nil
}

func (r *regres) newTest(commit git.Hash) *test {
	srcDir := filepath.Join(r.cacheRoot, "src", commit.String())
	resDir := filepath.Join(r.cacheRoot, "res", commit.String())
	return &test{
		r:        r,
		commit:   commit,
		srcDir:   srcDir,
		resDir:   resDir,
		buildDir: filepath.Join(srcDir, "build"),
	}
}

type test struct {
	r             *regres
	commit        git.Hash // hash of the commit to test
	srcDir        string   // directory for the SwiftShader checkout
	resDir        string   // directory for the test results
	buildDir      string   // directory for SwiftShader build
	keepCheckouts bool     // don't delete source & build checkouts after testing
}

// cleanup removes any temporary files used by the test.
func (t *test) cleanup() {
	if t.srcDir != "" && !t.keepCheckouts {
		os.RemoveAll(t.srcDir)
	}
}

// checkout clones the test's source commit into t.src.
func (t *test) checkout() error {
	if isDir(t.srcDir) && t.keepCheckouts {
		log.Printf("Reusing source cache for commit '%s'\n", t.commit)
		return nil
	}
	log.Printf("Checking out '%s'\n", t.commit)
	os.RemoveAll(t.srcDir)
	if err := git.CheckoutRemoteCommit(t.srcDir, gitURL, t.commit); err != nil {
		return cause.Wrap(err, "Checking out commit '%s'", t.commit)
	}
	log.Printf("Checked out commit '%s'\n", t.commit)
	return nil
}

// buildAndRun calls t.build() followed by t.run(). Errors are logged and
// reported in the returned CommitTestResults.Error field.
func (t *test) buildAndRun(testLists testlist.Lists, d deqp) *CommitTestResults {
	// Build the parent change.
	if err := t.build(); err != nil {
		msg := fmt.Sprintf("Failed to build '%s'", t.commit)
		log.Println(cause.Wrap(err, msg))
		return &CommitTestResults{Error: msg}
	}

	// Run the tests on the parent change.
	results, err := t.run(testLists, d)
	if err != nil {
		msg := fmt.Sprintf("Failed to test change '%s'", t.commit)
		log.Println(cause.Wrap(err, msg))
		return &CommitTestResults{Error: msg}
	}

	return results
}

// build builds the SwiftShader source into t.buildDir.
func (t *test) build() error {
	log.Printf("Building '%s'\n", t.commit)

	if err := os.MkdirAll(t.buildDir, 0777); err != nil {
		return cause.Wrap(err, "Failed to create build directory")
	}

	if err := shell.Shell(buildTimeout, t.r.cmake, t.buildDir,
		"-DCMAKE_BUILD_TYPE=Release",
		"-DDCHECK_ALWAYS_ON=1",
		"-DREACTOR_VERIFY_LLVM_IR=1",
		"-DWARNINGS_AS_ERRORS=0",
		".."); err != nil {
		return err
	}

	if err := shell.Shell(buildTimeout, t.r.make, t.buildDir, fmt.Sprintf("-j%d", runtime.NumCPU())); err != nil {
		return err
	}

	return nil
}

// run runs all the tests.
func (t *test) run(testLists testlist.Lists, d deqp) (*CommitTestResults, error) {
	log.Printf("Running tests for '%s'\n", t.commit)

	outDir := filepath.Join(t.srcDir, "out")
	if !isDir(outDir) { // https://swiftshader-review.googlesource.com/c/SwiftShader/+/27188
		outDir = t.buildDir
	}
	if !isDir(outDir) {
		return nil, fmt.Errorf("Couldn't find output directory")
	}
	log.Println("outDir:", outDir)

	start := time.Now()

	// Wait group that completes once all the tests have finished.
	wg := sync.WaitGroup{}
	results := make(chan TestResult, 256)

	numTests := 0

	// For each API that we are testing
	for _, list := range testLists {
		// Resolve the test runner
		var exe string
		switch list.API {
		case testlist.EGL:
			exe = filepath.Join(d.path, "build", "modules", "egl", "deqp-egl")
		case testlist.GLES2:
			exe = filepath.Join(d.path, "build", "modules", "gles2", "deqp-gles2")
		case testlist.GLES3:
			exe = filepath.Join(d.path, "build", "modules", "gles3", "deqp-gles3")
		case testlist.Vulkan:
			exe = filepath.Join(d.path, "build", "external", "vulkancts", "modules", "vulkan", "deqp-vk")
		default:
			return nil, fmt.Errorf("Unknown API '%v'", list.API)
		}
		if !isFile(exe) {
			return nil, fmt.Errorf("Couldn't find dEQP executable at '%s'", exe)
		}

		// Build a chan for the test names to be run.
		tests := make(chan string, len(list.Tests))

		// Start a number of go routines to run the tests.
		wg.Add(numParallelTests)
		for i := 0; i < numParallelTests; i++ {
			go func() {
				t.deqpTestRoutine(exe, outDir, tests, results)
				wg.Done()
			}()
		}

		// Shuffle the test list.
		// This attempts to mix heavy-load tests with lighter ones.
		shuffled := make([]string, len(list.Tests))
		for i, j := range rand.New(rand.NewSource(42)).Perm(len(list.Tests)) {
			shuffled[i] = list.Tests[j]
		}

		// Hand the tests to the deqpTestRoutines.
		for _, t := range shuffled {
			tests <- t
		}

		// Close the tests chan to indicate that there are no more tests to run.
		// The deqpTestRoutine functions will return once all tests have been
		// run.
		close(tests)

		numTests += len(list.Tests)
	}

	out := CommitTestResults{
		Version: dataVersion,
		Tests:   map[string]TestResult{},
	}

	// Collect the results.
	finished := make(chan struct{})
	lastUpdate := time.Now()
	go func() {
		start, i := time.Now(), 0
		for r := range results {
			i++
			out.Tests[r.Test] = r
			if time.Since(lastUpdate) > time.Minute {
				lastUpdate = time.Now()
				remaining := numTests - i
				log.Printf("Ran %d/%d tests (%v%%). Estimated completion in %v.\n",
					i, numTests, percent(i, numTests),
					(time.Since(start)/time.Duration(i))*time.Duration(remaining))
			}
		}
		close(finished)
	}()

	wg.Wait()      // Block until all the deqpTestRoutines have finished.
	close(results) // Signal no more results.
	<-finished     // And wait for the result collecting go-routine to finish.

	out.Duration = time.Since(start)

	return &out, nil
}

func (t *test) writeTestListsByStatus(testLists testlist.Lists, results *CommitTestResults) ([]string, error) {
	out := []string{}

	for _, list := range testLists {
		files := map[testlist.Status]*os.File{}
		for _, status := range testlist.Statuses {
			path := testlist.FilePathWithStatus(filepath.Join(t.srcDir, list.File), status)
			dir := filepath.Dir(path)
			os.MkdirAll(dir, 0777)
			f, err := os.Create(path)
			if err != nil {
				return nil, cause.Wrap(err, "Couldn't create file '%v'", path)
			}
			defer f.Close()
			files[status] = f

			out = append(out, path)
		}

		for _, testName := range list.Tests {
			if r, found := results.Tests[testName]; found {
				fmt.Fprintln(files[r.Status], testName)
			}
		}
	}

	return out, nil
}

// resultsCachePath returns the path to the cache results file for the given
// test, testlists and path to deqp.
func (t *test) resultsCachePath(testLists testlist.Lists, d deqp) string {
	return filepath.Join(t.resDir, testLists.Hash(), d.hash)
}

// CommitTestResults holds the results the tests across all APIs for a given
// commit. The CommitTestResults structure may be serialized to cache the
// results.
type CommitTestResults struct {
	Version  int
	Error    string
	Tests    map[string]TestResult
	Duration time.Duration
}

func loadCommitTestResults(path string) (*CommitTestResults, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, cause.Wrap(err, "Couldn't open '%s' for loading test results", path)
	}
	defer f.Close()

	var out CommitTestResults
	if err := json.NewDecoder(f).Decode(&out); err != nil {
		return nil, err
	}
	if out.Version != dataVersion {
		return nil, errors.New("Data is from an old version")
	}
	return &out, nil
}

func (r *CommitTestResults) save(path string) error {
	os.MkdirAll(filepath.Dir(path), 0777)

	f, err := os.Create(path)
	if err != nil {
		return cause.Wrap(err, "Couldn't open '%s' for saving test results", path)
	}
	defer f.Close()

	enc := json.NewEncoder(f)
	enc.SetIndent("", "  ")
	if err := enc.Encode(r); err != nil {
		return cause.Wrap(err, "Couldn't encode test results")
	}

	return nil
}

type testStatusAndError struct {
	status testlist.Status
	error  string
}

type commonFailure struct {
	count int
	testStatusAndError
	exampleTest string
}

func (r *CommitTestResults) commonFailures() []commonFailure {
	failures := map[testStatusAndError]int{}
	examples := map[testStatusAndError]string{}
	for name, test := range r.Tests {
		if !test.Status.Failing() {
			continue
		}
		key := testStatusAndError{test.Status, test.Err}
		if count, ok := failures[key]; ok {
			failures[key] = count + 1
		} else {
			failures[key] = 1
			examples[key] = name
		}
	}
	out := make([]commonFailure, 0, len(failures))
	for failure, count := range failures {
		out = append(out, commonFailure{count, failure, examples[failure]})
	}
	sort.Slice(out, func(i, j int) bool { return out[i].count > out[j].count })
	return out
}

// compare returns a string describing all differences between two
// CommitTestResults. This string is used as the report message posted to the
// gerrit code review.
func compare(old, new *CommitTestResults) string {
	if old.Error != "" {
		return old.Error
	}
	if new.Error != "" {
		return new.Error
	}

	oldStatusCounts, newStatusCounts := map[testlist.Status]int{}, map[testlist.Status]int{}
	totalTests := 0

	broken, fixed, failing, removed, changed := []string{}, []string{}, []string{}, []string{}, []string{}

	for test, new := range new.Tests {
		old, found := old.Tests[test]
		if !found {
			log.Printf("Test result for '%s' not found on old change\n", test)
			continue
		}
		switch {
		case !old.Status.Failing() && new.Status.Failing():
			broken = append(broken, test)
		case !old.Status.Passing() && new.Status.Passing():
			fixed = append(fixed, test)
		case old.Status != new.Status:
			changed = append(changed, test)
		case old.Status.Failing() && new.Status.Failing():
			failing = append(failing, test) // Still broken
		}
		totalTests++
		if found {
			oldStatusCounts[old.Status] = oldStatusCounts[old.Status] + 1
		}
		newStatusCounts[new.Status] = newStatusCounts[new.Status] + 1
	}

	for test := range old.Tests {
		if _, found := new.Tests[test]; !found {
			removed = append(removed, test)
		}
	}

	sb := strings.Builder{}

	// list prints the list l to sb, truncating after a limit.
	list := func(l []string) {
		const max = 10
		for i, s := range l {
			sb.WriteString("  ")
			if i == max {
				sb.WriteString(fmt.Sprintf("> %d more\n", len(l)-i))
				break
			}
			sb.WriteString(fmt.Sprintf("> %s", s))
			if n, ok := new.Tests[s]; ok {
				if o, ok := old.Tests[s]; ok && n != o {
					sb.WriteString(fmt.Sprintf(" - [%s -> %s]", o.Status, n.Status))
				} else {
					sb.WriteString(fmt.Sprintf(" - [%s]", n.Status))
				}
				sb.WriteString("\n")
				for _, line := range strings.Split(n.Err, "\n") {
					if line != "" {
						sb.WriteString(fmt.Sprintf("     %v\n", line))
					}
				}
			} else {
				sb.WriteString("\n")
			}
		}
	}

	sb.WriteString(fmt.Sprintf("          Total tests: %d\n", totalTests))
	for _, s := range []struct {
		label  string
		status testlist.Status
	}{
		{"                 Pass", testlist.Pass},
		{"                 Fail", testlist.Fail},
		{"              Timeout", testlist.Timeout},
		{"      UNIMPLEMENTED()", testlist.Unimplemented},
		{"        UNSUPPORTED()", testlist.Unsupported},
		{"        UNREACHABLE()", testlist.Unreachable},
		{"             ASSERT()", testlist.Assert},
		{"              ABORT()", testlist.Abort},
		{"                Crash", testlist.Crash},
		{"        Not Supported", testlist.NotSupported},
		{"Compatibility Warning", testlist.CompatibilityWarning},
		{"      Quality Warning", testlist.QualityWarning},
	} {
		old, new := oldStatusCounts[s.status], newStatusCounts[s.status]
		if old == 0 && new == 0 {
			continue
		}
		change := percent64(int64(new-old), int64(old))
		switch {
		case old == new:
			sb.WriteString(fmt.Sprintf("%s: %v\n", s.label, new))
		case change == 0:
			sb.WriteString(fmt.Sprintf("%s: %v -> %v (%+d)\n", s.label, old, new, new-old))
		default:
			sb.WriteString(fmt.Sprintf("%s: %v -> %v (%+d %+d%%)\n", s.label, old, new, new-old, change))
		}
	}

	if old, new := old.Duration, new.Duration; old != 0 && new != 0 {
		label := "           Time taken"
		change := percent64(int64(new-old), int64(old))
		switch {
		case old == new:
			sb.WriteString(fmt.Sprintf("%s: %v\n", label, new))
		case change == 0:
			sb.WriteString(fmt.Sprintf("%s: %v -> %v\n", label, old, new))
		default:
			sb.WriteString(fmt.Sprintf("%s: %v -> %v (%+d%%)\n", label, old, new, change))
		}
	}

	if n := len(broken); n > 0 {
		sort.Strings(broken)
		sb.WriteString(fmt.Sprintf("\n--- This change breaks %d tests: ---\n", n))
		list(broken)
	}
	if n := len(fixed); n > 0 {
		sort.Strings(fixed)
		sb.WriteString(fmt.Sprintf("\n--- This change fixes %d tests: ---\n", n))
		list(fixed)
	}
	if n := len(removed); n > 0 {
		sort.Strings(removed)
		sb.WriteString(fmt.Sprintf("\n--- This change removes %d tests: ---\n", n))
		list(removed)
	}
	if n := len(changed); n > 0 {
		sort.Strings(changed)
		sb.WriteString(fmt.Sprintf("\n--- This change alters %d tests: ---\n", n))
		list(changed)
	}

	if len(broken) == 0 && len(fixed) == 0 && len(removed) == 0 && len(changed) == 0 {
		sb.WriteString(fmt.Sprintf("\n--- No change in test results ---\n"))
	}

	type timingDiff struct {
		old      time.Duration
		new      time.Duration
		relDelta float64
		name     string
	}

	timingDiffs := []timingDiff{}
	for name, new := range new.Tests {
		if old, ok := old.Tests[name]; ok {
			old, new := old.TimeTaken, new.TimeTaken
			delta := new.Seconds() - old.Seconds()
			absDelta := math.Abs(delta)
			relDelta := delta / old.Seconds()
			if absDelta > 2.0 && math.Abs(relDelta) > 0.05 { // If change > ±2s and > than ±5% old time...
				timingDiffs = append(timingDiffs, timingDiff{
					old:      old,
					new:      new,
					name:     name,
					relDelta: relDelta,
				})
			}
		}
	}
	if len(timingDiffs) > 0 {
		sb.WriteString(fmt.Sprintf("\n--- Test duration changes ---\n"))
		const limit = 10
		if len(timingDiffs) > limit {
			sort.Slice(timingDiffs, func(i, j int) bool { return math.Abs(timingDiffs[i].relDelta) > math.Abs(timingDiffs[j].relDelta) })
			timingDiffs = timingDiffs[:limit]
		}
		sort.Slice(timingDiffs, func(i, j int) bool { return timingDiffs[i].relDelta < timingDiffs[j].relDelta })
		for _, d := range timingDiffs {
			percent := percent64(int64(d.new-d.old), int64(d.old))
			sb.WriteString(fmt.Sprintf("  > %v: %v -> %v (%+d%%)\n", d.name, d.old, d.new, percent))
		}
	}

	return sb.String()
}

// TestResult holds the results of a single API test.
type TestResult struct {
	Test      string
	Status    testlist.Status
	TimeTaken time.Duration
	Err       string `json:",omitempty"`
}

func (r TestResult) String() string {
	if r.Err != "" {
		return fmt.Sprintf("%s: %s (%s)", r.Test, r.Status, r.Err)
	}
	return fmt.Sprintf("%s: %s", r.Test, r.Status)
}

var (
	// Regular expression to parse the output of a dEQP test.
	deqpRE = regexp.MustCompile(`(Fail|Pass|NotSupported|CompatibilityWarning|QualityWarning) \(([^\)]*)\)`)
	// Regular expression to parse a test that failed due to UNIMPLEMENTED()
	unimplementedRE = regexp.MustCompile(`[^\n]*UNIMPLEMENTED:[^\n]*`)
	// Regular expression to parse a test that failed due to UNSUPPORTED()
	unsupportedRE = regexp.MustCompile(`[^\n]*UNSUPPORTED:[^\n]*`)
	// Regular expression to parse a test that failed due to UNREACHABLE()
	unreachableRE = regexp.MustCompile(`[^\n]*UNREACHABLE:[^\n]*`)
	// Regular expression to parse a test that failed due to ASSERT()
	assertRE = regexp.MustCompile(`[^\n]*ASSERT\([^\)]*\)[^\n]*`)
	// Regular expression to parse a test that failed due to ABORT()
	abortRE = regexp.MustCompile(`[^\n]*ABORT:[^\n]*`)
)

// deqpTestRoutine repeatedly runs the dEQP test executable exe with the tests
// taken from tests. The output of the dEQP test is parsed, and the test result
// is written to results.
// deqpTestRoutine only returns once the tests chan has been closed.
// deqpTestRoutine does not close the results chan.
func (t *test) deqpTestRoutine(exe, outDir string, tests <-chan string, results chan<- TestResult) {
nextTest:
	for name := range tests {
		// log.Printf("Running test '%s'\n", name)
		env := []string{
			"LD_LIBRARY_PATH=" + t.buildDir + ":" + os.Getenv("LD_LIBRARY_PATH"),
			"VK_ICD_FILENAMES=" + filepath.Join(outDir, "Linux", "vk_swiftshader_icd.json"),
			"DISPLAY=" + os.Getenv("DISPLAY"),
			"LIBC_FATAL_STDERR_=1", // Put libc explosions into logs.
		}

		start := time.Now()
		outRaw, err := shell.Exec(testTimeout, exe, filepath.Dir(exe), env,
			"--deqp-surface-type=pbuffer",
			"--deqp-shadercache=disable",
			"--deqp-log-images=disable",
			"--deqp-log-shader-sources=disable",
			"--deqp-log-flush=disable",
			"-n="+name)
		duration := time.Since(start)
		out := string(outRaw)
		out = strings.ReplaceAll(out, t.srcDir, "<SwiftShader>")
		out = strings.ReplaceAll(out, exe, "<dEQP>")

		// Don't treat non-zero error codes as crashes.
		var exitErr *exec.ExitError
		if errors.As(err, &exitErr) {
			if exitErr.ExitCode() != -1 {
				out += fmt.Sprintf("\nProcess terminated with code %d", exitErr.ExitCode())
				err = nil
			}
		}

		switch err.(type) {
		default:
			for _, test := range []struct {
				re *regexp.Regexp
				s  testlist.Status
			}{
				{unimplementedRE, testlist.Unimplemented},
				{unsupportedRE, testlist.Unsupported},
				{unreachableRE, testlist.Unreachable},
				{assertRE, testlist.Assert},
				{abortRE, testlist.Abort},
			} {
				if s := test.re.FindString(out); s != "" {
					results <- TestResult{
						Test:      name,
						Status:    test.s,
						TimeTaken: duration,
						Err:       s,
					}
					continue nextTest
				}
			}
			results <- TestResult{
				Test:      name,
				Status:    testlist.Crash,
				TimeTaken: duration,
				Err:       out,
			}
		case shell.ErrTimeout:
			log.Printf("Timeout for test '%v'\n", name)
			results <- TestResult{
				Test:      name,
				Status:    testlist.Timeout,
				TimeTaken: duration,
			}
		case nil:
			toks := deqpRE.FindStringSubmatch(out)
			if len(toks) < 3 {
				err := fmt.Sprintf("Couldn't parse test '%v' output:\n%s", name, out)
				log.Println("Warning: ", err)
				results <- TestResult{Test: name, Status: testlist.Fail, Err: err}
				continue
			}
			switch toks[1] {
			case "Pass":
				results <- TestResult{Test: name, Status: testlist.Pass, TimeTaken: duration}
			case "NotSupported":
				results <- TestResult{Test: name, Status: testlist.NotSupported, TimeTaken: duration}
			case "CompatibilityWarning":
				results <- TestResult{Test: name, Status: testlist.CompatibilityWarning, TimeTaken: duration}
			case "QualityWarning":
				results <- TestResult{Test: name, Status: testlist.QualityWarning, TimeTaken: duration}
			case "Fail":
				var err string
				if toks[2] != "Fail" {
					err = toks[2]
				}
				results <- TestResult{Test: name, Status: testlist.Fail, Err: err, TimeTaken: duration}
			default:
				err := fmt.Sprintf("Couldn't parse test output:\n%s", out)
				log.Println("Warning: ", err)
				results <- TestResult{Test: name, Status: testlist.Fail, Err: err, TimeTaken: duration}
			}
		}
	}
}

// loadTestLists loads the full test lists from the json file.
// The file is first searched at {t.srcDir}/{relPath}
// If this cannot be found, then the file is searched at the fallback path
// {CWD}/{relPath}
// This allows CLs to alter the list of tests to be run, as well as providing
// a default set.
func (t *test) loadTestLists(relPath string) (testlist.Lists, error) {
	// Seach for the test.json file in the checked out source directory.
	if path := filepath.Join(t.srcDir, relPath); isFile(path) {
		log.Printf("Loading test list '%v' from commit\n", relPath)
		return testlist.Load(t.srcDir, path)
	}

	// Not found there. Search locally.
	wd, err := os.Getwd()
	if err != nil {
		return testlist.Lists{}, cause.Wrap(err, "Couldn't get current working directory")
	}
	if path := filepath.Join(wd, relPath); isFile(path) {
		log.Printf("Loading test list '%v' from regres\n", relPath)
		return testlist.Load(wd, relPath)
	}

	return nil, errors.New("Couldn't find a test list file")
}

// isDir returns true if path is a file.
func isFile(path string) bool {
	s, err := os.Stat(path)
	if err != nil {
		return false
	}
	return !s.IsDir()
}

// isDir returns true if path is a directory.
func isDir(path string) bool {
	s, err := os.Stat(path)
	if err != nil {
		return false
	}
	return s.IsDir()
}

// percent returns the percentage completion of i items out of n.
func percent(i, n int) int {
	return int(percent64(int64(i), int64(n)))
}

// percent64 returns the percentage completion of i items out of n.
func percent64(i, n int64) int64 {
	if n == 0 {
		return 0
	}
	return (100 * i) / n
}

type date struct {
	year  int
	month time.Month
	day   int
}

func toDate(t time.Time) date {
	d := date{}
	d.year, d.month, d.day = t.Date()
	return d
}
