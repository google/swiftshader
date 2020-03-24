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

// Package cov provides functions for consuming and combining llvm coverage
// information from multiple processes.
package cov

import (
	"bytes"
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"

	"../cause"
	"../llvm"
)

// Location describes a single line-column position in a source file.
type Location struct {
	Line, Column int
}

func (l Location) String() string {
	return fmt.Sprintf("%v:%v", l.Line, l.Column)
}

// Span describes a start and end interval in a source file.
type Span struct {
	Start, End Location
}

func (l Span) String() string {
	return fmt.Sprintf("%v-%v", l.Start, l.End)
}

// File describes the coverage spans in a single source file.
type File struct {
	Path  string
	Spans []Span
}

// Coverage describes the coverage spans for all the source files for a single
// process invocation.
type Coverage struct {
	Files []File
}

// Env holds the enviroment settings for performing coverage processing.
type Env struct {
	LLVM    llvm.Toolchain
	RootDir string // path to SwiftShader git root directory
	ExePath string // path to the executable binary
}

// AppendRuntimeEnv returns the environment variables env with the
// LLVM_PROFILE_FILE environment variable appended.
func AppendRuntimeEnv(env []string, coverageFile string) []string {
	return append(env, "LLVM_PROFILE_FILE="+coverageFile)
}

// Import uses the llvm-profdata and llvm-cov tools to import the coverage
// information from a .profraw file.
func (e Env) Import(profrawPath string) (*Coverage, error) {
	profdata := profrawPath + ".profdata"

	if err := exec.Command(e.LLVM.Profdata(), "merge", "-sparse", profrawPath, "-o", profdata).Run(); err != nil {
		return nil, cause.Wrap(err, "llvm-profdata errored")
	}
	defer os.Remove(profdata)

	args := []string{
		"export",
		e.ExePath,
		"-instr-profile=" + profdata,
		"-format=text",
	}
	if e.LLVM.Version.GreaterEqual(llvm.Version{Major: 9}) {
		// LLVM 9 has new flags that omit stuff we don't care about.
		args = append(args,
			"-skip-expansions",
			"-skip-functions",
		)
	}
	data, err := exec.Command(e.LLVM.Cov(), args...).Output()
	if err != nil {
		return nil, cause.Wrap(err, "llvm-cov errored")
	}

	c, err := e.parse(data)
	if err != nil {
		return nil, cause.Wrap(err, "Couldn't parse coverage json data")
	}

	return c, nil
}

// https://clang.llvm.org/docs/SourceBasedCodeCoverage.html
// https://stackoverflow.com/a/56792192
func (e Env) parse(raw []byte) (*Coverage, error) {
	// line int, col int, count int64, hasCount bool, isRegionEntry bool
	type segment []interface{}

	type file struct {
		// expansions ignored
		Name     string    `json:"filename"`
		Segments []segment `json:"segments"`
		// summary ignored
	}

	type data struct {
		Files []file `json:"files"`
	}

	root := struct {
		Data []data `json:"data"`
	}{}
	err := json.NewDecoder(bytes.NewReader(raw)).Decode(&root)
	if err != nil {
		return nil, err
	}

	c := &Coverage{Files: make([]File, 0, len(root.Data[0].Files))}
	for _, f := range root.Data[0].Files {
		relpath, err := filepath.Rel(e.RootDir, f.Name)
		if err != nil {
			return nil, err
		}
		if strings.HasPrefix(relpath, "..") {
			continue
		}
		file := File{Path: relpath}
		for sIdx := 0; sIdx+1 < len(f.Segments); sIdx++ {
			start := Location{(int)(f.Segments[sIdx][0].(float64)), (int)(f.Segments[sIdx][1].(float64))}
			end := Location{(int)(f.Segments[sIdx+1][0].(float64)), (int)(f.Segments[sIdx+1][1].(float64))}
			covered := f.Segments[sIdx][2].(float64) != 0
			if covered {
				if c := len(file.Spans); c > 0 && file.Spans[c-1].End == start {
					file.Spans[c-1].End = end
				} else {
					file.Spans = append(file.Spans, Span{start, end})
				}
			}
		}
		if len(file.Spans) > 0 {
			c.Files = append(c.Files, file)
		}
	}
	return c, nil
}

