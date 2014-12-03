// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

// Context.cpp: Implements the es2::Context class, managing all GL state and performing
// rendering operations. It is the GLES2 specific implementation of EGLContext.

#include "Context.h"

#include "main.h"
#include "mathutil.h"
#include "utilities.h"
#include "Fence.h"
#include "Program.h"
#include "Query.h"
#include "Renderbuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "libEGL/Display.h"
#include "libEGL/Surface.h"
#include "Common/Half.hpp"

#include <EGL/eglext.h>

#undef near
#undef far

namespace es2
{
Context::Context(const egl::Config *config, const Context *shareContext) : mConfig(config)
{
	sw::Context *context = new sw::Context();
	device = new es2::Device(context);

    mFenceHandleAllocator.setBaseHandle(0);

    setClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    mState.depthClearValue = 1.0f;
    mState.stencilClearValue = 0;

    mState.cullFace = false;
    mState.cullMode = GL_BACK;
    mState.frontFace = GL_CCW;
    mState.depthTest = false;
    mState.depthFunc = GL_LESS;
    mState.blend = false;
    mState.sourceBlendRGB = GL_ONE;
    mState.sourceBlendAlpha = GL_ONE;
    mState.destBlendRGB = GL_ZERO;
    mState.destBlendAlpha = GL_ZERO;
    mState.blendEquationRGB = GL_FUNC_ADD;
    mState.blendEquationAlpha = GL_FUNC_ADD;
    mState.blendColor.red = 0;
    mState.blendColor.green = 0;
    mState.blendColor.blue = 0;
    mState.blendColor.alpha = 0;
    mState.stencilTest = false;
    mState.stencilFunc = GL_ALWAYS;
    mState.stencilRef = 0;
    mState.stencilMask = -1;
    mState.stencilWritemask = -1;
    mState.stencilBackFunc = GL_ALWAYS;
    mState.stencilBackRef = 0;
    mState.stencilBackMask = - 1;
    mState.stencilBackWritemask = -1;
    mState.stencilFail = GL_KEEP;
    mState.stencilPassDepthFail = GL_KEEP;
    mState.stencilPassDepthPass = GL_KEEP;
    mState.stencilBackFail = GL_KEEP;
    mState.stencilBackPassDepthFail = GL_KEEP;
    mState.stencilBackPassDepthPass = GL_KEEP;
    mState.polygonOffsetFill = false;
    mState.polygonOffsetFactor = 0.0f;
    mState.polygonOffsetUnits = 0.0f;
    mState.sampleAlphaToCoverage = false;
    mState.sampleCoverage = false;
    mState.sampleCoverageValue = 1.0f;
    mState.sampleCoverageInvert = false;
    mState.scissorTest = false;
    mState.dither = true;
    mState.generateMipmapHint = GL_DONT_CARE;
    mState.fragmentShaderDerivativeHint = GL_DONT_CARE;

    mState.lineWidth = 1.0f;

    mState.viewportX = 0;
    mState.viewportY = 0;
    mState.viewportWidth = config->mDisplayMode.width;
    mState.viewportHeight = config->mDisplayMode.height;
    mState.zNear = 0.0f;
    mState.zFar = 1.0f;

    mState.scissorX = 0;
    mState.scissorY = 0;
    mState.scissorWidth = config->mDisplayMode.width;
    mState.scissorHeight = config->mDisplayMode.height;

    mState.colorMaskRed = true;
    mState.colorMaskGreen = true;
    mState.colorMaskBlue = true;
    mState.colorMaskAlpha = true;
    mState.depthMask = true;

    mState.activeSampler = 0;
    mState.elementArrayBuffer = 0;
	
    mState.packAlignment = 4;
    mState.unpackAlignment = 4;

	mState.program = 0;
	mState.colorBuffer = 0;
	mState.depthBuffer = 0;
	mState.stencilBuffer = 0;

    mInvalidEnum = false;
    mInvalidValue = false;
    mInvalidOperation = false;
    mOutOfMemory = false;
    mInvalidFramebufferOperation = false;

    mHasBeenCurrent = false;

    markAllStateDirty();
}

Context::~Context()
{
}

void Context::makeCurrent(egl::Surface *surface)
{
    if(!mHasBeenCurrent)
    {
        mState.viewportX = 0;
        mState.viewportY = 0;
        mState.viewportWidth = surface->getWidth();
        mState.viewportHeight = surface->getHeight();

        mState.scissorX = 0;
        mState.scissorY = 0;
        mState.scissorWidth = surface->getWidth();
        mState.scissorHeight = surface->getHeight();

        mHasBeenCurrent = true;
    }
    
    markAllStateDirty();
}

void Context::destroy()
{
	delete this;
}

int Context::getClientVersion()
{
	return 2;
}

// This function will set all of the state-related dirty flags, so that all state is set during next pre-draw.
void Context::markAllStateDirty()
{
    mAppliedProgramSerial = 0;

    mDepthStateDirty = true;
    mMaskStateDirty = true;
    mBlendStateDirty = true;
    mStencilStateDirty = true;
    mPolygonOffsetStateDirty = true;
    mSampleStateDirty = true;
    mDitherStateDirty = true;
    mFrontFaceDirty = true;
}

void Context::setClearColor(float red, float green, float blue, float alpha)
{
    mState.colorClearValue.red = red;
    mState.colorClearValue.green = green;
    mState.colorClearValue.blue = blue;
    mState.colorClearValue.alpha = alpha;
}

void Context::setClearDepth(float depth)
{
    mState.depthClearValue = depth;
}

void Context::setClearStencil(int stencil)
{
    mState.stencilClearValue = stencil;
}

void Context::setCullFace(bool enabled)
{
    mState.cullFace = enabled;
}

bool Context::isCullFaceEnabled() const
{
    return mState.cullFace;
}

void Context::setCullMode(GLenum mode)
{
   mState.cullMode = mode;
}

void Context::setFrontFace(GLenum front)
{
    if(mState.frontFace != front)
    {
        mState.frontFace = front;
        mFrontFaceDirty = true;
    }
}

void Context::setDepthTest(bool enabled)
{
    if(mState.depthTest != enabled)
    {
        mState.depthTest = enabled;
        mDepthStateDirty = true;
    }
}

bool Context::isDepthTestEnabled() const
{
    return mState.depthTest;
}

void Context::setDepthFunc(GLenum depthFunc)
{
    if(mState.depthFunc != depthFunc)
    {
        mState.depthFunc = depthFunc;
        mDepthStateDirty = true;
    }
}

void Context::setDepthRange(float zNear, float zFar)
{
    mState.zNear = zNear;
    mState.zFar = zFar;
}

void Context::setBlend(bool enabled)
{
    if(mState.blend != enabled)
    {
        mState.blend = enabled;
        mBlendStateDirty = true;
    }
}

bool Context::isBlendEnabled() const
{
    return mState.blend;
}

void Context::setBlendFactors(GLenum sourceRGB, GLenum destRGB, GLenum sourceAlpha, GLenum destAlpha)
{
    if(mState.sourceBlendRGB != sourceRGB ||
       mState.sourceBlendAlpha != sourceAlpha ||
       mState.destBlendRGB != destRGB ||
       mState.destBlendAlpha != destAlpha)
    {
        mState.sourceBlendRGB = sourceRGB;
        mState.destBlendRGB = destRGB;
        mState.sourceBlendAlpha = sourceAlpha;
        mState.destBlendAlpha = destAlpha;
        mBlendStateDirty = true;
    }
}

void Context::setBlendColor(float red, float green, float blue, float alpha)
{
    if(mState.blendColor.red != red ||
       mState.blendColor.green != green ||
       mState.blendColor.blue != blue ||
       mState.blendColor.alpha != alpha)
    {
        mState.blendColor.red = red;
        mState.blendColor.green = green;
        mState.blendColor.blue = blue;
        mState.blendColor.alpha = alpha;
        mBlendStateDirty = true;
    }
}

void Context::setBlendEquation(GLenum rgbEquation, GLenum alphaEquation)
{
    if(mState.blendEquationRGB != rgbEquation ||
       mState.blendEquationAlpha != alphaEquation)
    {
        mState.blendEquationRGB = rgbEquation;
        mState.blendEquationAlpha = alphaEquation;
        mBlendStateDirty = true;
    }
}

void Context::setStencilTest(bool enabled)
{
    if(mState.stencilTest != enabled)
    {
        mState.stencilTest = enabled;
        mStencilStateDirty = true;
    }
}

bool Context::isStencilTestEnabled() const
{
    return mState.stencilTest;
}

void Context::setStencilParams(GLenum stencilFunc, GLint stencilRef, GLuint stencilMask)
{
    if(mState.stencilFunc != stencilFunc ||
        mState.stencilRef != stencilRef ||
        mState.stencilMask != stencilMask)
    {
        mState.stencilFunc = stencilFunc;
        mState.stencilRef = (stencilRef > 0) ? stencilRef : 0;
        mState.stencilMask = stencilMask;
        mStencilStateDirty = true;
    }
}

void Context::setStencilBackParams(GLenum stencilBackFunc, GLint stencilBackRef, GLuint stencilBackMask)
{
    if(mState.stencilBackFunc != stencilBackFunc ||
        mState.stencilBackRef != stencilBackRef ||
        mState.stencilBackMask != stencilBackMask)
    {
        mState.stencilBackFunc = stencilBackFunc;
        mState.stencilBackRef = (stencilBackRef > 0) ? stencilBackRef : 0;
        mState.stencilBackMask = stencilBackMask;
        mStencilStateDirty = true;
    }
}

void Context::setStencilWritemask(GLuint stencilWritemask)
{
    if(mState.stencilWritemask != stencilWritemask)
    {
        mState.stencilWritemask = stencilWritemask;
        mStencilStateDirty = true;
    }
}

void Context::setStencilBackWritemask(GLuint stencilBackWritemask)
{
    if(mState.stencilBackWritemask != stencilBackWritemask)
    {
        mState.stencilBackWritemask = stencilBackWritemask;
        mStencilStateDirty = true;
    }
}

void Context::setStencilOperations(GLenum stencilFail, GLenum stencilPassDepthFail, GLenum stencilPassDepthPass)
{
    if(mState.stencilFail != stencilFail ||
        mState.stencilPassDepthFail != stencilPassDepthFail ||
        mState.stencilPassDepthPass != stencilPassDepthPass)
    {
        mState.stencilFail = stencilFail;
        mState.stencilPassDepthFail = stencilPassDepthFail;
        mState.stencilPassDepthPass = stencilPassDepthPass;
        mStencilStateDirty = true;
    }
}

void Context::setStencilBackOperations(GLenum stencilBackFail, GLenum stencilBackPassDepthFail, GLenum stencilBackPassDepthPass)
{
    if(mState.stencilBackFail != stencilBackFail ||
        mState.stencilBackPassDepthFail != stencilBackPassDepthFail ||
        mState.stencilBackPassDepthPass != stencilBackPassDepthPass)
    {
        mState.stencilBackFail = stencilBackFail;
        mState.stencilBackPassDepthFail = stencilBackPassDepthFail;
        mState.stencilBackPassDepthPass = stencilBackPassDepthPass;
        mStencilStateDirty = true;
    }
}

void Context::setPolygonOffsetFill(bool enabled)
{
    if(mState.polygonOffsetFill != enabled)
    {
        mState.polygonOffsetFill = enabled;
        mPolygonOffsetStateDirty = true;
    }
}

bool Context::isPolygonOffsetFillEnabled() const
{
    return mState.polygonOffsetFill;
}

void Context::setPolygonOffsetParams(GLfloat factor, GLfloat units)
{
    if(mState.polygonOffsetFactor != factor ||
        mState.polygonOffsetUnits != units)
    {
        mState.polygonOffsetFactor = factor;
        mState.polygonOffsetUnits = units;
        mPolygonOffsetStateDirty = true;
    }
}

void Context::setSampleAlphaToCoverage(bool enabled)
{
    if(mState.sampleAlphaToCoverage != enabled)
    {
        mState.sampleAlphaToCoverage = enabled;
        mSampleStateDirty = true;
    }
}

bool Context::isSampleAlphaToCoverageEnabled() const
{
    return mState.sampleAlphaToCoverage;
}

void Context::setSampleCoverage(bool enabled)
{
    if(mState.sampleCoverage != enabled)
    {
        mState.sampleCoverage = enabled;
        mSampleStateDirty = true;
    }
}

bool Context::isSampleCoverageEnabled() const
{
    return mState.sampleCoverage;
}

void Context::setSampleCoverageParams(GLclampf value, bool invert)
{
    if(mState.sampleCoverageValue != value ||
        mState.sampleCoverageInvert != invert)
    {
        mState.sampleCoverageValue = value;
        mState.sampleCoverageInvert = invert;
        mSampleStateDirty = true;
    }
}

void Context::setScissorTest(bool enabled)
{
    mState.scissorTest = enabled;
}

bool Context::isScissorTestEnabled() const
{
    return mState.scissorTest;
}

void Context::setDither(bool enabled)
{
    if(mState.dither != enabled)
    {
        mState.dither = enabled;
        mDitherStateDirty = true;
    }
}

bool Context::isDitherEnabled() const
{
    return mState.dither;
}

void Context::setLineWidth(GLfloat width)
{
    mState.lineWidth = width;
}

void Context::setGenerateMipmapHint(GLenum hint)
{
    mState.generateMipmapHint = hint;
}

void Context::setFragmentShaderDerivativeHint(GLenum hint)
{
    mState.fragmentShaderDerivativeHint = hint;
    // TODO: Propagate the hint to shader translator so we can write
    // ddx, ddx_coarse, or ddx_fine depending on the hint.
    // Ignore for now. It is valid for implementations to ignore hint.
}

void Context::setViewportParams(GLint x, GLint y, GLsizei width, GLsizei height)
{
    mState.viewportX = x;
    mState.viewportY = y;
    mState.viewportWidth = width;
    mState.viewportHeight = height;
}

void Context::setScissorParams(GLint x, GLint y, GLsizei width, GLsizei height)
{
    mState.scissorX = x;
    mState.scissorY = y;
    mState.scissorWidth = width;
    mState.scissorHeight = height;
}

void Context::setColorMask(bool red, bool green, bool blue, bool alpha)
{
    if(mState.colorMaskRed != red || mState.colorMaskGreen != green ||
       mState.colorMaskBlue != blue || mState.colorMaskAlpha != alpha)
    {
        mState.colorMaskRed = red;
        mState.colorMaskGreen = green;
        mState.colorMaskBlue = blue;
        mState.colorMaskAlpha = alpha;
        mMaskStateDirty = true;
    }
}

void Context::setDepthMask(bool mask)
{
    if(mState.depthMask != mask)
    {
        mState.depthMask = mask;
        mMaskStateDirty = true;
    }
}

void Context::setActiveSampler(unsigned int active)
{
    mState.activeSampler = active;
}

GLuint Context::getActiveQuery(GLenum target) const
{
    Query *queryObject = NULL;
    
    switch(target)
    {
    case GL_ANY_SAMPLES_PASSED_EXT:
        queryObject = mState.activeQuery[QUERY_ANY_SAMPLES_PASSED].get();
        break;
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT:
        queryObject = mState.activeQuery[QUERY_ANY_SAMPLES_PASSED_CONSERVATIVE].get();
        break;
    default:
        ASSERT(false);
    }

    if(queryObject)
    {
        return queryObject->id();
    }
    
	return 0;
}

void Context::setEnableVertexAttribArray(unsigned int attribNum, bool enabled)
{
    mState.vertexAttribute[attribNum].mArrayEnabled = enabled;
}

const VertexAttribute &Context::getVertexAttribState(unsigned int attribNum)
{
    return mState.vertexAttribute[attribNum];
}

void Context::setVertexAttribState(unsigned int attribNum, sw::Resource *buffer, GLint size, GLenum type, bool normalized,
                                   GLsizei stride, intptr_t offset)
{
    mState.vertexAttribute[attribNum].buffer = buffer;
    mState.vertexAttribute[attribNum].mSize = size;
    mState.vertexAttribute[attribNum].mType = type;
    mState.vertexAttribute[attribNum].mNormalized = normalized;
    mState.vertexAttribute[attribNum].mStride = stride;
    mState.vertexAttribute[attribNum].mOffset = offset;
}

void Context::setPackAlignment(GLint alignment)
{
    mState.packAlignment = alignment;
}

GLint Context::getPackAlignment() const
{
    return mState.packAlignment;
}

void Context::setUnpackAlignment(GLint alignment)
{
    mState.unpackAlignment = alignment;
}

GLint Context::getUnpackAlignment() const
{
    return mState.unpackAlignment;
}

void Context::setRenderbufferStorage(RenderbufferStorage *renderbuffer)
{
    Renderbuffer *renderbufferObject = mState.renderbuffer.get();
    renderbufferObject->setStorage(renderbuffer);
}

// Applies the render target surface, depth stencil surface, viewport rectangle and scissor rectangle
bool Context::applyRenderTarget()
{
    egl::Image *renderTarget = mState.colorBuffer;//framebuffer->getRenderTarget();
	device->setRenderTarget(renderTarget);
	//if(renderTarget) renderTarget->release();

    egl::Image *depthStencil = mState.depthBuffer;//framebuffer->getDepthStencil();
    device->setDepthStencilSurface(depthStencil);
	//if(depthStencil) depthStencil->release();

    Viewport viewport;
    float zNear = clamp01(mState.zNear);
    float zFar = clamp01(mState.zFar);

    viewport.x0 = mState.viewportX;
    viewport.y0 = mState.viewportY;
    viewport.width = mState.viewportWidth;
    viewport.height = mState.viewportHeight;
    viewport.minZ = zNear;
    viewport.maxZ = zFar;

    device->setViewport(viewport);

    if(mState.scissorTest)
    {
		sw::Rect scissor = {mState.scissorX, mState.scissorY, mState.scissorX + mState.scissorWidth, mState.scissorY + mState.scissorHeight};
		scissor.clip(0, 0, renderTarget->getWidth(), renderTarget->getHeight());
        
		device->setScissorRect(scissor);
        device->setScissorEnable(true);
    }
    else
    {
        device->setScissorEnable(false);
    }

	Program *program = mState.program;

	if(program)
	{
		GLfloat nearFarDiff[3] = {zNear, zFar, zFar - zNear};
        program->setUniform1fv(program->getUniformLocation("gl_DepthRange.near"), 1, &nearFarDiff[0]);
		program->setUniform1fv(program->getUniformLocation("gl_DepthRange.far"), 1, &nearFarDiff[1]);
		program->setUniform1fv(program->getUniformLocation("gl_DepthRange.diff"), 1, &nearFarDiff[2]);
    }

    return true;
}

// Applies the fixed-function state (culling, depth test, alpha blending, stenciling, etc)
void Context::applyState(GLenum drawMode)
{
    if(mState.cullFace)
    {
        device->setCullMode(rad2sw::ConvertCullMode(mState.cullMode, mState.frontFace));
    }
    else
    {
		device->setCullMode(sw::CULL_NONE);
    }

    if(mDepthStateDirty)
    {
        if(mState.depthTest)
        {
			device->setDepthBufferEnable(true);
			device->setDepthCompare(rad2sw::ConvertDepthComparison(mState.depthFunc));
        }
        else
        {
            device->setDepthBufferEnable(false);
        }

        mDepthStateDirty = false;
    }

    if(mBlendStateDirty)
    {
        if(mState.blend)
        {
			device->setAlphaBlendEnable(true);
			device->setSeparateAlphaBlendEnable(true);

            device->setBlendConstant(rad2sw::ConvertColor(mState.blendColor));

			device->setSourceBlendFactor(rad2sw::ConvertBlendFunc(mState.sourceBlendRGB));
			device->setDestBlendFactor(rad2sw::ConvertBlendFunc(mState.destBlendRGB));
			device->setBlendOperation(rad2sw::ConvertBlendOp(mState.blendEquationRGB));

            device->setSourceBlendFactorAlpha(rad2sw::ConvertBlendFunc(mState.sourceBlendAlpha));
			device->setDestBlendFactorAlpha(rad2sw::ConvertBlendFunc(mState.destBlendAlpha));
			device->setBlendOperationAlpha(rad2sw::ConvertBlendOp(mState.blendEquationAlpha));
        }
        else
        {
			device->setAlphaBlendEnable(false);
        }

        mBlendStateDirty = false;
    }

    if(mStencilStateDirty || mFrontFaceDirty)
    {
        if(mState.stencilTest && mState.stencilBuffer)
        {
			device->setStencilEnable(true);
			device->setTwoSidedStencil(true);

            if(mState.stencilWritemask != mState.stencilBackWritemask || 
               mState.stencilRef != mState.stencilBackRef || 
               mState.stencilMask != mState.stencilBackMask)
            {
				ERR("Separate front/back stencil writemasks, reference values, or stencil mask values are invalid under WebGL.");
                return error(GL_INVALID_OPERATION);
            }

            // get the maximum size of the stencil ref
            egl::Image *stencilbuffer = mState.stencilBuffer;
            GLuint maxStencil = (1 << sw2rad::GetStencilSize(stencilbuffer->getInternalFormat())) - 1;

			if(mState.frontFace == GL_CCW)
			{
				device->setStencilWriteMask(mState.stencilWritemask);
				device->setStencilCompare(rad2sw::ConvertStencilComparison(mState.stencilFunc));

				device->setStencilReference((mState.stencilRef < (GLint)maxStencil) ? mState.stencilRef : maxStencil);
				device->setStencilMask(mState.stencilMask);

				device->setStencilFailOperation(rad2sw::ConvertStencilOp(mState.stencilFail));
				device->setStencilZFailOperation(rad2sw::ConvertStencilOp(mState.stencilPassDepthFail));
				device->setStencilPassOperation(rad2sw::ConvertStencilOp(mState.stencilPassDepthPass));

				device->setStencilWriteMaskCCW(mState.stencilBackWritemask);
				device->setStencilCompareCCW(rad2sw::ConvertStencilComparison(mState.stencilBackFunc));

				device->setStencilReferenceCCW((mState.stencilBackRef < (GLint)maxStencil) ? mState.stencilBackRef : maxStencil);
				device->setStencilMaskCCW(mState.stencilBackMask);

				device->setStencilFailOperationCCW(rad2sw::ConvertStencilOp(mState.stencilBackFail));
				device->setStencilZFailOperationCCW(rad2sw::ConvertStencilOp(mState.stencilBackPassDepthFail));
				device->setStencilPassOperationCCW(rad2sw::ConvertStencilOp(mState.stencilBackPassDepthPass));
			}
			else
			{
				device->setStencilWriteMaskCCW(mState.stencilWritemask);
				device->setStencilCompareCCW(rad2sw::ConvertStencilComparison(mState.stencilFunc));

				device->setStencilReferenceCCW((mState.stencilRef < (GLint)maxStencil) ? mState.stencilRef : maxStencil);
				device->setStencilMaskCCW(mState.stencilMask);

				device->setStencilFailOperationCCW(rad2sw::ConvertStencilOp(mState.stencilFail));
				device->setStencilZFailOperationCCW(rad2sw::ConvertStencilOp(mState.stencilPassDepthFail));
				device->setStencilPassOperationCCW(rad2sw::ConvertStencilOp(mState.stencilPassDepthPass));

				device->setStencilWriteMask(mState.stencilBackWritemask);
				device->setStencilCompare(rad2sw::ConvertStencilComparison(mState.stencilBackFunc));

				device->setStencilReference((mState.stencilBackRef < (GLint)maxStencil) ? mState.stencilBackRef : maxStencil);
				device->setStencilMask(mState.stencilBackMask);

				device->setStencilFailOperation(rad2sw::ConvertStencilOp(mState.stencilBackFail));
				device->setStencilZFailOperation(rad2sw::ConvertStencilOp(mState.stencilBackPassDepthFail));
				device->setStencilPassOperation(rad2sw::ConvertStencilOp(mState.stencilBackPassDepthPass));
			}
        }
        else
        {
			device->setStencilEnable(false);
        }

        mStencilStateDirty = false;
        mFrontFaceDirty = false;
    }

    if(mMaskStateDirty)
    {
		device->setColorWriteMask(0, rad2sw::ConvertColorMask(mState.colorMaskRed, mState.colorMaskGreen, mState.colorMaskBlue, mState.colorMaskAlpha));
		device->setDepthWriteEnable(mState.depthMask);

        mMaskStateDirty = false;
    }

    if(mPolygonOffsetStateDirty)
    {
        if(mState.polygonOffsetFill)
        {
            egl::Image *depthbuffer = mState.depthBuffer;
            if(depthbuffer)
            {
				device->setSlopeDepthBias(mState.polygonOffsetFactor);
                float depthBias = ldexp(mState.polygonOffsetUnits, -(int)(sw2rad::GetDepthSize(depthbuffer->getInternalFormat())));
				device->setDepthBias(depthBias);
            }
        }
        else
        {
            device->setSlopeDepthBias(0);
            device->setDepthBias(0);
        }

        mPolygonOffsetStateDirty = false;
    }

    if(mSampleStateDirty)
    {
        if(mState.sampleAlphaToCoverage)
        {
            device->setTransparencyAntialiasing(sw::TRANSPARENCY_ALPHA_TO_COVERAGE);
        }
		else
		{
			device->setTransparencyAntialiasing(sw::TRANSPARENCY_NONE);
		}

        if(mState.sampleCoverage)
        {
            unsigned int mask = 0;
            if(mState.sampleCoverageValue != 0)
            {
				int width = mState.colorBuffer->getWidth();
				int height = mState.colorBuffer->getHeight();
				int samples = mState.colorBuffer->getMultiSampleDepth();

                float threshold = 0.5f;

                for(int i = 0; i < samples; i++)
                {
                    mask <<= 1;

                    if((i + 1) * mState.sampleCoverageValue >= threshold)
                    {
                        threshold += 1.0f;
                        mask |= 1;
                    }
                }
            }
            
            if(mState.sampleCoverageInvert)
            {
                mask = ~mask;
            }

			device->setMultiSampleMask(mask);
        }
        else
        {
			device->setMultiSampleMask(0xFFFFFFFF);
        }

        mSampleStateDirty = false;
    }

    if(mDitherStateDirty)
    {
    //	UNIMPLEMENTED();   // FIXME

        mDitherStateDirty = false;
    }
}

struct TranslatedAttribute
{
    sw::StreamType type;
	int count;
	bool normalized;

