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

package cov

import (
	"fmt"
	"log"
	"sort"
	"strings"
	"sync"
)

type treeFile struct {
	tcm        TestCoverageMap
	spangroups map[SpanGroupID]SpanGroup
}

func newTreeFile() *treeFile {
	return &treeFile{
		tcm:        TestCoverageMap{},
		spangroups: map[SpanGroupID]SpanGroup{},
	}
}

// Tree represents source code coverage across a tree of different processes.
// Each tree node is addressed by a Path.
type Tree struct {
	initialized bool
	strings     Strings
	spans       map[Span]SpanID
	testRoot    Test
	files       map[string]*treeFile
}

func (t *Tree) init() {
	if !t.initialized {
		t.strings.m = map[string]StringID{}
		t.spans = map[Span]SpanID{}
		t.testRoot = newTest()
		t.files = map[string]*treeFile{}
		t.initialized = true
	}
}

// SpanList is a list of Spans
type SpanList []Span

// Compare returns -1 if l comes before o, 1 if l comes after o, otherwise 0.
func (l SpanList) Compare(o SpanList) int {
	switch {
	case len(l) < len(o):
		return -1
	case len(l) > len(o):
		return 1
	}
	for i, a := range l {
		switch a.Compare(o[i]) {
		case -1:
			return -1
		case 1:
			return 1
		}
	}
	return 0
}

// Spans returns all the spans used by the tree
func (t *Tree) Spans() SpanList {
	out := make(SpanList, 0, len(t.spans))
	for span := range t.spans {
		out = append(out, span)
	}
	sort.Slice(out, func(i, j int) bool { return out[i].Before(out[j]) })
	return out
}

// FileCoverage returns the TestCoverageMap for the given file
func (t *Tree) FileCoverage(path string) TestCoverageMap {
	return t.files[path].tcm
}

// Tests returns the root test
func (t *Tree) Tests() *Test { return &t.testRoot }

// Strings returns the string table
func (t *Tree) Strings() Strings { return t.strings }