// Path is a tree node path formed from a list of strings
type Path []string

// Tree represents source code coverage across a tree of different processes.
// Each tree node is addressed by a Path.
type Tree struct {
	initialized bool
	strings     Strings
	spans       map[Span]SpanID
	testRoot    Test
	files       map[string]TestCoverageMap
}

func (t *Tree) init() {
	if !t.initialized {
		t.strings.m = map[string]StringID{}
		t.spans = map[Span]SpanID{}
		t.testRoot = newTest()
		t.files = map[string]TestCoverageMap{}
		t.initialized = true
	}
}

// Spans returns all the spans used by the tree
func (t *Tree) Spans() []Span {
	out := make([]Span, 0, len(t.spans))
	for span := range t.spans {
		out = append(out, span)
	}
	sort.Slice(out, func(i, j int) bool {
		if out[i].Start.Line < out[j].Start.Line {
			return true
		}
		if out[i].Start.Line > out[j].Start.Line {
			return false
		}
		return out[i].Start.Column < out[j].Start.Column
	})
	return out
}

// File returns the TestCoverageMap for the given file
func (t *Tree) File(path string) TestCoverageMap {
	return t.files[path]
}

// Tests returns the root test
func (t *Tree) Tests() *Test { return &t.testRoot }

// Strings returns the string table
func (t *Tree) Strings() Strings { return t.strings }

type indexedTest struct {
	index   TestIndex
	created bool
}

func (t *Tree) index(path Path) []indexedTest {
	out := make([]indexedTest, len(path))
	test := &t.testRoot
	for i, p := range path {
		name := t.strings.index(p)
		idx, ok := test.indices[name]
		if !ok {
			idx = TestIndex(len(test.children))
			test.children = append(test.children, newTest())
			test.indices[name] = idx
		}
		out[i] = indexedTest{idx, !ok}
		test = &test.children[idx]
	}
	return out
}

func (t *Tree) addSpans(spans []Span) SpanSet {
	out := make(SpanSet, len(spans))
	for _, s := range spans {
		id, ok := t.spans[s]
		if !ok {
			id = SpanID(len(t.spans))
			t.spans[s] = id
		}
		out[id] = struct{}{}
	}
	return out
}

// Add adds the coverage information cov to the tree node addressed by path.
func (t *Tree) Add(path Path, cov *Coverage) {
	t.init()

	tests := t.index(path)

nextFile:
	// For each file with coverage...
	for _, file := range cov.Files {
		// Lookup or create the file's test coverage map
		tcm, ok := t.files[file.Path]
		if !ok {
			tcm = TestCoverageMap{}
			t.files[file.Path] = tcm
		}

		// Add all the spans to the map, get the span ids
		spans := t.addSpans(file.Spans)

		// Starting from the test root, walk down the test tree.
		test := t.testRoot
		parent := (*TestCoverage)(nil)
		for _, indexedTest := range tests {
			if indexedTest.created {
				if parent != nil && len(test.children) == 1 {
					parent.Spans = parent.Spans.add(spans)
					delete(parent.Children, indexedTest.index)
				} else {
					tc := tcm.index(indexedTest.index)
					tc.Spans = spans
				}
				continue nextFile
			}

			test = test.children[indexedTest.index]
			tc := tcm.index(indexedTest.index)

			// If the tree node contains spans that are not in this new test,
			// we need to push those spans down to all the other children.
			if lower := tc.Spans.sub(spans); len(lower) > 0 {
				// push into each child node
				for i := range test.children {
					child := tc.Children.index(TestIndex(i))
					child.Spans = child.Spans.add(lower)
				}
				// remove from node
				tc.Spans = tc.Spans.sub(lower)
			}

			// The spans that are in the new test, but are not part of the tree
			// node carry propagating down.
			spans = spans.sub(tc.Spans)
			if len(spans) == 0 {
				continue nextFile
			}

			tcm = tc.Children
			parent = tc
		}
	}
}

// StringID is an identifier of a string
type StringID int

// Strings holds a map of string to identifier
type Strings struct {
	m map[string]StringID
	s []string
}