    unsigned int offset;
    unsigned int stride;   // 0 means not to advance the read pointer at all

    sw::Resource *vertexBuffer;
};

GLenum Context::applyVertexBuffer(GLint base, GLint first)
{
    const VertexAttributeArray &attribs = mState.vertexAttribute;
    Program *program = mState.program;

	device->resetInputStreams(false);
    
    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        if(program->getAttributeStream(i) != -1 && attribs[i].mArrayEnabled)
        {
			unsigned int offset = first * attribs[i].stride() + attribs[i].mOffset;
			unsigned int stride = attribs[i].stride();
			sw::StreamType type;
               
			switch(attribs[i].mType)
			{
			case GL_BYTE:           type = sw::STREAMTYPE_SBYTE;  break;
			case GL_UNSIGNED_BYTE:  type = sw::STREAMTYPE_BYTE;   break;
			case GL_SHORT:          type = sw::STREAMTYPE_SHORT;  break;
			case GL_UNSIGNED_SHORT: type = sw::STREAMTYPE_USHORT; break;
			case GL_FIXED:          type = sw::STREAMTYPE_FIXED;  break;
			case GL_FLOAT:          type = sw::STREAMTYPE_FLOAT;  break;
			default: UNREACHABLE(); type = sw::STREAMTYPE_FLOAT;  break;
			}
			        
			sw::Resource *resource = attribs[i].buffer;
			const void *buffer = (char*)resource->data() + offset;
			buffer = (char*)buffer + stride * base;

			sw::Stream attribute(resource, buffer, stride);

			attribute.type = type;
			attribute.count = attribs[i].mSize;
			attribute.normalized = attribs[i].mNormalized;

			int stream = program->getAttributeStream(i);
			device->setInputStream(stream, attribute);
        }
    }

