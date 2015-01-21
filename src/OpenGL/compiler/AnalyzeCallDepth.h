//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_ANALYZE_CALL_DEPTH_H_
#define COMPILER_ANALYZE_CALL_DEPTH_H_

#include "intermediate.h"

#include <set>
#include <limits.h>

// Traverses intermediate tree to analyze call depth or detect function recursion
class AnalyzeCallDepth : public TIntermTraverser
{
public:
    AnalyzeCallDepth(TIntermNode *root);
    ~AnalyzeCallDepth();

    virtual bool visitAggregate(Visit, TIntermAggregate*);

    unsigned int analyzeCallDepth();

private:
    class FunctionNode
	{
    public:
        FunctionNode(TIntermAggregate *node);

        const TString &getName() const;
        void addCallee(FunctionNode *callee);
		unsigned int analyzeCallDepth(AnalyzeCallDepth *analyzeCallDepth);
		unsigned int getLastDepth() const;

		void removeIfUnreachable();

    private:
        TIntermAggregate *const node;
        TVector<FunctionNode*> callees;

        Visit visit;
		unsigned int callDepth;
    };

    FunctionNode *findFunctionByName(const TString &name);
	
    std::vector<FunctionNode*> functions;
	typedef std::set<FunctionNode*> FunctionSet;
	FunctionSet globalFunctionCalls;
    FunctionNode *currentFunction;
};

#endif  // COMPILER_ANALYZE_CALL_DEPTH_H_