func (s *Strings) index(str string) StringID {
	i, ok := s.m[str]
	if !ok {
		i = StringID(len(s.s))
		s.s = append(s.s, str)
		s.m[str] = i
	}
	return i
}

// TestIndex is an child test index
type TestIndex int

// Test is an collection of named sub-tests
type Test struct {
	indices  map[StringID]TestIndex
	children []Test
}

func newTest() Test {
	return Test{
		indices: map[StringID]TestIndex{},
	}
}

type namedIndex struct {
	name string
	idx  TestIndex
}

func (t Test) byName(s Strings) []namedIndex {
	out := make([]namedIndex, len(t.children))
	for id, idx := range t.indices {
		out[idx] = namedIndex{s.s[id], idx}
	}
	sort.Slice(out, func(i, j int) bool { return out[i].name < out[j].name })
	return out
}

func (t Test) String(s Strings) string {
	sb := strings.Builder{}
	for i, n := range t.byName(s) {
		child := t.children[n.idx]
		if i > 0 {
			sb.WriteString(" ")
		}
		sb.WriteString(n.name)
		if len(child.children) > 0 {
			sb.WriteString(fmt.Sprintf(":%v", child.String(s)))
		}
	}
	return "{" + sb.String() + "}"
}

// TestCoverage holds the coverage information for a deqp test group / leaf.
// For example:
// The deqp test group may hold spans that are common for all children, and may
// also optionally hold child nodes that describe coverage that differs per
// child test.
type TestCoverage struct {
	Spans    SpanSet
	Children TestCoverageMap
}

func (tc TestCoverage) String(t *Test, s Strings) string {
	sb := strings.Builder{}
	sb.WriteString(fmt.Sprintf("{%v", tc.Spans))
	if len(tc.Children) > 0 {
		sb.WriteString(" ")
		sb.WriteString(tc.Children.String(t, s))
	}
	sb.WriteString("}")
	return sb.String()
}

// TestCoverageMap is a map of TestIndex to *TestCoverage.
type TestCoverageMap map[TestIndex]*TestCoverage

func (tcm TestCoverageMap) String(t *Test, s Strings) string {
	sb := strings.Builder{}
	for _, n := range t.byName(s) {
		if child, ok := tcm[n.idx]; ok {
			sb.WriteString(fmt.Sprintf("\n%v: %v", n.name, child.String(&t.children[n.idx], s)))
		}
	}
	if sb.Len() > 0 {
		sb.WriteString("\n")
	}
	return indent(sb.String())
}

func newTestCoverage() *TestCoverage {
	return &TestCoverage{
		Children: TestCoverageMap{},
		Spans:    SpanSet{},
	}
}

func (tcm TestCoverageMap) index(idx TestIndex) *TestCoverage {
	tc, ok := tcm[idx]
	if !ok {
		tc = newTestCoverage()
		tcm[idx] = tc
	}
	return tc
}

// SpanID is an identifier of a span in a Tree.
type SpanID int

// SpanSet is a set of SpanIDs.
type SpanSet map[SpanID]struct{}

// List returns the full list of sorted span ids.
func (s SpanSet) List() []SpanID {
	out := make([]SpanID, 0, len(s))
	for span := range s {
		out = append(out, span)
	}
	sort.Slice(out, func(i, j int) bool { return out[i] < out[j] })
	return out
}

func (s SpanSet) String() string {
	sb := strings.Builder{}
	sb.WriteString(`[`)
	l := s.List()
	for i, span := range l {
		if i > 0 {
			sb.WriteString(`, `)
		}
		sb.WriteString(fmt.Sprintf("%v", span))
	}
	sb.WriteString(`]`)
	return sb.String()
}

func (s SpanSet) sub(rhs SpanSet) SpanSet {
	out := make(SpanSet, len(s))
	for span := range s {
		if _, found := rhs[span]; !found {
			out[span] = struct{}{}
		}
	}
	return out
}

func (s SpanSet) add(rhs SpanSet) SpanSet {
	out := make(SpanSet, len(s)+len(rhs))
	for span := range s {
		out[span] = struct{}{}
	}
	for span := range rhs {
		out[span] = struct{}{}
	}
	return out
}

func indent(s string) string {
	return strings.TrimSuffix(strings.ReplaceAll(s, "\n", "\n  "), "  ")
}