	return GL_NO_ERROR;
}

void Context::applyIndexBuffer()
{
	device->setIndexBuffer(mState.elementArrayBuffer);
}

// Applies the shaders and shader constants
void Context::applyShaders()
{
    Program *programObject = mState.program;
    sw::VertexShader *vertexShader = programObject->getVertexShader();
	sw::PixelShader *pixelShader = programObject->getPixelShader();

    device->setVertexShader(vertexShader);
    device->setPixelShader(pixelShader);

    if(programObject->getSerial() != mAppliedProgramSerial)
    {
        programObject->dirtyAllUniforms();
        mAppliedProgramSerial = programObject->getSerial();
    }

    programObject->applyUniforms();
}

void Context::applyTexture(sw::SamplerType type, int index, Texture *baseTexture)
{
	//Program *program = mState.program;
	int sampler = (type == sw::SAMPLER_PIXEL) ? index : 16 + index;
	bool textureUsed = true;

	//if(type == sw::SAMPLER_PIXEL)
	//{
	//	textureUsed = program->getPixelShader()->usesSampler(index);
	//}
	//else if(type == sw::SAMPLER_VERTEX)
	//{
	//	textureUsed = program->getVertexShader()->usesSampler(index);
	//}
	//else UNREACHABLE();

//	GLenum wrapS = baseTexture->getWrapS();
//    GLenum wrapT = baseTexture->getWrapT();
//    GLenum texFilter = baseTexture->getMinFilter();
//    GLenum magFilter = baseTexture->getMagFilter();
//	GLenum maxAnisotropy = baseTexture->getMaxAnisotropy();
//
//	device->setAddressingModeU(type, index, rad2sw::ConvertTextureWrap(wrapS));
//    device->setAddressingModeV(type, index, rad2sw::ConvertTextureWrap(wrapT));
//
//	sw::FilterType minFilter;
//	sw::MipmapType mipFilter;
//    rad2sw::ConvertMinFilter(texFilter, &minFilter, &mipFilter, maxAnisotropy);
////	ASSERT(minFilter == rad2sw::ConvertMagFilter(magFilter));

	device->setTextureFilter(type, index, sw::FILTER_LINEAR);
//	device->setTextureFilter(type, index, rad2sw::ConvertMagFilter(magFilter));
	device->setMipmapFilter(type, index, sw::MIPMAP_NONE);
	device->setMaxAnisotropy(type, index, 1.0f);   

	sw::Resource *resource = 0;

	if(baseTexture && textureUsed)
	{
		resource = baseTexture->getResource();
	}

	device->setTextureResource(sampler, resource);
			
	if(baseTexture && textureUsed)
	{
		int levelCount = baseTexture->getLevelCount();

		if(baseTexture->getTarget() == GL_TEXTURE_2D || baseTexture->getTarget() == GL_TEXTURE_EXTERNAL_OES)
		{
			Texture2D *texture = static_cast<Texture2D*>(baseTexture);

			for(int mipmapLevel = 0; mipmapLevel < MIPMAP_LEVELS; mipmapLevel++)
			{
				int surfaceLevel = mipmapLevel;

				if(surfaceLevel < 0)
				{
					surfaceLevel = 0;
				}
				else if(surfaceLevel >= levelCount)
				{
					surfaceLevel = levelCount - 1;
				}

				egl::Image *surface = texture->getImage(surfaceLevel);
				device->setTextureLevel(sampler, 0, mipmapLevel, surface, sw::TEXTURE_2D);
			}
		}
		else if(baseTexture->getTarget() == GL_TEXTURE_CUBE_MAP)
		{
			for(int face = 0; face < 6; face++)
			{
				TextureCubeMap *cubeTexture = static_cast<TextureCubeMap*>(baseTexture);

				for(int mipmapLevel = 0; mipmapLevel < MIPMAP_LEVELS; mipmapLevel++)
				{
					int surfaceLevel = mipmapLevel;

					if(surfaceLevel < 0)
					{
						surfaceLevel = 0;
					}
					else if(surfaceLevel >= levelCount)
					{
						surfaceLevel = levelCount - 1;
					}

					egl::Image *surface = cubeTexture->getImage(face, surfaceLevel);
					device->setTextureLevel(sampler, face, mipmapLevel, surface, sw::TEXTURE_CUBE);
				}
			}
		}
		else UNIMPLEMENTED();
	}
	else
	{
		device->setTextureLevel(sampler, 0, 0, 0, sw::TEXTURE_NULL);
	}
}