func (t *Tree) index(path Path) []indexedTest {
	out := make([]indexedTest, len(path))
	test := &t.testRoot
	for i, p := range path {
		name := t.strings.index(p)
		test, out[i] = test.index(name)
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
		tf, ok := t.files[file.Path]
		if !ok {
			tf = newTreeFile()
			t.files[file.Path] = tf
		}

		// Add all the spans to the map, get the span ids
		spans := t.addSpans(file.Spans)

		// Starting from the test root, walk down the test tree.
		tcm, test := tf.tcm, t.testRoot
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

type indexedTest struct {
	index   TestIndex
	created bool
}

func (t *Test) index(name StringID) (*Test, indexedTest) {
	idx, ok := t.indices[name]
	if !ok {
		idx = TestIndex(len(t.children))
		t.children = append(t.children, newTest())
		t.indices[name] = idx
	}
	return &t.children[idx], indexedTest{idx, !ok}
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
	Group    *SpanGroupID
	Children TestCoverageMap
}

func (tc TestCoverage) String(t *Test, s Strings) string {
	sb := strings.Builder{}
	sb.WriteString("{")
	if len(tc.Spans) > 0 {
		sb.WriteString(tc.Spans.String())
	}
	if tc.Group != nil {
		sb.WriteString(" <")
		sb.WriteString(fmt.Sprintf("%v", *tc.Group))
		sb.WriteString(">")
	}
	if len(tc.Children) > 0 {
		sb.WriteString(" ")
		sb.WriteString(tc.Children.String(t, s))
	}
	sb.WriteString("}")
	return sb.String()
}

// TestCoverageMap is a map of TestIndex to *TestCoverage.
type TestCoverageMap map[TestIndex]*TestCoverage

func (tcm TestCoverageMap) traverse(cb func(*TestCoverage)) {
	for _, tc := range tcm {
		cb(tc)
		tc.Children.traverse(cb)
	}
}

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

// SpanIDList is a list of SpanIDs
type SpanIDList []SpanID

// Compare returns -1 if l comes before o, 1 if l comes after o, otherwise 0.
func (l SpanIDList) Compare(o SpanIDList) int {
	switch {
	case len(l) < len(o):
		return -1
	case len(l) > len(o):
		return 1
	}
	for i, a := range l {
		b := o[i]
		switch {
		case a < b:
			return -1
		case a > b:
			return 1
		}
	}
	return 0
}

// List returns the full list of sorted span ids.
func (s SpanSet) List() SpanIDList {
	out := make(SpanIDList, 0, len(s))
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

func (s SpanSet) containsAll(rhs SpanSet) bool {
	for span := range rhs {
		if _, found := s[span]; !found {
			return false
		}
	}
	return true
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

// SpanGroupID is an identifier of a SpanGroup.
type SpanGroupID int

// SpanGroup holds a number of spans, potentially extending from another
// SpanGroup.
type SpanGroup struct {
	spans  SpanSet
	extend *SpanGroupID
}

func newSpanGroup() SpanGroup {
	return SpanGroup{spans: SpanSet{}}
}

func indent(s string) string {
	return strings.TrimSuffix(strings.ReplaceAll(s, "\n", "\n  "), "  ")
}

// Optimize optimizes the Tree by de-duplicating common spans into a tree of
// SpanGroups.
func (t *Tree) Optimize() {
	log.Printf("Optimizing coverage tree...")

	// Start by gathering all of the unique spansets
	wg := sync.WaitGroup{}
	wg.Add(len(t.files))
	for _, file := range t.files {
		file := file
		go func() {
			defer wg.Done()
			file.optimize()
		}()
	}
	wg.Wait()
}

func (f *treeFile) optimize() {
	const minSpansInGroup = 2

	type spansetKey string
	spansetMap := map[spansetKey]SpanSet{}

	f.tcm.traverse(func(tc *TestCoverage) {
		if len(tc.Spans) >= minSpansInGroup {
			key := spansetKey(tc.Spans.String())
			if _, ok := spansetMap[key]; !ok {
				spansetMap[key] = tc.Spans
			}
		}
	})

	if len(spansetMap) == 0 {
		return
	}

	type spansetInfo struct {
		key spansetKey
		set SpanSet // fully expanded set
		grp SpanGroup
		id  SpanGroupID
	}
	spansets := make([]*spansetInfo, 0, len(spansetMap))
	for key, set := range spansetMap {
		spansets = append(spansets, &spansetInfo{
			key: key,
			set: set,
			grp: SpanGroup{spans: set},
		})
	}

	// Sort by number of spans in each sets starting with the largest.
	sort.Slice(spansets, func(i, j int) bool {
		a, b := spansets[i].set, spansets[j].set
		switch {
		case len(a) > len(b):
			return true
		case len(a) < len(b):
			return false
		}
		return a.List().Compare(b.List()) == -1 // Just to keep output stable
	})

	// Assign IDs now that we have stable order.
	for i := range spansets {
		spansets[i].id = SpanGroupID(i)
	}

	// Loop over the spanGroups starting from the largest, and try to fold them
	// into the larger sets.
	// This is O(n^2) complexity.
nextSpan:
	for i, a := range spansets[:len(spansets)-1] {
		for _, b := range spansets[i+1:] {
			if len(a.set) > len(b.set) && a.set.containsAll(b.set) {
				extend := b.id // Do not take address of iterator!
				a.grp.spans = a.set.sub(b.set)
				a.grp.extend = &extend
				continue nextSpan
			}
		}
	}

	// Rebuild a map of spansetKey to SpanGroup
	spangroupMap := make(map[spansetKey]*spansetInfo, len(spansets))
	for _, s := range spansets {
		spangroupMap[s.key] = s
	}

	// Store the groups in the tree
	f.spangroups = make(map[SpanGroupID]SpanGroup, len(spansets))
	for _, s := range spansets {
		f.spangroups[s.id] = s.grp
	}

	// Update all the uses.
	f.tcm.traverse(func(tc *TestCoverage) {
		key := spansetKey(tc.Spans.String())
		if g, ok := spangroupMap[key]; ok {
			tc.Spans = nil
			tc.Group = &g.id
		}
	})
}
