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

// strip_unneeded is a tool that attempts to remove unnecessary lines from a
// file by running a test script after each marked line is removed.
//
// strip_unneeded will scan the file specified by the --file argument for lines
// that contain the substring specified by the --marker argument. One-by-one
// those marked lines will be removed from the file, after which the test script
// specified by --test will be run. If the test passes (the process completes
// with a 0 return code), then the line will remain removed, otherwise it is
// restored. This will repeat for every line in the file containing the marker,
// until all lines are tested.
package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"strings"
)

var (
	file   = flag.String("file", "", "file to modify")
	marker = flag.String("marker", "CHECK_NEEDED", "line token")
	test   = flag.String("test", "", "test script to run with each change")
)

func main() {
	if err := run(); err != nil {
		fmt.Println(err.Error())
		os.Exit(1)
	}
}

func run() error {
	flag.Parse()
	if *file == "" {
		return fmt.Errorf("Missing --file argument")
	}
	if *marker == "" {
		return fmt.Errorf("Missing --marker argument")
	}
	if *test == "" {
		return fmt.Errorf("Missing --test argument")
	}

	// make sure the test passes with no modifications
	if err := runTest(); err != nil {
		return fmt.Errorf("Test fails with no modifications.\n%v", err)
	}

	// load the test file
	body, err := ioutil.ReadFile(*file)
	if err != nil {
		return fmt.Errorf("Couldn't load file '%v'", *file)
	}

	// gather all the lines
	allLines := strings.Split(string(body), "\n")

	// find all the lines with the marker
	markerLines := make([]int, 0, len(allLines))
	for i, l := range allLines {
		if strings.Contains(l, *marker) {
			markerLines = append(markerLines, i)
		}
	}

	omit := map[int]bool{}

	save := func() error {
		f, err := os.Create(*file)
		if err != nil {
			return err
		}
		defer f.Close()
		for i, l := range allLines {
			if !omit[i] {
				f.WriteString(l)
				f.WriteString("\n")
			}
		}
		return nil
	}

	for i, l := range markerLines {
		omit[l] = true
		if err := save(); err != nil {
			return err
		}
		if err := runTest(); err != nil {
			omit[l] = false
			fmt.Printf("%d/%d: Test fails when removing line %v: %v\n", i, len(markerLines), l, allLines[l])
		} else {
			fmt.Printf("%d/%d: Test passes when removing line %v: %v\n", i, len(markerLines), l, allLines[l])
		}
	}

	return save()
}

func runTest() error {
	cmd := exec.Command("sh", "-c", *test)
	out, err := cmd.CombinedOutput()
	if err != nil {
		return fmt.Errorf("Test failed with error: %v\n%v", err, string(out))
	}
	return nil
}