// JSON returns the full test tree serialized to JSON.
func (t *Tree) JSON() string {
	sb := &strings.Builder{}
	sb.WriteString(`{`)

	// write the strings
	sb.WriteString(`"n":[`)
	for i, s := range t.strings.s {
		if i > 0 {
			sb.WriteString(`,`)
		}
		sb.WriteString(`"`)
		sb.WriteString(s)
		sb.WriteString(`"`)
	}
	sb.WriteString(`]`)

	// write the tests
	sb.WriteString(`,"t":`)
	t.writeTestJSON(&t.testRoot, sb)

	// write the spans
	sb.WriteString(`,"s":`)
	t.writeSpansJSON(sb)

	// write the files
	sb.WriteString(`,"f":`)
	t.writeFilesJSON(sb)

	sb.WriteString(`}`)
	return sb.String()
}

func (t *Tree) writeTestJSON(test *Test, sb *strings.Builder) {
	names := map[int]StringID{}
	for name, idx := range test.indices {
		names[int(idx)] = name
	}

	sb.WriteString(`[`)
	for i, child := range test.children {
		if i > 0 {
			sb.WriteString(`,`)
		}
		sb.WriteString(`[`)
		sb.WriteString(fmt.Sprintf("%v,", names[i]))
		t.writeTestJSON(&child, sb)
		sb.WriteString(`]`)
	}

	sb.WriteString(`]`)
}

func (t *Tree) writeSpansJSON(sb *strings.Builder) {
	type spanAndID struct {
		span Span
		id   SpanID
	}
	spans := make([]spanAndID, 0, len(t.spans))
	for span, id := range t.spans {
		spans = append(spans, spanAndID{span, id})
	}
	sort.Slice(spans, func(i, j int) bool { return spans[i].id < spans[j].id })

	sb.WriteString(`[`)
	for i, s := range spans {
		if i > 0 {
			sb.WriteString(`,`)
		}
		span := s.span
		sb.WriteString(fmt.Sprintf("[%v,%v,%v,%v]",
			span.Start.Line, span.Start.Column,
			span.End.Line, span.End.Column))
	}

	sb.WriteString(`]`)
}

func (t *Tree) writeFilesJSON(sb *strings.Builder) {
	paths := make([]string, 0, len(t.files))
	for path := range t.files {
		paths = append(paths, path)
	}
	sort.Strings(paths)

	sb.WriteString(`{`)
	for i, path := range paths {
		if i > 0 {
			sb.WriteString(`,`)
		}
		sb.WriteString(`"`)
		sb.WriteString(path)
		sb.WriteString(`":`)
		t.writeCoverageMapJSON(t.files[path], sb)
	}

	sb.WriteString(`}`)
}

func (t *Tree) writeCoverageMapJSON(c TestCoverageMap, sb *strings.Builder) {
	ids := make([]TestIndex, 0, len(c))
	for id := range c {
		ids = append(ids, id)
	}
	sort.Slice(ids, func(i, j int) bool { return ids[i] < ids[j] })

	sb.WriteString(`[`)
	for i, id := range ids {
		if i > 0 {
			sb.WriteString(`,`)
		}

		sb.WriteString(`[`)
		sb.WriteString(fmt.Sprintf("%v", id))
		sb.WriteString(`,`)
		t.writeCoverageJSON(c[id], sb)
		sb.WriteString(`]`)
	}
	sb.WriteString(`]`)
}

func (t *Tree) writeCoverageJSON(c *TestCoverage, sb *strings.Builder) {
	sb.WriteString(`{`)
	comma := false
	if len(c.Spans) > 0 {
		sb.WriteString(`"s":[`)
		for i, spanID := range c.Spans.List() {
			if i > 0 {
				sb.WriteString(`,`)
			}
			sb.WriteString(fmt.Sprintf("%v", spanID))
		}
		sb.WriteString(`]`)
		comma = true
	}
	if len(c.Children) > 0 {
		if comma {
			sb.WriteString(`,`)
		}
		sb.WriteString(`"c":`)
		t.writeCoverageMapJSON(c.Children, sb)
	}
	sb.WriteString(`}`)
}
