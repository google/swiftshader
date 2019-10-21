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

// Package testlist provides utilities for handling test lists.
package testlist

import (
	"bytes"
	"crypto/sha1"
	"encoding/gob"
	"encoding/hex"
	"encoding/json"
	"io/ioutil"
	"path/filepath"
	"sort"
	"strings"

	"../cause"
)

// API is an enumerator of graphics APIs.
type API string

// Graphics APIs.
const (
	EGL    = API("egl")
	GLES2  = API("gles2")
	GLES3  = API("gles3")
	Vulkan = API("vulkan")
)

// Group is a list of tests to be run for a single API.
type Group struct {
	Name  string
	File  string
	API   API
	Tests []string
}

// Filter returns a new Group that contains only tests that match the predicate.
func (g Group) Filter(pred func(string) bool) Group {
	out := Group{
		Name: g.Name,
		File: g.File,
		API:  g.API,
	}
	for _, test := range g.Tests {
		if pred(test) {
			out.Tests = append(out.Tests, test)
		}
	}
	return out
}

// Lists is the full list of tests to be run.
type Lists []Group

// Filter returns a new Lists that contains only tests that match the predicate.
func (l Lists) Filter(pred func(string) bool) Lists {
	out := Lists{}
	for _, group := range l {
		filtered := group.Filter(pred)
		if len(filtered.Tests) > 0 {
			out = append(out, filtered)
		}
	}
	return out
}

// Hash returns a SHA1 hash of the set of tests.
func (l Lists) Hash() string {
	h := sha1.New()
	if err := gob.NewEncoder(h).Encode(l); err != nil {
		panic(cause.Wrap(err, "Could not encode testlist to produce hash"))
	}
	return hex.EncodeToString(h.Sum(nil))
}

// Load loads the test list json file and returns the full set of tests.
func Load(root, jsonPath string) (Lists, error) {
	root, err := filepath.Abs(root)
	if err != nil {
		return nil, cause.Wrap(err, "Couldn't get absolute path of '%s'", root)
	}

	jsonPath, err = filepath.Abs(jsonPath)
	if err != nil {
		return nil, cause.Wrap(err, "Couldn't get absolute path of '%s'", jsonPath)
	}

	i, err := ioutil.ReadFile(jsonPath)
	if err != nil {
		return nil, cause.Wrap(err, "Couldn't read test list from '%s'", jsonPath)
	}

	var jsonGroups []struct {
		Name     string
		API      string
		TestFile string `json:"tests"`
	}
	if err := json.NewDecoder(bytes.NewReader(i)).Decode(&jsonGroups); err != nil {
		return nil, cause.Wrap(err, "Couldn't parse '%s'", jsonPath)
	}

	dir := filepath.Dir(jsonPath)

	out := make(Lists, len(jsonGroups))
	for i, jsonGroup := range jsonGroups {
		path := filepath.Join(dir, jsonGroup.TestFile)
		tests, err := ioutil.ReadFile(path)
		if err != nil {
			return nil, cause.Wrap(err, "Couldn't read '%s'", tests)
		}
		relPath, err := filepath.Rel(root, path)
		if err != nil {
			return nil, cause.Wrap(err, "Couldn't get relative path for '%s'", path)
		}
		group := Group{
			Name: jsonGroup.Name,
			File: relPath,
			API:  API(jsonGroup.API),
		}
		for _, line := range strings.Split(string(tests), "\n") {
			line = strings.TrimSpace(line)
			if line != "" && !strings.HasPrefix(line, "#") {
				group.Tests = append(group.Tests, line)
			}
		}
		sort.Strings(group.Tests)
		out[i] = group
	}

	return out, nil
}

// Status is an enumerator of test results.
type Status string

const (
	// Pass is the status of a successful test.
	Pass = Status("PASS")
	// Fail is the status of a failed test.
	Fail = Status("FAIL")
	// Timeout is the status of a test that failed to complete in the alloted
	// time.
	Timeout = Status("TIMEOUT")
	// Crash is the status of a test that crashed.
	Crash = Status("CRASH")
	// Unimplemented is the status of a test that failed with UNIMPLEMENTED().
	Unimplemented = Status("UNIMPLEMENTED")
	// Unsupported is the status of a test that failed with UNSUPPORTED().
	Unsupported = Status("UNSUPPORTED")
	// Unreachable is the status of a test that failed with UNREACHABLE().
	Unreachable = Status("UNREACHABLE")
	// Assert is the status of a test that failed with ASSERT() or ASSERT_MSG().
	Assert = Status("ASSERT")
	// Abort is the status of a test that failed with ABORT().
	Abort = Status("ABORT")
	// NotSupported is the status of a test feature not supported by the driver.
	NotSupported = Status("NOT_SUPPORTED")
	// CompatibilityWarning is the status passing test with a warning.
	CompatibilityWarning = Status("COMPATIBILITY_WARNING")
	// QualityWarning is the status passing test with a warning.
	QualityWarning = Status("QUALITY_WARNING")
)

// Statuses is the full list of status types
var Statuses = []Status{
	Pass,
	Fail,
	Timeout,
	Crash,
	Unimplemented,
	Unsupported,
	Unreachable,
	Assert,
	Abort,
	NotSupported,
	CompatibilityWarning,
	QualityWarning,
}

// Failing returns true if the task status requires fixing.
func (s Status) Failing() bool {
	switch s {
	case Fail, Timeout, Crash, Unimplemented, Unreachable, Assert, Abort:
		return true
	case Unsupported:
		// This may seem surprising that this should be a failure, however these
		// should not be reached, as dEQP should not be using features that are
		// not advertised.
		return true
	default:
		return false
	}
}

// Passing returns true if the task status is considered a pass.
func (s Status) Passing() bool {
	switch s {
	case Pass, CompatibilityWarning, QualityWarning:
		return true
	default:
		return false
	}
}

// FilePathWithStatus returns the path to the test list file with the status
// appended before the file extension.
func FilePathWithStatus(listPath string, status Status) string {
	ext := filepath.Ext(listPath)
	name := listPath[:len(listPath)-len(ext)]
	return name + "-" + string(status) + ext
}