static std::size_t typeSize(GLenum type)
{
    switch(type)
    {
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    default: UNREACHABLE(); return sizeof(GLushort);
    }
}

void Context::drawArrays(GLenum mode, GLint first, GLsizei count)
{
    if(!mState.program)
    {
        return error(GL_INVALID_OPERATION);
    }

    PrimitiveType primitiveType;
    int primitiveCount;

    if(!rad2sw::ConvertPrimitiveType(mode, count, primitiveType, primitiveCount))
        return error(GL_INVALID_ENUM);

    if(primitiveCount <= 0)
    {
        return;
    }

    if(!applyRenderTarget())
    {
        return;
    }

    applyState(mode);

    GLenum err = applyVertexBuffer(0, first);
    if(err != GL_NO_ERROR)
    {
        return error(err);
    }

    applyShaders();

    if(!mState.program->validateSamplers(false))
    {
        return error(GL_INVALID_OPERATION);
    }

    if(!cullSkipsDraw(mode))
    {
        device->drawPrimitive(primitiveType, primitiveCount);
    }
}

void Context::drawElements(GLenum mode, GLsizei count, GLenum type, intptr_t offset)
{
    if(!mState.program || !mState.elementArrayBuffer)
    {
        return error(GL_INVALID_OPERATION);
    }

    PrimitiveType primitiveType;
    int primitiveCount;

    if(!rad2sw::ConvertPrimitiveType(mode, count, primitiveType, primitiveCount))
        return error(GL_INVALID_ENUM);

    if(primitiveCount <= 0)
    {
        return;
    }

    if(!applyRenderTarget())
    {
        return;
    }

    applyState(mode);

    applyIndexBuffer();
    
    GLenum err = applyVertexBuffer(0, 0);
    if(err != GL_NO_ERROR)
    {
        return error(err);
    }

    applyShaders();
    //applyTextures();

    if(!mState.program->validateSamplers(false))
    {
        return error(GL_INVALID_OPERATION);
    }

    if(!cullSkipsDraw(mode))
    {
		device->drawIndexedPrimitive(primitiveType, offset, primitiveCount, typeSize(type));
    }
}

