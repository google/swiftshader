// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

// Package llvm provides functions and types for locating and using the llvm
// toolchains.
package llvm

import (
	"fmt"
	"os/exec"
	"path/filepath"
	"regexp"
	"runtime"
	"sort"
	"strconv"

	"../util"
)

const maxLLVMVersion = 10

// Version holds the build version information of an LLVM toolchain.
type Version struct {
	Major, Minor, Point int
}

// GreaterEqual returns true if v >= rhs.
func (v Version) GreaterEqual(rhs Version) bool {
	if v.Major > rhs.Major {
		return true
	}
	if v.Major < rhs.Major {
		return false
	}
	if v.Minor > rhs.Minor {
		return true
	}
	if v.Minor < rhs.Minor {
		return false
	}
	return v.Point >= rhs.Point
}

// Toolchain holds the paths and version information about an LLVM toolchain.
type Toolchain struct {
	Version Version
	BinDir  string
}

// Toolchains is a list of Toolchain
type Toolchains []Toolchain

// FindAtLeast looks for a toolchain with the given version, returning the highest found version.
func (l Toolchains) FindAtLeast(v Version) *Toolchain {
	out := (*Toolchain)(nil)
	for _, t := range l {
		if t.Version.GreaterEqual(v) && (out == nil || out.Version.GreaterEqual(t.Version)) {
			t := t
			out = &t
		}
	}
	return out
}

// Search looks for llvm toolchains in paths.
// If paths is empty, then PATH is searched.
func Search(paths ...string) Toolchains {
	toolchains := map[Version]Toolchain{}
	search := func(name string) {
		if len(paths) > 0 {
			for _, path := range paths {
				if util.IsFile(path) {
					path = filepath.Dir(path)
				}
				if t := toolchain(path); t != nil {
					toolchains[t.Version] = *t
					continue
				}
				if t := toolchain(filepath.Join(path, "bin")); t != nil {
					toolchains[t.Version] = *t
					continue
				}
			}
		} else {
			path, err := exec.LookPath(name)
			if err == nil {
				if t := toolchain(filepath.Dir(path)); t != nil {
					toolchains[t.Version] = *t
				}
			}
		}
	}

	search("clang")
	for i := 8; i < maxLLVMVersion; i++ {
		search(fmt.Sprintf("clang-%d", i))
	}

	out := make([]Toolchain, 0, len(toolchains))
	for _, t := range toolchains {
		out = append(out, t)
	}
	sort.Slice(out, func(i, j int) bool { return out[i].Version.GreaterEqual(out[j].Version) })

	return out
}

// Cov returns the path to the llvm-cov executable.
func (t Toolchain) Cov() string {
	return filepath.Join(t.BinDir, "llvm-cov"+exeExt())
}

// Profdata returns the path to the llvm-profdata executable.
func (t Toolchain) Profdata() string {
	return filepath.Join(t.BinDir, "llvm-profdata"+exeExt())
}

func toolchain(dir string) *Toolchain {
	t := Toolchain{BinDir: dir}
	if t.resolve() {
		return &t
	}
	return nil
}

func (t *Toolchain) resolve() bool {
	if !util.IsFile(t.Profdata()) { // llvm-profdata doesn't have --version flag
		return false
	}
	version, ok := parseVersion(t.Cov())
	t.Version = version
	return ok
}

func exeExt() string {
	switch runtime.GOOS {
	case "windows":
		return ".exe"
	default:
		return ""
	}
}

var versionRE = regexp.MustCompile(`(?:clang|LLVM) version ([0-9]+)\.([0-9]+)\.([0-9]+)`)

func parseVersion(tool string) (Version, bool) {
	out, err := exec.Command(tool, "--version").Output()
	if err != nil {
		return Version{}, false
	}
	matches := versionRE.FindStringSubmatch(string(out))
	if len(matches) < 4 {
		return Version{}, false
	}
	major, majorErr := strconv.Atoi(matches[1])
	minor, minorErr := strconv.Atoi(matches[2])
	point, pointErr := strconv.Atoi(matches[3])
	if majorErr != nil || minorErr != nil || pointErr != nil {
		return Version{}, false
	}
	return Version{major, minor, point}, true
}
