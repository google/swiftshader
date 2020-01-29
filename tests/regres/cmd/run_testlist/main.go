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

// run_testlist is a tool runs a dEQP test list, using multiple sand-boxed
// processes.
//
// Unlike simply running deqp with its --deqp-caselist-file flag, run_testlist
// uses multiple sand-boxed processes, which greatly reduces testing time, and
// gracefully handles crashing processes.
package main

import (
	"errors"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"runtime"
	"sort"
	"strings"
	"time"

	"../../cause"
	"../../deqp"
	"../../shell"
	"../../testlist"
)

var (
	deqpVkBinary  = flag.String("deqp-vk", "deqp-vk", "path to the deqp-vk binary")
	testList      = flag.String("test-list", "vk-master-PASS.txt", "path to a test list file")
	numThreads    = flag.Int("num-threads", runtime.NumCPU(), "number of parallel test runner processes")
	maxProcMemory = flag.Uint64("max-proc-mem", shell.MaxProcMemory, "maximum virtual memory per child process")
	output        = flag.String("output", "results.json", "path to an output JSON results file")
)

const testTimeout = time.Minute * 2

func runTests() error {
	tests, err := ioutil.ReadFile(*testList)
	if err != nil {
		return cause.Wrap(err, "Couldn't read '%s'", *testList)
	}
	group := testlist.Group{
		Name: "",
		File: "",
		API:  testlist.Vulkan,
	}
	for _, line := range strings.Split(string(tests), "\n") {
		line = strings.TrimSpace(line)
		if line != "" && !strings.HasPrefix(line, "#") {
			group.Tests = append(group.Tests, line)
		}
	}
	sort.Strings(group.Tests)

	testLists := testlist.Lists{group}

	shell.MaxProcMemory = *maxProcMemory

	config := deqp.Config{
		ExeEgl:           "",
		ExeGles2:         "",
		ExeGles3:         "",
		ExeVulkan:        *deqpVkBinary,
		Env:              os.Environ(),
		NumParallelTests: *numThreads,
		TestLists:        testLists,
		TestTimeout:      testTimeout,
	}

	res, err := config.Run()
	if err != nil {
		return err
	}

	err = res.Save(*output)
	if err != nil {
		return err
	}

	return nil
}

func main() {
	flag.ErrHelp = errors.New("regres is a tool to detect regressions between versions of SwiftShader")
	flag.Parse()
	if err := runTests(); err != nil {
		_, _ = fmt.Fprintln(os.Stderr, err)
		os.Exit(-1)
	}
}