void Context::finish()
{
	device->finish();
}

void Context::flush()
{
    // We don't queue anything without processing it as fast as possible
}

void Context::recordInvalidEnum()
{
    mInvalidEnum = true;
}

void Context::recordInvalidValue()
{
    mInvalidValue = true;
}

void Context::recordInvalidOperation()
{
    mInvalidOperation = true;
}

void Context::recordOutOfMemory()
{
    mOutOfMemory = true;
}

void Context::recordInvalidFramebufferOperation()
{
    mInvalidFramebufferOperation = true;
}

// Get one of the recorded errors and clear its flag, if any.
// [OpenGL ES 2.0.24] section 2.5 page 13.
GLenum Context::getError()
{
    if(mInvalidEnum)
    {
        mInvalidEnum = false;

        return GL_INVALID_ENUM;
    }

    if(mInvalidValue)
    {
        mInvalidValue = false;

        return GL_INVALID_VALUE;
    }

    if(mInvalidOperation)
    {
        mInvalidOperation = false;

        return GL_INVALID_OPERATION;
    }

    if(mOutOfMemory)
    {
        mOutOfMemory = false;

        return GL_OUT_OF_MEMORY;
    }

    if(mInvalidFramebufferOperation)
    {
        mInvalidFramebufferOperation = false;

        return GL_INVALID_FRAMEBUFFER_OPERATION;
    }

    return GL_NO_ERROR;
}

int Context::getSupportedMultiSampleDepth(sw::Format format, int requested)
{
    if(requested <= 1)
    {
        return 1;
    }
	
	if(requested == 2)
	{
		return 2;
	}
	
	return 4;
}

bool Context::cullSkipsDraw(GLenum drawMode)
{
    return mState.cullFace && mState.cullMode == GL_FRONT_AND_BACK && isTriangleMode(drawMode);
}

bool Context::isTriangleMode(GLenum drawMode)
{
    switch (drawMode)
    {
      case GL_TRIANGLES:
      case GL_TRIANGLE_FAN:
      case GL_TRIANGLE_STRIP:
        return true;
      case GL_POINTS:
      case GL_LINES:
      case GL_LINE_LOOP:
      case GL_LINE_STRIP:
        return false;
      default: UNREACHABLE();
    }

    return false;
}

Device *Context::getDevice()
{
	return device;
}

}

// Exported functions for use by EGL
extern "C"
{
	es2::Context *glCreateContext(const egl::Config *config, const es2::Context *shareContext)
	{
		return new es2::Context(config, shareContext);
	}
}
