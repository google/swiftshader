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
	"log"
	"math/rand"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"strings"
	"time"

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
	filter        = flag.String("filter", "", "filter for test names. Start with a '/' to indicate regex")
	limit         = flag.Int("limit", 0, "only run a maximum of this number of tests")
	shuffle       = flag.Bool("shuffle", false, "shuffle tests")
	noResults     = flag.Bool("no-results", false, "disable generation of results.json file")
)

const testTimeout = time.Minute * 2

func run() error {
	group := testlist.Group{
		Name: "",
		File: *testList,
		API:  testlist.Vulkan,
	}
	if err := group.Load(); err != nil {
		return err
	}

	if *filter != "" {
		if strings.HasPrefix(*filter, "/") {
			re := regexp.MustCompile((*filter)[1:])
			group = group.Filter(re.MatchString)
		} else {
			group = group.Filter(func(name string) bool {
				ok, _ := filepath.Match(*filter, name)
				return ok
			})
		}
	}

	shell.MaxProcMemory = *maxProcMemory

	if *shuffle {
		rnd := rand.New(rand.NewSource(time.Now().UnixNano()))
		rnd.Shuffle(len(group.Tests), func(i, j int) { group.Tests[i], group.Tests[j] = group.Tests[j], group.Tests[i] })
	}

	if *limit != 0 && len(group.Tests) > *limit {
		group.Tests = group.Tests[:*limit]
	}

	log.Printf("Running %d tests...\n", len(group.Tests))

	config := deqp.Config{
		ExeEgl:           "",
		ExeGles2:         "",
		ExeGles3:         "",
		ExeVulkan:        *deqpVkBinary,
		Env:              os.Environ(),
		NumParallelTests: *numThreads,
		TestLists:        testlist.Lists{group},
		TestTimeout:      testTimeout,
	}

	res, err := config.Run()
	if err != nil {
		return err
	}

	counts := map[testlist.Status]int{}
	for _, r := range res.Tests {
		counts[r.Status] = counts[r.Status] + 1
	}
	for _, s := range testlist.Statuses {
		if count := counts[s]; count > 0 {
			log.Printf("%s: %d\n", string(s), count)
		}
	}

	if !*noResults {
		err = res.Save(*output)
		if err != nil {
			return err
		}
	}

	return nil
}

func main() {
	flag.ErrHelp = errors.New("regres is a tool to detect regressions between versions of SwiftShader")
	flag.Parse()
	if err := run(); err != nil {
		_, _ = fmt.Fprintln(os.Stderr, err)
		os.Exit(-1)
	}
}
