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

// Package deqp provides functions for running dEQP, as well as loading and storing the results.
package deqp

import (
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"math/rand"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strconv"
	"strings"
	"sync"
	"time"

	"../cause"
	"../shell"
	"../testlist"
	"../util"
)

const dataVersion = 1

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

// Config contains the inputs required for running dEQP on a group of test lists.
type Config struct {
	ExeEgl           string
	ExeGles2         string
	ExeGles3         string
	ExeVulkan        string
	TestLists        testlist.Lists
	Env              []string
	LogReplacements  map[string]string
	NumParallelTests int
	TestTimeout      time.Duration
}

// Results holds the results of tests across all APIs.
// The Results structure may be serialized to cache results.
type Results struct {
	Version  int
	Error    string
	Tests    map[string]TestResult
	Duration time.Duration
}

// TestResult holds the results of a single dEQP test.
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

// LoadResults loads cached test results from disk.
func LoadResults(path string) (*Results, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, cause.Wrap(err, "Couldn't open '%s' for loading test results", path)
	}
	defer f.Close()

	var out Results
	if err := json.NewDecoder(f).Decode(&out); err != nil {
		return nil, err
	}
	if out.Version != dataVersion {
		return nil, errors.New("Data is from an old version")
	}
	return &out, nil
}

// Save saves (caches) test results to disk.
func (r *Results) Save(path string) error {
	if err := os.MkdirAll(filepath.Dir(path), 0777); err != nil {
		return cause.Wrap(err, "couldn't make '%s' for saving test results", filepath.Dir(path))
	}

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

// Run runs all the tests.
func (c *Config) Run() (*Results, error) {

	start := time.Now()

	// Wait group that completes once all the tests have finished.
	wg := sync.WaitGroup{}
	results := make(chan TestResult, 256)

	numTests := 0

	goroutineIndex := 0

	// For each API that we are testing
	for _, list := range c.TestLists {
		// Resolve the test runner
		var exe string
		switch list.API {
		case testlist.EGL:
			exe = c.ExeEgl
		case testlist.GLES2:
			exe = c.ExeGles2
		case testlist.GLES3:
			exe = c.ExeGles3
		case testlist.Vulkan:
			exe = c.ExeVulkan
		default:
			return nil, fmt.Errorf("Unknown API '%v'", list.API)
		}
		if !util.IsFile(exe) {
			return nil, fmt.Errorf("Couldn't find dEQP executable at '%s'", exe)
		}

		// Build a chan for the test names to be run.
		tests := make(chan string, len(list.Tests))

		// Start a number of go routines to run the tests.
		wg.Add(c.NumParallelTests)
		for i := 0; i < c.NumParallelTests; i++ {
			go func(index int) {
				c.TestRoutine(exe, tests, results, index)
				wg.Done()
			}(goroutineIndex)
			goroutineIndex++
		}

		// Shuffle the test list.
		// This attempts to mix heavy-load tests with lighter ones.
		shuffled := make([]string, len(list.Tests))
		for i, j := range rand.New(rand.NewSource(42)).Perm(len(list.Tests)) {
			shuffled[i] = list.Tests[j]
		}

		// Hand the tests to the TestRoutines.
		for _, t := range shuffled {
			tests <- t
		}

		// Close the tests chan to indicate that there are no more tests to run.
		// The TestRoutine functions will return once all tests have been
		// run.
		close(tests)

		numTests += len(list.Tests)
	}

	out := Results{
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
					i, numTests, util.Percent(i, numTests),
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

// TestRoutine repeatedly runs the dEQP test executable exe with the tests
// taken from tests. The output of the dEQP test is parsed, and the test result
// is written to results.
// TestRoutine only returns once the tests chan has been closed.
// TestRoutine does not close the results chan.
func (c *Config) TestRoutine(exe string, tests <-chan string, results chan<- TestResult, goroutineIndex int) {
	// Context for the GCOV_PREFIX environment variable:
	// If you compile SwiftShader with gcc and the --coverage flag, the build will contain coverage instrumentation.
	// We can use this to get the code coverage of SwiftShader from running dEQP.
	// The coverage instrumentation reads the existing coverage files on start-up (at a hardcoded path alongside the
	// SwiftShader build), updates coverage info as the programs runs, then (over)writes the coverage files on exit.
	// Thus, multiple parallel processes will race when updating coverage information. The GCOV_PREFIX environment
	// variable adds a prefix to the hardcoded paths.
	// E.g. Given GCOV_PREFIX=/tmp/coverage, the hardcoded path /ss/build/a.gcno becomes /tmp/coverage/ss/build/a.gcno.
	// This is mainly intended for running the target program on a different machine where the hardcoded paths don't
	// make sense. It can also be used to avoid races. It would be trivial to avoid races if the GCOV_PREFIX variable
	// supported macro variables like the Clang code coverage "%p" variable that expands to the process ID; in this
	// case, we could use GCOV_PREFIX=/tmp/coverage/%p to avoid races. Unfortunately, gcc does not support this.
	// Furthermore, processing coverage information from many directories can be slow; we start a lot of dEQP child
	// processes, each of which will likely get a unique process ID. In practice, we only need one directory per go
	// routine.

	// If GCOV_PREFIX is in Env, replace occurrences of "PROC_ID" in GCOV_PREFIX with goroutineIndex.
	// This avoids races between parallel child processes reading and writing coverage output files.
	// For example, GCOV_PREFIX="/tmp/gcov_output/PROC_ID" becomes GCOV_PREFIX="/tmp/gcov_output/1" in the first go routine.
	// You might expect PROC_ID to be the process ID of some process, but the only real requirement is that
	// it is a unique ID between the *parallel* child processes.
	env := make([]string, 0, len(c.Env))
	for _, v := range c.Env {
		if strings.HasPrefix(v, "GCOV_PREFIX=") {
			v = strings.ReplaceAll(v, "PROC_ID", strconv.Itoa(goroutineIndex))
		}
		env = append(env, v)
	}

nextTest:
	for name := range tests {
		// log.Printf("Running test '%s'\n", name)

		start := time.Now()
		outRaw, err := shell.Exec(c.TestTimeout, exe, filepath.Dir(exe), env,
			"--deqp-surface-type=pbuffer",
			"--deqp-shadercache=disable",
			"--deqp-log-images=disable",
			"--deqp-log-shader-sources=disable",
			"--deqp-log-flush=disable",
			"-n="+name)
		duration := time.Since(start)
		out := string(outRaw)
		out = strings.ReplaceAll(out, exe, "<dEQP>")
		for k, v := range c.LogReplacements {
			out = strings.ReplaceAll(out, k, v)
		}

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

		// Don't treat non-zero error codes as crashes.
		var exitErr *exec.ExitError
		if errors.As(err, &exitErr) {
			if exitErr.ExitCode() != 255 {
				out += fmt.Sprintf("\nProcess terminated with code %d", exitErr.ExitCode())
				err = nil
			}
		}

		switch err.(type) {
		default:
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
