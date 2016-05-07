// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#include "ValidateGlobalInitializer.h"

#include "ParseHelper.h"

namespace
{

class ValidateGlobalInitializerTraverser : public TIntermTraverser
{
public:
	ValidateGlobalInitializerTraverser(const TParseContext *context);

	void visitSymbol(TIntermSymbol *node) override;

	bool isValid() const { return mIsValid; }
	bool issueWarning() const { return mIssueWarning; }

private:
	const TParseContext *mContext;
	bool mIsValid;
	bool mIssueWarning;
};

void ValidateGlobalInitializerTraverser::visitSymbol(TIntermSymbol *node)
{
	const TSymbol *sym = mContext->symbolTable.find(node->getSymbol(), mContext->getShaderVersion());
	if (sym->isVariable())
	{
		// ESSL 1.00 section 4.3 (or ESSL 3.00 section 4.3):
		// Global initializers must be constant expressions.
		const TVariable *var = static_cast<const TVariable *>(sym);
		switch (var->getType().getQualifier())
		{
		case EvqConstExpr:
			break;
		case EvqGlobal:
		case EvqTemporary:
		case EvqUniform:
			// We allow these cases to be compatible with legacy ESSL 1.00 content.
			// Implement stricter rules for ESSL 3.00 since there's no legacy content to deal with.
			if (mContext->getShaderVersion() >= 300)
			{
				mIsValid = false;
			}
			else
			{
				mIssueWarning = true;
			}
			break;
		default:
			mIsValid = false;
		}
	}
}

ValidateGlobalInitializerTraverser::ValidateGlobalInitializerTraverser(const TParseContext *context)
	: TIntermTraverser(true, false, false),
	  mContext(context),
	  mIsValid(true),
	  mIssueWarning(false)
{
}

} // namespace

bool ValidateGlobalInitializer(TIntermTyped *initializer, const TParseContext *context, bool *warning)
{
	ValidateGlobalInitializerTraverser validate(context);
	initializer->traverse(&validate);
	ASSERT(warning != nullptr);
	*warning = validate.issueWarning();
	return validate.isValid();
}

