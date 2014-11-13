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
#include "ResourceManager.h"
#include "Buffer.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Program.h"
#include "Query.h"
#include "Renderbuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexDataManager.h"
#include "IndexDataManager.h"
#include "libEGL/Display.h"
#include "libEGL/Surface.h"
#include "Common/Half.hpp"

#include <EGL/eglext.h>

#undef near
#undef far

namespace es2
{
Device *Context::device = 0;

Context::Context(const egl::Config *config, const Context *shareContext) : mConfig(config)
{
	device = getDevice();

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

    if(shareContext != NULL)
    {
        mResourceManager = shareContext->mResourceManager;
        mResourceManager->addRef();
    }
    else
    {
        mResourceManager = new ResourceManager();
    }

    // [OpenGL ES 2.0.24] section 3.7 page 83:
    // In the initial state, TEXTURE_2D and TEXTURE_CUBE_MAP have twodimensional
    // and cube map texture state vectors respectively associated with them.
    // In order that access to these initial textures not be lost, they are treated as texture
    // objects all of whose names are 0.

    mTexture2DZero.set(new Texture2D(0));
    mTextureCubeMapZero.set(new TextureCubeMap(0));
    mTextureExternalZero.set(new TextureExternal(0));

    mState.activeSampler = 0;
    bindArrayBuffer(0);
    bindElementArrayBuffer(0);
    bindTextureCubeMap(0);
    bindTexture2D(0);
    bindReadFramebuffer(0);
    bindDrawFramebuffer(0);
    bindRenderbuffer(0);

    mState.currentProgram = 0;

    mState.packAlignment = 4;
    mState.unpackAlignment = 4;

    mVertexDataManager = NULL;
    mIndexDataManager = NULL;

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
    if(mState.currentProgram != 0)
    {
        Program *programObject = mResourceManager->getProgram(mState.currentProgram);
        if(programObject)
        {
            programObject->release();
        }
        mState.currentProgram = 0;
    }

    while(!mFramebufferMap.empty())
    {
        deleteFramebuffer(mFramebufferMap.begin()->first);
    }

    while(!mFenceMap.empty())
    {
        deleteFence(mFenceMap.begin()->first);
    }

	while(!mQueryMap.empty())
    {
        deleteQuery(mQueryMap.begin()->first);
    }

    for(int type = 0; type < TEXTURE_TYPE_COUNT; type++)
    {
        for(int sampler = 0; sampler < MAX_COMBINED_TEXTURE_IMAGE_UNITS; sampler++)
        {
            mState.samplerTexture[type][sampler].set(NULL);
        }
    }

    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        mState.vertexAttribute[i].mBoundBuffer.set(NULL);
    }

	for(int i = 0; i < QUERY_TYPE_COUNT; i++)
    {
        mState.activeQuery[i].set(NULL);
    }

    mState.arrayBuffer.set(NULL);
    mState.elementArrayBuffer.set(NULL);
    mState.renderbuffer.set(NULL);

    mTexture2DZero.set(NULL);
    mTextureCubeMapZero.set(NULL);
    mTextureExternalZero.set(NULL);

    delete mVertexDataManager;
    delete mIndexDataManager;

    mResourceManager->release();
}

void Context::makeCurrent(egl::Surface *surface)
{
    if(!mHasBeenCurrent)
    {
        mVertexDataManager = new VertexDataManager(this);
        mIndexDataManager = new IndexDataManager();

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

    // Wrap the existing resources into GL objects and assign them to the '0' names
    egl::Image *defaultRenderTarget = surface->getRenderTarget();
    egl::Image *depthStencil = surface->getDepthStencil();

    Colorbuffer *colorbufferZero = new Colorbuffer(defaultRenderTarget);
    DepthStencilbuffer *depthStencilbufferZero = new DepthStencilbuffer(depthStencil);
    Framebuffer *framebufferZero = new DefaultFramebuffer(colorbufferZero, depthStencilbufferZero);

    setFramebufferZero(framebufferZero);

    if(defaultRenderTarget)
    {
        defaultRenderTarget->release();
    }

    if(depthStencil)
    {
        depthStencil->release();
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

GLuint Context::getReadFramebufferHandle() const
{
    return mState.readFramebuffer;
}

GLuint Context::getDrawFramebufferHandle() const
{
    return mState.drawFramebuffer;
}

GLuint Context::getRenderbufferHandle() const
{
    return mState.renderbuffer.id();
}

GLuint Context::getArrayBufferHandle() const
{
    return mState.arrayBuffer.id();
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

void Context::setVertexAttribState(unsigned int attribNum, Buffer *boundBuffer, GLint size, GLenum type, bool normalized,
                                   GLsizei stride, const void *pointer)
{
    mState.vertexAttribute[attribNum].mBoundBuffer.set(boundBuffer);
    mState.vertexAttribute[attribNum].mSize = size;
    mState.vertexAttribute[attribNum].mType = type;
    mState.vertexAttribute[attribNum].mNormalized = normalized;
    mState.vertexAttribute[attribNum].mStride = stride;
    mState.vertexAttribute[attribNum].mPointer = pointer;
}

const void *Context::getVertexAttribPointer(unsigned int attribNum) const
{
    return mState.vertexAttribute[attribNum].mPointer;
}

const VertexAttributeArray &Context::getVertexAttributes()
{
    return mState.vertexAttribute;
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

GLuint Context::createBuffer()
{
    return mResourceManager->createBuffer();
}

GLuint Context::createProgram()
{
    return mResourceManager->createProgram();
}

GLuint Context::createShader(GLenum type)
{
    return mResourceManager->createShader(type);
}

GLuint Context::createTexture()
{
    return mResourceManager->createTexture();
}

GLuint Context::createRenderbuffer()
{
    return mResourceManager->createRenderbuffer();
}

// Returns an unused framebuffer name
GLuint Context::createFramebuffer()
{
    GLuint handle = mFramebufferHandleAllocator.allocate();

    mFramebufferMap[handle] = NULL;

    return handle;
}

GLuint Context::createFence()
{
    GLuint handle = mFenceHandleAllocator.allocate();

    mFenceMap[handle] = new Fence;

    return handle;
}

// Returns an unused query name
GLuint Context::createQuery()
{
    GLuint handle = mQueryHandleAllocator.allocate();

    mQueryMap[handle] = NULL;

    return handle;
}

void Context::deleteBuffer(GLuint buffer)
{
    if(mResourceManager->getBuffer(buffer))
    {
        detachBuffer(buffer);
    }
    
    mResourceManager->deleteBuffer(buffer);
}

void Context::deleteShader(GLuint shader)
{
    mResourceManager->deleteShader(shader);
}

void Context::deleteProgram(GLuint program)
{
    mResourceManager->deleteProgram(program);
}

void Context::deleteTexture(GLuint texture)
{
    if(mResourceManager->getTexture(texture))
    {
        detachTexture(texture);
    }

    mResourceManager->deleteTexture(texture);
}

void Context::deleteRenderbuffer(GLuint renderbuffer)
{
    if(mResourceManager->getRenderbuffer(renderbuffer))
    {
        detachRenderbuffer(renderbuffer);
    }
    
    mResourceManager->deleteRenderbuffer(renderbuffer);
}

void Context::deleteFramebuffer(GLuint framebuffer)
{
    FramebufferMap::iterator framebufferObject = mFramebufferMap.find(framebuffer);

    if(framebufferObject != mFramebufferMap.end())
    {
        detachFramebuffer(framebuffer);

        mFramebufferHandleAllocator.release(framebufferObject->first);
        delete framebufferObject->second;
        mFramebufferMap.erase(framebufferObject);
    }
}

void Context::deleteFence(GLuint fence)
{
    FenceMap::iterator fenceObject = mFenceMap.find(fence);

    if(fenceObject != mFenceMap.end())
    {
        mFenceHandleAllocator.release(fenceObject->first);
        delete fenceObject->second;
        mFenceMap.erase(fenceObject);
    }
}

void Context::deleteQuery(GLuint query)
{
    QueryMap::iterator queryObject = mQueryMap.find(query);
    
	if(queryObject != mQueryMap.end())
    {
        mQueryHandleAllocator.release(queryObject->first);
        
		if(queryObject->second)
        {
            queryObject->second->release();
        }
        
		mQueryMap.erase(queryObject);
    }
}

Buffer *Context::getBuffer(GLuint handle)
{
    return mResourceManager->getBuffer(handle);
}

Shader *Context::getShader(GLuint handle)
{
    return mResourceManager->getShader(handle);
}

Program *Context::getProgram(GLuint handle)
{
    return mResourceManager->getProgram(handle);
}

Texture *Context::getTexture(GLuint handle)
{
    return mResourceManager->getTexture(handle);
}

Renderbuffer *Context::getRenderbuffer(GLuint handle)
{
    return mResourceManager->getRenderbuffer(handle);
}

Framebuffer *Context::getReadFramebuffer()
{
    return getFramebuffer(mState.readFramebuffer);
}

Framebuffer *Context::getDrawFramebuffer()
{
    return getFramebuffer(mState.drawFramebuffer);
}

void Context::bindArrayBuffer(unsigned int buffer)
{
    mResourceManager->checkBufferAllocation(buffer);

    mState.arrayBuffer.set(getBuffer(buffer));
}

void Context::bindElementArrayBuffer(unsigned int buffer)
{
    mResourceManager->checkBufferAllocation(buffer);

    mState.elementArrayBuffer.set(getBuffer(buffer));
}

void Context::bindTexture2D(GLuint texture)
{
    mResourceManager->checkTextureAllocation(texture, TEXTURE_2D);

    mState.samplerTexture[TEXTURE_2D][mState.activeSampler].set(getTexture(texture));
}

void Context::bindTextureCubeMap(GLuint texture)
{
    mResourceManager->checkTextureAllocation(texture, TEXTURE_CUBE);

    mState.samplerTexture[TEXTURE_CUBE][mState.activeSampler].set(getTexture(texture));
}

void Context::bindTextureExternal(GLuint texture)
{
    mResourceManager->checkTextureAllocation(texture, TEXTURE_EXTERNAL);

    mState.samplerTexture[TEXTURE_EXTERNAL][mState.activeSampler].set(getTexture(texture));
}

void Context::bindReadFramebuffer(GLuint framebuffer)
{
    if(!getFramebuffer(framebuffer))
    {
        mFramebufferMap[framebuffer] = new Framebuffer();
    }

    mState.readFramebuffer = framebuffer;
}

void Context::bindDrawFramebuffer(GLuint framebuffer)
{
    if(!getFramebuffer(framebuffer))
    {
        mFramebufferMap[framebuffer] = new Framebuffer();
    }

    mState.drawFramebuffer = framebuffer;
}

void Context::bindRenderbuffer(GLuint renderbuffer)
{
    mResourceManager->checkRenderbufferAllocation(renderbuffer);

    mState.renderbuffer.set(getRenderbuffer(renderbuffer));
}

void Context::useProgram(GLuint program)
{
    GLuint priorProgram = mState.currentProgram;
    mState.currentProgram = program;               // Must switch before trying to delete, otherwise it only gets flagged.

    if(priorProgram != program)
    {
        Program *newProgram = mResourceManager->getProgram(program);
        Program *oldProgram = mResourceManager->getProgram(priorProgram);

        if(newProgram)
        {
            newProgram->addRef();
        }
        
        if(oldProgram)
        {
            oldProgram->release();
        }
    }
}

void Context::beginQuery(GLenum target, GLuint query)
{
    // From EXT_occlusion_query_boolean: If BeginQueryEXT is called with an <id>  
    // of zero, if the active query object name for <target> is non-zero (for the  
    // targets ANY_SAMPLES_PASSED_EXT and ANY_SAMPLES_PASSED_CONSERVATIVE_EXT, if  
    // the active query for either target is non-zero), if <id> is the name of an 
    // existing query object whose type does not match <target>, or if <id> is the
    // active query object name for any query type, the error INVALID_OPERATION is
    // generated.

    // Ensure no other queries are active
    // NOTE: If other queries than occlusion are supported, we will need to check
    // separately that:
    //    a) The query ID passed is not the current active query for any target/type
    //    b) There are no active queries for the requested target (and in the case
    //       of GL_ANY_SAMPLES_PASSED_EXT and GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT,
    //       no query may be active for either if glBeginQuery targets either.
    for(int i = 0; i < QUERY_TYPE_COUNT; i++)
    {
        if(mState.activeQuery[i].get() != NULL)
        {
            return error(GL_INVALID_OPERATION);
        }
    }

    QueryType qType;
    switch(target)
    {
    case GL_ANY_SAMPLES_PASSED_EXT: 
        qType = QUERY_ANY_SAMPLES_PASSED; 
        break;
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT: 
        qType = QUERY_ANY_SAMPLES_PASSED_CONSERVATIVE; 
        break;
    default: 
        ASSERT(false);
    }

    Query *queryObject = getQuery(query, true, target);

    // Check that name was obtained with glGenQueries
    if(!queryObject)
    {
        return error(GL_INVALID_OPERATION);
    }

    // Check for type mismatch
    if(queryObject->getType() != target)
    {
        return error(GL_INVALID_OPERATION);
    }

    // Set query as active for specified target
    mState.activeQuery[qType].set(queryObject);

    // Begin query
    queryObject->begin();
}

void Context::endQuery(GLenum target)
{
    QueryType qType;

    switch(target)
    {
    case GL_ANY_SAMPLES_PASSED_EXT: 
        qType = QUERY_ANY_SAMPLES_PASSED; 
        break;
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT: 
        qType = QUERY_ANY_SAMPLES_PASSED_CONSERVATIVE; 
        break;
    default: 
        ASSERT(false);
    }

    Query *queryObject = mState.activeQuery[qType].get();

    if(queryObject == NULL)
    {
        return error(GL_INVALID_OPERATION);
    }

    queryObject->end();

    mState.activeQuery[qType].set(NULL);
}

void Context::setFramebufferZero(Framebuffer *buffer)
{
    delete mFramebufferMap[0];
    mFramebufferMap[0] = buffer;
}

void Context::setRenderbufferStorage(RenderbufferStorage *renderbuffer)
{
    Renderbuffer *renderbufferObject = mState.renderbuffer.get();
    renderbufferObject->setStorage(renderbuffer);
}

Framebuffer *Context::getFramebuffer(unsigned int handle)
{
    FramebufferMap::iterator framebuffer = mFramebufferMap.find(handle);

    if(framebuffer == mFramebufferMap.end())
    {
        return NULL;
    }
    else
    {
        return framebuffer->second;
    }
}

Fence *Context::getFence(unsigned int handle)
{
    FenceMap::iterator fence = mFenceMap.find(handle);

    if(fence == mFenceMap.end())
    {
        return NULL;
    }
    else
    {
        return fence->second;
    }
}

Query *Context::getQuery(unsigned int handle, bool create, GLenum type)
{
    QueryMap::iterator query = mQueryMap.find(handle);

    if(query == mQueryMap.end())
    {
        return NULL;
    }
    else
    {
        if(!query->second && create)
        {
            query->second = new Query(handle, type);
            query->second->addRef();
        }

        return query->second;
    }
}

Buffer *Context::getArrayBuffer()
{
    return mState.arrayBuffer.get();
}

Buffer *Context::getElementArrayBuffer()
{
    return mState.elementArrayBuffer.get();
}

Program *Context::getCurrentProgram()
{
    return mResourceManager->getProgram(mState.currentProgram);
}

Texture2D *Context::getTexture2D()
{
    return static_cast<Texture2D*>(getSamplerTexture(mState.activeSampler, TEXTURE_2D));
}

TextureCubeMap *Context::getTextureCubeMap()
{
    return static_cast<TextureCubeMap*>(getSamplerTexture(mState.activeSampler, TEXTURE_CUBE));
}

TextureExternal *Context::getTextureExternal()
{
    return static_cast<TextureExternal*>(getSamplerTexture(mState.activeSampler, TEXTURE_EXTERNAL));
}

Texture *Context::getSamplerTexture(unsigned int sampler, TextureType type)
{
    GLuint texid = mState.samplerTexture[type][sampler].id();

    if(texid == 0)   // Special case: 0 refers to different initial textures based on the target
    {
        switch (type)
        {
        case TEXTURE_2D: return mTexture2DZero.get();
        case TEXTURE_CUBE: return mTextureCubeMapZero.get();
        case TEXTURE_EXTERNAL: return mTextureExternalZero.get();
        default: UNREACHABLE();
        }
    }

    return mState.samplerTexture[type][sampler].get();
}

bool Context::getBooleanv(GLenum pname, GLboolean *params)
{
    switch (pname)
    {
      case GL_SHADER_COMPILER:          *params = GL_TRUE;                          break;
      case GL_SAMPLE_COVERAGE_INVERT:   *params = mState.sampleCoverageInvert;      break;
      case GL_DEPTH_WRITEMASK:          *params = mState.depthMask;                 break;
      case GL_COLOR_WRITEMASK:
        params[0] = mState.colorMaskRed;
        params[1] = mState.colorMaskGreen;
        params[2] = mState.colorMaskBlue;
        params[3] = mState.colorMaskAlpha;
        break;
      case GL_CULL_FACE:                *params = mState.cullFace;                  break;
      case GL_POLYGON_OFFSET_FILL:      *params = mState.polygonOffsetFill;         break;
      case GL_SAMPLE_ALPHA_TO_COVERAGE: *params = mState.sampleAlphaToCoverage;     break;
      case GL_SAMPLE_COVERAGE:          *params = mState.sampleCoverage;            break;
      case GL_SCISSOR_TEST:             *params = mState.scissorTest;               break;
      case GL_STENCIL_TEST:             *params = mState.stencilTest;               break;
      case GL_DEPTH_TEST:               *params = mState.depthTest;                 break;
      case GL_BLEND:                    *params = mState.blend;                     break;
      case GL_DITHER:                   *params = mState.dither;                    break;
      default:
        return false;
    }

    return true;
}

bool Context::getFloatv(GLenum pname, GLfloat *params)
{
    // Please note: DEPTH_CLEAR_VALUE is included in our internal getFloatv implementation
    // because it is stored as a float, despite the fact that the GL ES 2.0 spec names
    // GetIntegerv as its native query function. As it would require conversion in any
    // case, this should make no difference to the calling application.
    switch (pname)
    {
      case GL_LINE_WIDTH:               *params = mState.lineWidth;            break;
      case GL_SAMPLE_COVERAGE_VALUE:    *params = mState.sampleCoverageValue;  break;
      case GL_DEPTH_CLEAR_VALUE:        *params = mState.depthClearValue;      break;
      case GL_POLYGON_OFFSET_FACTOR:    *params = mState.polygonOffsetFactor;  break;
      case GL_POLYGON_OFFSET_UNITS:     *params = mState.polygonOffsetUnits;   break;
      case GL_ALIASED_LINE_WIDTH_RANGE:
        params[0] = ALIASED_LINE_WIDTH_RANGE_MIN;
        params[1] = ALIASED_LINE_WIDTH_RANGE_MAX;
        break;
      case GL_ALIASED_POINT_SIZE_RANGE:
        params[0] = ALIASED_POINT_SIZE_RANGE_MIN;
        params[1] = ALIASED_POINT_SIZE_RANGE_MAX;
        break;
      case GL_DEPTH_RANGE:
        params[0] = mState.zNear;
        params[1] = mState.zFar;
        break;
      case GL_COLOR_CLEAR_VALUE:
        params[0] = mState.colorClearValue.red;
        params[1] = mState.colorClearValue.green;
        params[2] = mState.colorClearValue.blue;
        params[3] = mState.colorClearValue.alpha;
        break;
      case GL_BLEND_COLOR:
        params[0] = mState.blendColor.red;
        params[1] = mState.blendColor.green;
        params[2] = mState.blendColor.blue;
        params[3] = mState.blendColor.alpha;
        break;
	  case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
        *params = MAX_TEXTURE_MAX_ANISOTROPY;
		break;
      default:
        return false;
    }

    return true;
}

bool Context::getIntegerv(GLenum pname, GLint *params)
{
    // Please note: DEPTH_CLEAR_VALUE is not included in our internal getIntegerv implementation
    // because it is stored as a float, despite the fact that the GL ES 2.0 spec names
    // GetIntegerv as its native query function. As it would require conversion in any
    // case, this should make no difference to the calling application. You may find it in 
    // Context::getFloatv.
    switch (pname)
    {
    case GL_MAX_VERTEX_ATTRIBS:               *params = MAX_VERTEX_ATTRIBS;               break;
    case GL_MAX_VERTEX_UNIFORM_VECTORS:       *params = MAX_VERTEX_UNIFORM_VECTORS;       break;
    case GL_MAX_VARYING_VECTORS:              *params = MAX_VARYING_VECTORS;              break;
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: *params = MAX_COMBINED_TEXTURE_IMAGE_UNITS; break;
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:   *params = MAX_VERTEX_TEXTURE_IMAGE_UNITS;   break;
    case GL_MAX_TEXTURE_IMAGE_UNITS:          *params = MAX_TEXTURE_IMAGE_UNITS;          break;
	case GL_MAX_FRAGMENT_UNIFORM_VECTORS:     *params = MAX_FRAGMENT_UNIFORM_VECTORS;     break;
	case GL_MAX_RENDERBUFFER_SIZE:            *params = IMPLEMENTATION_MAX_RENDERBUFFER_SIZE; break;
    case GL_NUM_SHADER_BINARY_FORMATS:        *params = 0;                                    break;
    case GL_SHADER_BINARY_FORMATS:      /* no shader binary formats are supported */          break;
    case GL_ARRAY_BUFFER_BINDING:             *params = mState.arrayBuffer.id();              break;
    case GL_ELEMENT_ARRAY_BUFFER_BINDING:     *params = mState.elementArrayBuffer.id();       break;
//	case GL_FRAMEBUFFER_BINDING:            // now equivalent to GL_DRAW_FRAMEBUFFER_BINDING_ANGLE
    case GL_DRAW_FRAMEBUFFER_BINDING_ANGLE:   *params = mState.drawFramebuffer;               break;
    case GL_READ_FRAMEBUFFER_BINDING_ANGLE:   *params = mState.readFramebuffer;               break;
    case GL_RENDERBUFFER_BINDING:             *params = mState.renderbuffer.id();             break;
    case GL_CURRENT_PROGRAM:                  *params = mState.currentProgram;                break;
    case GL_PACK_ALIGNMENT:                   *params = mState.packAlignment;                 break;
    case GL_UNPACK_ALIGNMENT:                 *params = mState.unpackAlignment;               break;
    case GL_GENERATE_MIPMAP_HINT:             *params = mState.generateMipmapHint;            break;
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES: *params = mState.fragmentShaderDerivativeHint; break;
    case GL_ACTIVE_TEXTURE:                   *params = (mState.activeSampler + GL_TEXTURE0); break;
    case GL_STENCIL_FUNC:                     *params = mState.stencilFunc;                   break;
    case GL_STENCIL_REF:                      *params = mState.stencilRef;                    break;
    case GL_STENCIL_VALUE_MASK:               *params = mState.stencilMask;                   break;
    case GL_STENCIL_BACK_FUNC:                *params = mState.stencilBackFunc;               break;
    case GL_STENCIL_BACK_REF:                 *params = mState.stencilBackRef;                break;
    case GL_STENCIL_BACK_VALUE_MASK:          *params = mState.stencilBackMask;               break;
    case GL_STENCIL_FAIL:                     *params = mState.stencilFail;                   break;
    case GL_STENCIL_PASS_DEPTH_FAIL:          *params = mState.stencilPassDepthFail;          break;
    case GL_STENCIL_PASS_DEPTH_PASS:          *params = mState.stencilPassDepthPass;          break;
    case GL_STENCIL_BACK_FAIL:                *params = mState.stencilBackFail;               break;
    case GL_STENCIL_BACK_PASS_DEPTH_FAIL:     *params = mState.stencilBackPassDepthFail;      break;
    case GL_STENCIL_BACK_PASS_DEPTH_PASS:     *params = mState.stencilBackPassDepthPass;      break;
    case GL_DEPTH_FUNC:                       *params = mState.depthFunc;                     break;
    case GL_BLEND_SRC_RGB:                    *params = mState.sourceBlendRGB;                break;
    case GL_BLEND_SRC_ALPHA:                  *params = mState.sourceBlendAlpha;              break;
    case GL_BLEND_DST_RGB:                    *params = mState.destBlendRGB;                  break;
    case GL_BLEND_DST_ALPHA:                  *params = mState.destBlendAlpha;                break;
    case GL_BLEND_EQUATION_RGB:               *params = mState.blendEquationRGB;              break;
    case GL_BLEND_EQUATION_ALPHA:             *params = mState.blendEquationAlpha;            break;
    case GL_STENCIL_WRITEMASK:                *params = mState.stencilWritemask;              break;
    case GL_STENCIL_BACK_WRITEMASK:           *params = mState.stencilBackWritemask;          break;
    case GL_STENCIL_CLEAR_VALUE:              *params = mState.stencilClearValue;             break;
    case GL_SUBPIXEL_BITS:                    *params = 4;                                    break;
	case GL_MAX_TEXTURE_SIZE:                 *params = IMPLEMENTATION_MAX_TEXTURE_SIZE;  break;
	case GL_MAX_CUBE_MAP_TEXTURE_SIZE:        *params = IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE; break;
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:   
        {
            if(S3TC_SUPPORT)
            {
                // GL_COMPRESSED_RGB_S3TC_DXT1_EXT
                // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
				// GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
				// GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
                *params = 4;
            }
            else
            {
                *params = 0;
            }
        }
        break;
	case GL_MAX_SAMPLES_ANGLE:                *params = IMPLEMENTATION_MAX_SAMPLES; break;
    case GL_SAMPLE_BUFFERS:                   
    case GL_SAMPLES:
        {
            Framebuffer *framebuffer = getDrawFramebuffer();
			int width, height, samples;

            if(framebuffer->completeness(width, height, samples) == GL_FRAMEBUFFER_COMPLETE)
            {
                switch(pname)
                {
                case GL_SAMPLE_BUFFERS:
                    if(samples > 1)
                    {
                        *params = 1;
                    }
                    else
                    {
                        *params = 0;
                    }
                    break;
                case GL_SAMPLES:
                    *params = samples & ~1;
                    break;
                }
            }
            else
            {
                *params = 0;
            }
        }
        break;
    case GL_IMPLEMENTATION_COLOR_READ_TYPE:   *params = IMPLEMENTATION_COLOR_READ_TYPE;   break;
    case GL_IMPLEMENTATION_COLOR_READ_FORMAT: *params = IMPLEMENTATION_COLOR_READ_FORMAT; break;
    case GL_MAX_VIEWPORT_DIMS:
        {
			int maxDimension = IMPLEMENTATION_MAX_RENDERBUFFER_SIZE;
            params[0] = maxDimension;
            params[1] = maxDimension;
        }
        break;
    case GL_COMPRESSED_TEXTURE_FORMATS:
        {
            if(S3TC_SUPPORT)
            {
                params[0] = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                params[1] = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				params[2] = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE;
                params[3] = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE;
            }
        }
        break;
    case GL_VIEWPORT:
        params[0] = mState.viewportX;
        params[1] = mState.viewportY;
        params[2] = mState.viewportWidth;
        params[3] = mState.viewportHeight;
        break;
    case GL_SCISSOR_BOX:
        params[0] = mState.scissorX;
        params[1] = mState.scissorY;
        params[2] = mState.scissorWidth;
        params[3] = mState.scissorHeight;
        break;
    case GL_CULL_FACE_MODE:                   *params = mState.cullMode;                 break;
    case GL_FRONT_FACE:                       *params = mState.frontFace;                break;
    case GL_RED_BITS:
    case GL_GREEN_BITS:
    case GL_BLUE_BITS:
    case GL_ALPHA_BITS:
        {
            Framebuffer *framebuffer = getDrawFramebuffer();
            Renderbuffer *colorbuffer = framebuffer->getColorbuffer();

            if(colorbuffer)
            {
                switch (pname)
                {
                  case GL_RED_BITS:   *params = colorbuffer->getRedSize();   break;
                  case GL_GREEN_BITS: *params = colorbuffer->getGreenSize(); break;
                  case GL_BLUE_BITS:  *params = colorbuffer->getBlueSize();  break;
                  case GL_ALPHA_BITS: *params = colorbuffer->getAlphaSize(); break;
                }
            }
            else
            {
                *params = 0;
            }
        }
        break;
    case GL_DEPTH_BITS:
        {
            Framebuffer *framebuffer = getDrawFramebuffer();
            Renderbuffer *depthbuffer = framebuffer->getDepthbuffer();

            if(depthbuffer)
            {
                *params = depthbuffer->getDepthSize();
            }
            else
            {
                *params = 0;
            }
        }
        break;
    case GL_STENCIL_BITS:
        {
            Framebuffer *framebuffer = getDrawFramebuffer();
            Renderbuffer *stencilbuffer = framebuffer->getStencilbuffer();

            if(stencilbuffer)
            {
                *params = stencilbuffer->getStencilSize();
            }
            else
            {
                *params = 0;
            }
        }
        break;
    case GL_TEXTURE_BINDING_2D:
        {
            if(mState.activeSampler < 0 || mState.activeSampler > MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)
            {
                error(GL_INVALID_OPERATION);
                return false;
            }

            *params = mState.samplerTexture[TEXTURE_2D][mState.activeSampler].id();
        }
        break;
    case GL_TEXTURE_BINDING_CUBE_MAP:
        {
            if(mState.activeSampler < 0 || mState.activeSampler > MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)
            {
                error(GL_INVALID_OPERATION);
                return false;
            }

            *params = mState.samplerTexture[TEXTURE_CUBE][mState.activeSampler].id();
        }
        break;
    case GL_TEXTURE_BINDING_EXTERNAL_OES:
        {
            if(mState.activeSampler < 0 || mState.activeSampler > MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)
            {
                error(GL_INVALID_OPERATION);
                return false;
            }

            *params = mState.samplerTexture[TEXTURE_EXTERNAL][mState.activeSampler].id();
        }
        break;
    default:
        return false;
    }

    return true;
}

bool Context::getQueryParameterInfo(GLenum pname, GLenum *type, unsigned int *numParams)
{
    // Please note: the query type returned for DEPTH_CLEAR_VALUE in this implementation
    // is FLOAT rather than INT, as would be suggested by the GL ES 2.0 spec. This is due
    // to the fact that it is stored internally as a float, and so would require conversion
    // if returned from Context::getIntegerv. Since this conversion is already implemented 
    // in the case that one calls glGetIntegerv to retrieve a float-typed state variable, we
    // place DEPTH_CLEAR_VALUE with the floats. This should make no difference to the calling
    // application.
    switch (pname)
    {
      case GL_COMPRESSED_TEXTURE_FORMATS:
		{
            *type = GL_INT;
            *numParams = S3TC_SUPPORT ? 4 : 0;
        }
		break;
      case GL_SHADER_BINARY_FORMATS:
        {
            *type = GL_INT;
            *numParams = 0;
        }
        break;
      case GL_MAX_VERTEX_ATTRIBS:
      case GL_MAX_VERTEX_UNIFORM_VECTORS:
      case GL_MAX_VARYING_VECTORS:
      case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
      case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
      case GL_MAX_TEXTURE_IMAGE_UNITS:
      case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
      case GL_MAX_RENDERBUFFER_SIZE:
      case GL_NUM_SHADER_BINARY_FORMATS:
      case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
      case GL_ARRAY_BUFFER_BINDING:
      case GL_FRAMEBUFFER_BINDING:
      case GL_RENDERBUFFER_BINDING:
      case GL_CURRENT_PROGRAM:
      case GL_PACK_ALIGNMENT:
      case GL_UNPACK_ALIGNMENT:
      case GL_GENERATE_MIPMAP_HINT:
      case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES:
      case GL_RED_BITS:
      case GL_GREEN_BITS:
      case GL_BLUE_BITS:
      case GL_ALPHA_BITS:
      case GL_DEPTH_BITS:
      case GL_STENCIL_BITS:
      case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      case GL_CULL_FACE_MODE:
      case GL_FRONT_FACE:
      case GL_ACTIVE_TEXTURE:
      case GL_STENCIL_FUNC:
      case GL_STENCIL_VALUE_MASK:
      case GL_STENCIL_REF:
      case GL_STENCIL_FAIL:
      case GL_STENCIL_PASS_DEPTH_FAIL:
      case GL_STENCIL_PASS_DEPTH_PASS:
      case GL_STENCIL_BACK_FUNC:
      case GL_STENCIL_BACK_VALUE_MASK:
      case GL_STENCIL_BACK_REF:
      case GL_STENCIL_BACK_FAIL:
      case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
      case GL_STENCIL_BACK_PASS_DEPTH_PASS:
      case GL_DEPTH_FUNC:
      case GL_BLEND_SRC_RGB:
      case GL_BLEND_SRC_ALPHA:
      case GL_BLEND_DST_RGB:
      case GL_BLEND_DST_ALPHA:
      case GL_BLEND_EQUATION_RGB:
      case GL_BLEND_EQUATION_ALPHA:
      case GL_STENCIL_WRITEMASK:
      case GL_STENCIL_BACK_WRITEMASK:
      case GL_STENCIL_CLEAR_VALUE:
      case GL_SUBPIXEL_BITS:
      case GL_MAX_TEXTURE_SIZE:
      case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
      case GL_SAMPLE_BUFFERS:
      case GL_SAMPLES:
      case GL_IMPLEMENTATION_COLOR_READ_TYPE:
      case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
      case GL_TEXTURE_BINDING_2D:
      case GL_TEXTURE_BINDING_CUBE_MAP:
      case GL_TEXTURE_BINDING_EXTERNAL_OES:
        {
            *type = GL_INT;
            *numParams = 1;
        }
        break;
      case GL_MAX_SAMPLES_ANGLE:
        {
            *type = GL_INT;
            *numParams = 1;
        }
        break;
      case GL_MAX_VIEWPORT_DIMS:
        {
            *type = GL_INT;
            *numParams = 2;
        }
        break;
      case GL_VIEWPORT:
      case GL_SCISSOR_BOX:
        {
            *type = GL_INT;
            *numParams = 4;
        }
        break;
      case GL_SHADER_COMPILER:
      case GL_SAMPLE_COVERAGE_INVERT:
      case GL_DEPTH_WRITEMASK:
      case GL_CULL_FACE:                // CULL_FACE through DITHER are natural to IsEnabled,
      case GL_POLYGON_OFFSET_FILL:      // but can be retrieved through the Get{Type}v queries.
      case GL_SAMPLE_ALPHA_TO_COVERAGE: // For this purpose, they are treated here as bool-natural
      case GL_SAMPLE_COVERAGE:
      case GL_SCISSOR_TEST:
      case GL_STENCIL_TEST:
      case GL_DEPTH_TEST:
      case GL_BLEND:
      case GL_DITHER:
        {
            *type = GL_BOOL;
            *numParams = 1;
        }
        break;
      case GL_COLOR_WRITEMASK:
        {
            *type = GL_BOOL;
            *numParams = 4;
        }
        break;
      case GL_POLYGON_OFFSET_FACTOR:
      case GL_POLYGON_OFFSET_UNITS:
      case GL_SAMPLE_COVERAGE_VALUE:
      case GL_DEPTH_CLEAR_VALUE:
      case GL_LINE_WIDTH:
        {
            *type = GL_FLOAT;
            *numParams = 1;
        }
        break;
      case GL_ALIASED_LINE_WIDTH_RANGE:
      case GL_ALIASED_POINT_SIZE_RANGE:
      case GL_DEPTH_RANGE:
        {
            *type = GL_FLOAT;
            *numParams = 2;
        }
        break;
      case GL_COLOR_CLEAR_VALUE:
      case GL_BLEND_COLOR:
        {
            *type = GL_FLOAT;
            *numParams = 4;
        }
        break;
	  case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
        *type = GL_FLOAT;
        *numParams = 1;
        break;
      default:
        return false;
    }

    return true;
}

// Applies the render target surface, depth stencil surface, viewport rectangle and scissor rectangle
bool Context::applyRenderTarget()
{
    Framebuffer *framebuffer = getDrawFramebuffer();
	int width, height, samples;

    if(!framebuffer || framebuffer->completeness(width, height, samples) != GL_FRAMEBUFFER_COMPLETE)
    {
        return error(GL_INVALID_FRAMEBUFFER_OPERATION, false);
    }

    egl::Image *renderTarget = framebuffer->getRenderTarget();
	device->setRenderTarget(renderTarget);
	if(renderTarget) renderTarget->release();

    egl::Image *depthStencil = framebuffer->getDepthStencil();
    device->setDepthStencilSurface(depthStencil);
	if(depthStencil) depthStencil->release();

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
		scissor.clip(0, 0, width, height);
        
		device->setScissorRect(scissor);
        device->setScissorEnable(true);
    }
    else
    {
        device->setScissorEnable(false);
    }

	Program *program = getCurrentProgram();

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
    Framebuffer *framebuffer = getDrawFramebuffer();

    if(mState.cullFace)
    {
        device->setCullMode(es2sw::ConvertCullMode(mState.cullMode, mState.frontFace));
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
			device->setDepthCompare(es2sw::ConvertDepthComparison(mState.depthFunc));
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

            device->setBlendConstant(es2sw::ConvertColor(mState.blendColor));

			device->setSourceBlendFactor(es2sw::ConvertBlendFunc(mState.sourceBlendRGB));
			device->setDestBlendFactor(es2sw::ConvertBlendFunc(mState.destBlendRGB));
			device->setBlendOperation(es2sw::ConvertBlendOp(mState.blendEquationRGB));

            device->setSourceBlendFactorAlpha(es2sw::ConvertBlendFunc(mState.sourceBlendAlpha));
			device->setDestBlendFactorAlpha(es2sw::ConvertBlendFunc(mState.destBlendAlpha));
			device->setBlendOperationAlpha(es2sw::ConvertBlendOp(mState.blendEquationAlpha));
        }
        else
        {
			device->setAlphaBlendEnable(false);
        }

        mBlendStateDirty = false;
    }

    if(mStencilStateDirty || mFrontFaceDirty)
    {
        if(mState.stencilTest && framebuffer->hasStencil())
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
            Renderbuffer *stencilbuffer = framebuffer->getStencilbuffer();
            GLuint maxStencil = (1 << stencilbuffer->getStencilSize()) - 1;

			if(mState.frontFace == GL_CCW)
			{
				device->setStencilWriteMask(mState.stencilWritemask);
				device->setStencilCompare(es2sw::ConvertStencilComparison(mState.stencilFunc));

				device->setStencilReference((mState.stencilRef < (GLint)maxStencil) ? mState.stencilRef : maxStencil);
				device->setStencilMask(mState.stencilMask);

				device->setStencilFailOperation(es2sw::ConvertStencilOp(mState.stencilFail));
				device->setStencilZFailOperation(es2sw::ConvertStencilOp(mState.stencilPassDepthFail));
				device->setStencilPassOperation(es2sw::ConvertStencilOp(mState.stencilPassDepthPass));

				device->setStencilWriteMaskCCW(mState.stencilBackWritemask);
				device->setStencilCompareCCW(es2sw::ConvertStencilComparison(mState.stencilBackFunc));

				device->setStencilReferenceCCW((mState.stencilBackRef < (GLint)maxStencil) ? mState.stencilBackRef : maxStencil);
				device->setStencilMaskCCW(mState.stencilBackMask);

				device->setStencilFailOperationCCW(es2sw::ConvertStencilOp(mState.stencilBackFail));
				device->setStencilZFailOperationCCW(es2sw::ConvertStencilOp(mState.stencilBackPassDepthFail));
				device->setStencilPassOperationCCW(es2sw::ConvertStencilOp(mState.stencilBackPassDepthPass));
			}
			else
			{
				device->setStencilWriteMaskCCW(mState.stencilWritemask);
				device->setStencilCompareCCW(es2sw::ConvertStencilComparison(mState.stencilFunc));

				device->setStencilReferenceCCW((mState.stencilRef < (GLint)maxStencil) ? mState.stencilRef : maxStencil);
				device->setStencilMaskCCW(mState.stencilMask);

				device->setStencilFailOperationCCW(es2sw::ConvertStencilOp(mState.stencilFail));
				device->setStencilZFailOperationCCW(es2sw::ConvertStencilOp(mState.stencilPassDepthFail));
				device->setStencilPassOperationCCW(es2sw::ConvertStencilOp(mState.stencilPassDepthPass));

				device->setStencilWriteMask(mState.stencilBackWritemask);
				device->setStencilCompare(es2sw::ConvertStencilComparison(mState.stencilBackFunc));

				device->setStencilReference((mState.stencilBackRef < (GLint)maxStencil) ? mState.stencilBackRef : maxStencil);
				device->setStencilMask(mState.stencilBackMask);

				device->setStencilFailOperation(es2sw::ConvertStencilOp(mState.stencilBackFail));
				device->setStencilZFailOperation(es2sw::ConvertStencilOp(mState.stencilBackPassDepthFail));
				device->setStencilPassOperation(es2sw::ConvertStencilOp(mState.stencilBackPassDepthPass));
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
		device->setColorWriteMask(0, es2sw::ConvertColorMask(mState.colorMaskRed, mState.colorMaskGreen, mState.colorMaskBlue, mState.colorMaskAlpha));
		device->setDepthWriteEnable(mState.depthMask);

        mMaskStateDirty = false;
    }

    if(mPolygonOffsetStateDirty)
    {
        if(mState.polygonOffsetFill)
        {
            Renderbuffer *depthbuffer = framebuffer->getDepthbuffer();
            if(depthbuffer)
            {
				device->setSlopeDepthBias(mState.polygonOffsetFactor);
                float depthBias = ldexp(mState.polygonOffsetUnits, -(int)(depthbuffer->getDepthSize()));
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
				int width, height, samples;
				framebuffer->completeness(width, height, samples);

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

GLenum Context::applyVertexBuffer(GLint base, GLint first, GLsizei count)
{
    TranslatedAttribute attributes[MAX_VERTEX_ATTRIBS];

    GLenum err = mVertexDataManager->prepareVertexData(first, count, attributes);
    if(err != GL_NO_ERROR)
    {
        return err;
    }

	Program *program = getCurrentProgram();

	device->resetInputStreams(false);

    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
	{
		if(program->getAttributeStream(i) == -1)
		{
			continue;
		}

		sw::Resource *resource = attributes[i].vertexBuffer;
		const void *buffer = (char*)resource->getBuffer() + attributes[i].offset;
			
		int stride = attributes[i].stride;

		buffer = (char*)buffer + stride * base;

		sw::Stream attribute(resource, buffer, stride);

		attribute.type = attributes[i].type;
		attribute.count = attributes[i].count;
		attribute.normalized = attributes[i].normalized;

		int stream = program->getAttributeStream(i);
		device->setInputStream(stream, attribute);
	}

	return GL_NO_ERROR;
}

// Applies the indices and element array bindings
GLenum Context::applyIndexBuffer(const void *indices, GLsizei count, GLenum mode, GLenum type, TranslatedIndexData *indexInfo)
{
    GLenum err = mIndexDataManager->prepareIndexData(type, count, mState.elementArrayBuffer.get(), indices, indexInfo);

    if(err == GL_NO_ERROR)
    {
        device->setIndexBuffer(indexInfo->indexBuffer);
    }

    return err;
}

// Applies the shaders and shader constants
void Context::applyShaders()
{
    Program *programObject = getCurrentProgram();
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

void Context::applyTextures()
{
    applyTextures(sw::SAMPLER_PIXEL);
	applyTextures(sw::SAMPLER_VERTEX);
}

void Context::applyTextures(sw::SamplerType samplerType)
{
    Program *programObject = getCurrentProgram();

    int samplerCount = (samplerType == sw::SAMPLER_PIXEL) ? MAX_TEXTURE_IMAGE_UNITS : MAX_VERTEX_TEXTURE_IMAGE_UNITS;   // Range of samplers of given sampler type

    for(int samplerIndex = 0; samplerIndex < samplerCount; samplerIndex++)
    {
        int textureUnit = programObject->getSamplerMapping(samplerType, samplerIndex);   // OpenGL texture image unit index

        if(textureUnit != -1)
        {
            TextureType textureType = programObject->getSamplerTextureType(samplerType, samplerIndex);

            Texture *texture = getSamplerTexture(textureUnit, textureType);

			if(texture->isSamplerComplete())
            {
                GLenum wrapS = texture->getWrapS();
                GLenum wrapT = texture->getWrapT();
                GLenum texFilter = texture->getMinFilter();
                GLenum magFilter = texture->getMagFilter();
				GLfloat maxAnisotropy = texture->getMaxAnisotropy();

				device->setAddressingModeU(samplerType, samplerIndex, es2sw::ConvertTextureWrap(wrapS));
                device->setAddressingModeV(samplerType, samplerIndex, es2sw::ConvertTextureWrap(wrapT));

				sw::FilterType minFilter;
				sw::MipmapType mipFilter;
                es2sw::ConvertMinFilter(texFilter, &minFilter, &mipFilter, maxAnisotropy);
			//	ASSERT(minFilter == es2sw::ConvertMagFilter(magFilter));

				device->setTextureFilter(samplerType, samplerIndex, minFilter);
			//	device->setTextureFilter(samplerType, samplerIndex, es2sw::ConvertMagFilter(magFilter));
				device->setMipmapFilter(samplerType, samplerIndex, mipFilter);
				device->setMaxAnisotropy(samplerType, samplerIndex, maxAnisotropy);                

				applyTexture(samplerType, samplerIndex, texture);
            }
            else
            {
                applyTexture(samplerType, samplerIndex, 0);
            }
        }
        else
        {
            applyTexture(samplerType, samplerIndex, NULL);
        }
    }
}

void Context::applyTexture(sw::SamplerType type, int index, Texture *baseTexture)
{
	Program *program = getCurrentProgram();
	int sampler = (type == sw::SAMPLER_PIXEL) ? index : 16 + index;
	bool textureUsed = false;

	if(type == sw::SAMPLER_PIXEL)
	{
		textureUsed = program->getPixelShader()->usesSampler(index);
	}
	else if(type == sw::SAMPLER_VERTEX)
	{
		textureUsed = program->getVertexShader()->usesSampler(index);
	}
	else UNREACHABLE();

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

void Context::readPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, GLsizei *bufSize, void* pixels)
{
    Framebuffer *framebuffer = getReadFramebuffer();
	int framebufferWidth, framebufferHeight, framebufferSamples;

    if(framebuffer->completeness(framebufferWidth, framebufferHeight, framebufferSamples) != GL_FRAMEBUFFER_COMPLETE)
    {
        return error(GL_INVALID_FRAMEBUFFER_OPERATION);
    }

    if(getReadFramebufferHandle() != 0 && framebufferSamples != 0)
    {
        return error(GL_INVALID_OPERATION);
    }

	GLsizei outputPitch = ComputePitch(width, format, type, mState.packAlignment);
    
	// Sized query sanity check
    if(bufSize)
    {
        int requiredSize = outputPitch * height;
        if(requiredSize > *bufSize)
        {
            return error(GL_INVALID_OPERATION);
        }
    }

    egl::Image *renderTarget = framebuffer->getRenderTarget();

    if(!renderTarget)
    {
        return error(GL_OUT_OF_MEMORY);
    }

	sw::Rect rect = {x, y, x + width, y + height};
	rect.clip(0, 0, renderTarget->getWidth(), renderTarget->getHeight());

    unsigned char *source = (unsigned char*)renderTarget->lock(rect.x0, rect.y0, sw::LOCK_READONLY);
    unsigned char *dest = (unsigned char*)pixels;
    unsigned short *dest16 = (unsigned short*)pixels;
    int inputPitch = (int)renderTarget->getPitch();

    for(int j = 0; j < rect.y1 - rect.y0; j++)
    {
        if(renderTarget->getInternalFormat() == sw::FORMAT_A8R8G8B8 &&
           format == GL_BGRA_EXT && type == GL_UNSIGNED_BYTE)
        {
            // Fast path for EXT_read_format_bgra, given an RGBA source buffer
			// Note that buffers with no alpha go through the slow path below
            memcpy(dest + j * outputPitch, source + j * inputPitch, (rect.x1 - rect.x0) * 4);
        }
		else
		{
			for(int i = 0; i < rect.x1 - rect.x0; i++)
			{
				float r;
				float g;
				float b;
				float a;

				switch(renderTarget->getInternalFormat())
				{
				case sw::FORMAT_R5G6B5:
					{
						unsigned short rgb = *(unsigned short*)(source + 2 * i + j * inputPitch);

						a = 1.0f;
						b = (rgb & 0x001F) * (1.0f / 0x001F);
						g = (rgb & 0x07E0) * (1.0f / 0x07E0);
						r = (rgb & 0xF800) * (1.0f / 0xF800);
					}
					break;
				case sw::FORMAT_A1R5G5B5:
					{
						unsigned short argb = *(unsigned short*)(source + 2 * i + j * inputPitch);

						a = (argb & 0x8000) ? 1.0f : 0.0f;
						b = (argb & 0x001F) * (1.0f / 0x001F);
						g = (argb & 0x03E0) * (1.0f / 0x03E0);
						r = (argb & 0x7C00) * (1.0f / 0x7C00);
					}
					break;
				case sw::FORMAT_A8R8G8B8:
					{
						unsigned int argb = *(unsigned int*)(source + 4 * i + j * inputPitch);

						a = (argb & 0xFF000000) * (1.0f / 0xFF000000);
						b = (argb & 0x000000FF) * (1.0f / 0x000000FF);
						g = (argb & 0x0000FF00) * (1.0f / 0x0000FF00);
						r = (argb & 0x00FF0000) * (1.0f / 0x00FF0000);
					}
					break;
				case sw::FORMAT_X8R8G8B8:
					{
						unsigned int xrgb = *(unsigned int*)(source + 4 * i + j * inputPitch);

						a = 1.0f;
						b = (xrgb & 0x000000FF) * (1.0f / 0x000000FF);
						g = (xrgb & 0x0000FF00) * (1.0f / 0x0000FF00);
						r = (xrgb & 0x00FF0000) * (1.0f / 0x00FF0000);
					}
					break;
				case sw::FORMAT_A2R10G10B10:
					{
						unsigned int argb = *(unsigned int*)(source + 4 * i + j * inputPitch);

						a = (argb & 0xC0000000) * (1.0f / 0xC0000000);
						b = (argb & 0x000003FF) * (1.0f / 0x000003FF);
						g = (argb & 0x000FFC00) * (1.0f / 0x000FFC00);
						r = (argb & 0x3FF00000) * (1.0f / 0x3FF00000);
					}
					break;
				case sw::FORMAT_A32B32G32R32F:
					{
						r = *((float*)(source + 16 * i + j * inputPitch) + 0);
						g = *((float*)(source + 16 * i + j * inputPitch) + 1);
						b = *((float*)(source + 16 * i + j * inputPitch) + 2);
						a = *((float*)(source + 16 * i + j * inputPitch) + 3);
					}
					break;
				case sw::FORMAT_A16B16G16R16F:
					{
						r = (float)*((sw::half*)(source + 8 * i + j * inputPitch) + 0);
						g = (float)*((sw::half*)(source + 8 * i + j * inputPitch) + 1);
						b = (float)*((sw::half*)(source + 8 * i + j * inputPitch) + 2);
						a = (float)*((sw::half*)(source + 8 * i + j * inputPitch) + 3);
					}
					break;
				default:
					UNIMPLEMENTED();   // FIXME
					UNREACHABLE();
				}

				switch(format)
				{
				case GL_RGBA:
					switch(type)
					{
					case GL_UNSIGNED_BYTE:
						dest[4 * i + j * outputPitch + 0] = (unsigned char)(255 * r + 0.5f);
						dest[4 * i + j * outputPitch + 1] = (unsigned char)(255 * g + 0.5f);
						dest[4 * i + j * outputPitch + 2] = (unsigned char)(255 * b + 0.5f);
						dest[4 * i + j * outputPitch + 3] = (unsigned char)(255 * a + 0.5f);
						break;
					default: UNREACHABLE();
					}
					break;
				case GL_BGRA_EXT:
					switch(type)
					{
					case GL_UNSIGNED_BYTE:
						dest[4 * i + j * outputPitch + 0] = (unsigned char)(255 * b + 0.5f);
						dest[4 * i + j * outputPitch + 1] = (unsigned char)(255 * g + 0.5f);
						dest[4 * i + j * outputPitch + 2] = (unsigned char)(255 * r + 0.5f);
						dest[4 * i + j * outputPitch + 3] = (unsigned char)(255 * a + 0.5f);
						break;
					case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
						// According to the desktop GL spec in the "Transfer of Pixel Rectangles" section
						// this type is packed as follows:
						//   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
						//  --------------------------------------------------------------------------------
						// |       4th         |        3rd         |        2nd        |   1st component   |
						//  --------------------------------------------------------------------------------
						// in the case of BGRA_EXT, B is the first component, G the second, and so forth.
						dest16[i + j * outputPitch / sizeof(unsigned short)] =
							((unsigned short)(15 * a + 0.5f) << 12)|
							((unsigned short)(15 * r + 0.5f) << 8) |
							((unsigned short)(15 * g + 0.5f) << 4) |
							((unsigned short)(15 * b + 0.5f) << 0);
						break;
					case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
						// According to the desktop GL spec in the "Transfer of Pixel Rectangles" section
						// this type is packed as follows:
						//   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
						//  --------------------------------------------------------------------------------
						// | 4th |          3rd           |           2nd          |      1st component     |
						//  --------------------------------------------------------------------------------
						// in the case of BGRA_EXT, B is the first component, G the second, and so forth.
						dest16[i + j * outputPitch / sizeof(unsigned short)] =
							((unsigned short)(     a + 0.5f) << 15) |
							((unsigned short)(31 * r + 0.5f) << 10) |
							((unsigned short)(31 * g + 0.5f) << 5) |
							((unsigned short)(31 * b + 0.5f) << 0);
						break;
					default: UNREACHABLE();
					}
					break;
				case GL_RGB:   // IMPLEMENTATION_COLOR_READ_FORMAT
					switch(type)
					{
					case GL_UNSIGNED_SHORT_5_6_5:   // IMPLEMENTATION_COLOR_READ_TYPE
						dest16[i + j * outputPitch / sizeof(unsigned short)] = 
							((unsigned short)(31 * b + 0.5f) << 0) |
							((unsigned short)(63 * g + 0.5f) << 5) |
							((unsigned short)(31 * r + 0.5f) << 11);
						break;
					default: UNREACHABLE();
					}
					break;
				default: UNREACHABLE();
				}
			}
        }
    }

	renderTarget->unlock();
	renderTarget->release();
}

void Context::clear(GLbitfield mask)
{
    Framebuffer *framebuffer = getDrawFramebuffer();

    if(!framebuffer || framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE)
    {
        return error(GL_INVALID_FRAMEBUFFER_OPERATION);
    }

    if(!applyRenderTarget())
    {
        return;
    }
	
	unsigned int color = (unorm<8>(mState.colorClearValue.alpha) << 24) |
                         (unorm<8>(mState.colorClearValue.red) << 16) |
                         (unorm<8>(mState.colorClearValue.green) << 8) | 
                         (unorm<8>(mState.colorClearValue.blue) << 0);
    float depth = clamp01(mState.depthClearValue);
    int stencil = mState.stencilClearValue & 0x000000FF;

	if(mask & GL_COLOR_BUFFER_BIT)
	{
		unsigned int rgbaMask = (mState.colorMaskRed ? 0x1 : 0) |
		                        (mState.colorMaskGreen ? 0x2 : 0) | 
		                        (mState.colorMaskBlue ? 0x4 : 0) |
		                        (mState.colorMaskAlpha ? 0x8 : 0);

		if(rgbaMask != 0)
		{
			device->clearColor(color, rgbaMask);
		}
	}

	if(mask & GL_DEPTH_BUFFER_BIT)
	{
		if(mState.depthMask != 0)
		{
			device->clearDepth(depth);
		}
	}

	if(mask & GL_STENCIL_BUFFER_BIT)
	{
		if(mState.stencilWritemask != 0)
		{
			device->clearStencil(stencil, mState.stencilWritemask);
		}
	}
}

void Context::drawArrays(GLenum mode, GLint first, GLsizei count)
{
    if(!mState.currentProgram)
    {
        return error(GL_INVALID_OPERATION);
    }

    PrimitiveType primitiveType;
    int primitiveCount;

    if(!es2sw::ConvertPrimitiveType(mode, count, primitiveType, primitiveCount))
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

    GLenum err = applyVertexBuffer(0, first, count);
    if(err != GL_NO_ERROR)
    {
        return error(err);
    }

    applyShaders();
    applyTextures();

    if(!getCurrentProgram()->validateSamplers(false))
    {
        return error(GL_INVALID_OPERATION);
    }

    if(!cullSkipsDraw(mode))
    {
        device->drawPrimitive(primitiveType, primitiveCount);
    }
}

void Context::drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    if(!mState.currentProgram)
    {
        return error(GL_INVALID_OPERATION);
    }

    if(!indices && !mState.elementArrayBuffer)
    {
        return error(GL_INVALID_OPERATION);
    }

    PrimitiveType primitiveType;
    int primitiveCount;

    if(!es2sw::ConvertPrimitiveType(mode, count, primitiveType, primitiveCount))
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

    TranslatedIndexData indexInfo;
    GLenum err = applyIndexBuffer(indices, count, mode, type, &indexInfo);
    if(err != GL_NO_ERROR)
    {
        return error(err);
    }

    GLsizei vertexCount = indexInfo.maxIndex - indexInfo.minIndex + 1;
    err = applyVertexBuffer(-(int)indexInfo.minIndex, indexInfo.minIndex, vertexCount);
    if(err != GL_NO_ERROR)
    {
        return error(err);
    }

    applyShaders();
    applyTextures();

    if(!getCurrentProgram()->validateSamplers(false))
    {
        return error(GL_INVALID_OPERATION);
    }

    if(!cullSkipsDraw(mode))
    {
		device->drawIndexedPrimitive(primitiveType, indexInfo.indexOffset, primitiveCount, IndexDataManager::typeSize(type));
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

void Context::detachBuffer(GLuint buffer)
{
    // [OpenGL ES 2.0.24] section 2.9 page 22:
    // If a buffer object is deleted while it is bound, all bindings to that object in the current context
    // (i.e. in the thread that called Delete-Buffers) are reset to zero.

    if(mState.arrayBuffer.id() == buffer)
    {
        mState.arrayBuffer.set(NULL);
    }

    if(mState.elementArrayBuffer.id() == buffer)
    {
        mState.elementArrayBuffer.set(NULL);
    }

    for(int attribute = 0; attribute < MAX_VERTEX_ATTRIBS; attribute++)
    {
        if(mState.vertexAttribute[attribute].mBoundBuffer.id() == buffer)
        {
            mState.vertexAttribute[attribute].mBoundBuffer.set(NULL);
        }
    }
}

void Context::detachTexture(GLuint texture)
{
    // [OpenGL ES 2.0.24] section 3.8 page 84:
    // If a texture object is deleted, it is as if all texture units which are bound to that texture object are
    // rebound to texture object zero

    for(int type = 0; type < TEXTURE_TYPE_COUNT; type++)
    {
        for(int sampler = 0; sampler < MAX_COMBINED_TEXTURE_IMAGE_UNITS; sampler++)
        {
            if(mState.samplerTexture[type][sampler].id() == texture)
            {
                mState.samplerTexture[type][sampler].set(NULL);
            }
        }
    }

    // [OpenGL ES 2.0.24] section 4.4 page 112:
    // If a texture object is deleted while its image is attached to the currently bound framebuffer, then it is
    // as if FramebufferTexture2D had been called, with a texture of 0, for each attachment point to which this
    // image was attached in the currently bound framebuffer.

    Framebuffer *readFramebuffer = getReadFramebuffer();
    Framebuffer *drawFramebuffer = getDrawFramebuffer();

    if(readFramebuffer)
    {
        readFramebuffer->detachTexture(texture);
    }

    if(drawFramebuffer && drawFramebuffer != readFramebuffer)
    {
        drawFramebuffer->detachTexture(texture);
    }
}

void Context::detachFramebuffer(GLuint framebuffer)
{
    // [OpenGL ES 2.0.24] section 4.4 page 107:
    // If a framebuffer that is currently bound to the target FRAMEBUFFER is deleted, it is as though
    // BindFramebuffer had been executed with the target of FRAMEBUFFER and framebuffer of zero.

    if(mState.readFramebuffer == framebuffer)
    {
        bindReadFramebuffer(0);
    }

    if(mState.drawFramebuffer == framebuffer)
    {
        bindDrawFramebuffer(0);
    }
}

void Context::detachRenderbuffer(GLuint renderbuffer)
{
    // [OpenGL ES 2.0.24] section 4.4 page 109:
    // If a renderbuffer that is currently bound to RENDERBUFFER is deleted, it is as though BindRenderbuffer
    // had been executed with the target RENDERBUFFER and name of zero.

    if(mState.renderbuffer.id() == renderbuffer)
    {
        bindRenderbuffer(0);
    }

    // [OpenGL ES 2.0.24] section 4.4 page 111:
    // If a renderbuffer object is deleted while its image is attached to the currently bound framebuffer,
    // then it is as if FramebufferRenderbuffer had been called, with a renderbuffer of 0, for each attachment
    // point to which this image was attached in the currently bound framebuffer.

    Framebuffer *readFramebuffer = getReadFramebuffer();
    Framebuffer *drawFramebuffer = getDrawFramebuffer();

    if(readFramebuffer)
    {
        readFramebuffer->detachRenderbuffer(renderbuffer);
    }

    if(drawFramebuffer && drawFramebuffer != readFramebuffer)
    {
        drawFramebuffer->detachRenderbuffer(renderbuffer);
    }
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

void Context::setVertexAttrib(GLuint index, const GLfloat *values)
{
    ASSERT(index < MAX_VERTEX_ATTRIBS);

    mState.vertexAttribute[index].mCurrentValue[0] = values[0];
    mState.vertexAttribute[index].mCurrentValue[1] = values[1];
    mState.vertexAttribute[index].mCurrentValue[2] = values[2];
    mState.vertexAttribute[index].mCurrentValue[3] = values[3];

    mVertexDataManager->dirtyCurrentValue(index);
}

void Context::blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, 
                              GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                              GLbitfield mask)
{
    Framebuffer *readFramebuffer = getReadFramebuffer();
    Framebuffer *drawFramebuffer = getDrawFramebuffer();

	int readBufferWidth, readBufferHeight, readBufferSamples;
    int drawBufferWidth, drawBufferHeight, drawBufferSamples;

    if(!readFramebuffer || readFramebuffer->completeness(readBufferWidth, readBufferHeight, readBufferSamples) != GL_FRAMEBUFFER_COMPLETE ||
       !drawFramebuffer || drawFramebuffer->completeness(drawBufferWidth, drawBufferHeight, drawBufferSamples) != GL_FRAMEBUFFER_COMPLETE)
    {
        return error(GL_INVALID_FRAMEBUFFER_OPERATION);
    }

    if(drawBufferSamples > 1)
    {
        return error(GL_INVALID_OPERATION);
    }

    sw::Rect sourceRect;
    sw::Rect destRect;

    if(srcX0 < srcX1)
    {
        sourceRect.x0 = srcX0;
        sourceRect.x1 = srcX1;
        destRect.x0 = dstX0;
        destRect.x1 = dstX1;
    }
    else
    {
        sourceRect.x0 = srcX1;
        destRect.x0 = dstX1;
        sourceRect.x1 = srcX0;
        destRect.x1 = dstX0;
    }

    if(srcY0 < srcY1)
    {
        sourceRect.y0 = srcY0;
        destRect.y0 = dstY0;
        sourceRect.y1 = srcY1;
        destRect.y1 = dstY1;
    }
    else
    {
        sourceRect.y0 = srcY1;
        destRect.y0 = dstY1;
        sourceRect.y1 = srcY0;
        destRect.y1 = dstY0;
    }

    sw::Rect sourceScissoredRect = sourceRect;
    sw::Rect destScissoredRect = destRect;

    if(mState.scissorTest)   // Only write to parts of the destination framebuffer which pass the scissor test
    {
        if(destRect.x0 < mState.scissorX)
        {
            int xDiff = mState.scissorX - destRect.x0;
            destScissoredRect.x0 = mState.scissorX;
            sourceScissoredRect.x0 += xDiff;
        }

        if(destRect.x1 > mState.scissorX + mState.scissorWidth)
        {
            int xDiff = destRect.x1 - (mState.scissorX + mState.scissorWidth);
            destScissoredRect.x1 = mState.scissorX + mState.scissorWidth;
            sourceScissoredRect.x1 -= xDiff;
        }

        if(destRect.y0 < mState.scissorY)
        {
            int yDiff = mState.scissorY - destRect.y0;
            destScissoredRect.y0 = mState.scissorY;
            sourceScissoredRect.y0 += yDiff;
        }

        if(destRect.y1 > mState.scissorY + mState.scissorHeight)
        {
            int yDiff = destRect.y1 - (mState.scissorY + mState.scissorHeight);
            destScissoredRect.y1 = mState.scissorY + mState.scissorHeight;
            sourceScissoredRect.y1 -= yDiff;
        }
    }

    sw::Rect sourceTrimmedRect = sourceScissoredRect;
    sw::Rect destTrimmedRect = destScissoredRect;

    // The source & destination rectangles also may need to be trimmed if they fall out of the bounds of 
    // the actual draw and read surfaces.
    if(sourceTrimmedRect.x0 < 0)
    {
        int xDiff = 0 - sourceTrimmedRect.x0;
        sourceTrimmedRect.x0 = 0;
        destTrimmedRect.x0 += xDiff;
    }

    if(sourceTrimmedRect.x1 > readBufferWidth)
    {
        int xDiff = sourceTrimmedRect.x1 - readBufferWidth;
        sourceTrimmedRect.x1 = readBufferWidth;
        destTrimmedRect.x1 -= xDiff;
    }

    if(sourceTrimmedRect.y0 < 0)
    {
        int yDiff = 0 - sourceTrimmedRect.y0;
        sourceTrimmedRect.y0 = 0;
        destTrimmedRect.y0 += yDiff;
    }

    if(sourceTrimmedRect.y1 > readBufferHeight)
    {
        int yDiff = sourceTrimmedRect.y1 - readBufferHeight;
        sourceTrimmedRect.y1 = readBufferHeight;
        destTrimmedRect.y1 -= yDiff;
    }

    if(destTrimmedRect.x0 < 0)
    {
        int xDiff = 0 - destTrimmedRect.x0;
        destTrimmedRect.x0 = 0;
        sourceTrimmedRect.x0 += xDiff;
    }

    if(destTrimmedRect.x1 > drawBufferWidth)
    {
        int xDiff = destTrimmedRect.x1 - drawBufferWidth;
        destTrimmedRect.x1 = drawBufferWidth;
        sourceTrimmedRect.x1 -= xDiff;
    }

    if(destTrimmedRect.y0 < 0)
    {
        int yDiff = 0 - destTrimmedRect.y0;
        destTrimmedRect.y0 = 0;
        sourceTrimmedRect.y0 += yDiff;
    }

    if(destTrimmedRect.y1 > drawBufferHeight)
    {
        int yDiff = destTrimmedRect.y1 - drawBufferHeight;
        destTrimmedRect.y1 = drawBufferHeight;
        sourceTrimmedRect.y1 -= yDiff;
    }

    bool partialBufferCopy = false;

    if(sourceTrimmedRect.y1 - sourceTrimmedRect.y0 < readBufferHeight ||
       sourceTrimmedRect.x1 - sourceTrimmedRect.x0 < readBufferWidth || 
       destTrimmedRect.y1 - destTrimmedRect.y0 < drawBufferHeight ||
       destTrimmedRect.x1 - destTrimmedRect.x0 < drawBufferWidth ||
       sourceTrimmedRect.y0 != 0 || destTrimmedRect.y0 != 0 || sourceTrimmedRect.x0 != 0 || destTrimmedRect.x0 != 0)
    {
        partialBufferCopy = true;
    }

	bool blitRenderTarget = false;
    bool blitDepthStencil = false;

    if(mask & GL_COLOR_BUFFER_BIT)
    {
        const bool validReadType = readFramebuffer->getColorbufferType() == GL_TEXTURE_2D ||
                                   readFramebuffer->getColorbufferType() == GL_RENDERBUFFER;
        const bool validDrawType = drawFramebuffer->getColorbufferType() == GL_TEXTURE_2D ||
                                   drawFramebuffer->getColorbufferType() == GL_RENDERBUFFER;
        if(!validReadType || !validDrawType ||
           readFramebuffer->getColorbuffer()->getInternalFormat() != drawFramebuffer->getColorbuffer()->getInternalFormat())
        {
            ERR("Color buffer format conversion in BlitFramebufferANGLE not supported by this implementation");
            return error(GL_INVALID_OPERATION);
        }
        
        if(partialBufferCopy && readBufferSamples > 1)
        {
            return error(GL_INVALID_OPERATION);
        }

        blitRenderTarget = true;
    }

    if(mask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT))
    {
        Renderbuffer *readDSBuffer = NULL;
        Renderbuffer *drawDSBuffer = NULL;

        // We support OES_packed_depth_stencil, and do not support a separately attached depth and stencil buffer, so if we have
        // both a depth and stencil buffer, it will be the same buffer.

        if(mask & GL_DEPTH_BUFFER_BIT)
        {
            if(readFramebuffer->getDepthbuffer() && drawFramebuffer->getDepthbuffer())
            {
                if(readFramebuffer->getDepthbufferType() != drawFramebuffer->getDepthbufferType() ||
                   readFramebuffer->getDepthbuffer()->getInternalFormat() != drawFramebuffer->getDepthbuffer()->getInternalFormat())
                {
                    return error(GL_INVALID_OPERATION);
                }

                blitDepthStencil = true;
                readDSBuffer = readFramebuffer->getDepthbuffer();
                drawDSBuffer = drawFramebuffer->getDepthbuffer();
            }
        }

        if(mask & GL_STENCIL_BUFFER_BIT)
        {
            if(readFramebuffer->getStencilbuffer() && drawFramebuffer->getStencilbuffer())
            {
                if(readFramebuffer->getStencilbufferType() != drawFramebuffer->getStencilbufferType() ||
                   readFramebuffer->getStencilbuffer()->getInternalFormat() != drawFramebuffer->getStencilbuffer()->getInternalFormat())
                {
                    return error(GL_INVALID_OPERATION);
                }

                blitDepthStencil = true;
                readDSBuffer = readFramebuffer->getStencilbuffer();
                drawDSBuffer = drawFramebuffer->getStencilbuffer();
            }
        }

        if(partialBufferCopy)
        {
            ERR("Only whole-buffer depth and stencil blits are supported by this implementation.");
            return error(GL_INVALID_OPERATION);   // Only whole-buffer copies are permitted
        }

        if((drawDSBuffer && drawDSBuffer->getSamples() > 1) || 
           (readDSBuffer && readDSBuffer->getSamples() > 1))
        {
            return error(GL_INVALID_OPERATION);
        }
    }

    if(blitRenderTarget || blitDepthStencil)
    {
        if(blitRenderTarget)
        {
            egl::Image *readRenderTarget = readFramebuffer->getRenderTarget();
            egl::Image *drawRenderTarget = drawFramebuffer->getRenderTarget();
 
            bool success = device->stretchRect(readRenderTarget, &sourceRect, drawRenderTarget, &destRect, false);

            readRenderTarget->release();
            drawRenderTarget->release();

            if(!success)
            {
                ERR("BlitFramebufferANGLE failed.");
                return;
            }
        }

        if(blitDepthStencil)
        {
            bool success = device->stretchRect(readFramebuffer->getDepthStencil(), NULL, drawFramebuffer->getDepthStencil(), NULL, false);

            if(!success)
            {
                ERR("BlitFramebufferANGLE failed.");
                return;
            }
        }
    }
}

void Context::bindTexImage(egl::Surface *surface)
{
	es2::Texture2D *textureObject = getTexture2D();

    if(textureObject)
    {
		textureObject->bindTexImage(surface);
	}
}

EGLenum Context::validateSharedImage(EGLenum target, GLuint name, GLuint textureLevel)
{
    GLenum textureTarget = GL_NONE;

    switch(target)
    {
    case EGL_GL_TEXTURE_2D_KHR:
        textureTarget = GL_TEXTURE_2D;
        break;
    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
    case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
    case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
    case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
        textureTarget = GL_TEXTURE_CUBE_MAP;
        break;
    case EGL_GL_RENDERBUFFER_KHR:
        break;
    default:
        return EGL_BAD_PARAMETER;
    }
	
    if(textureLevel >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
    {
        return EGL_BAD_MATCH;
    }

    if(textureTarget != GL_NONE)
    {
        es2::Texture *texture = getTexture(name);

        if(!texture || texture->getTarget() != textureTarget)
        {
            return EGL_BAD_PARAMETER;
        }

        if(texture->isShared(textureTarget, textureLevel))   // Bound to an EGLSurface or already an EGLImage sibling
        {
            return EGL_BAD_ACCESS;
        }

        if(textureLevel != 0 && !texture->isSamplerComplete())
        {
            return EGL_BAD_PARAMETER;
        }

        if(textureLevel == 0 && !(texture->isSamplerComplete() && texture->getLevelCount() == 1))
        {
            return EGL_BAD_PARAMETER;
        }
    }
    else if(target == EGL_GL_RENDERBUFFER_KHR)
    {
        es2::Renderbuffer *renderbuffer = getRenderbuffer(name);

        if(!renderbuffer)
        {
            return EGL_BAD_PARAMETER;
        }

        if(renderbuffer->isShared())   // Already an EGLImage sibling
        {
            return EGL_BAD_ACCESS;
        }
    }
    else UNREACHABLE();

	return EGL_SUCCESS;
}

egl::Image *Context::createSharedImage(EGLenum target, GLuint name, GLuint textureLevel)
{
	GLenum textureTarget = GL_NONE;

    switch(target)
    {
    case EGL_GL_TEXTURE_2D_KHR:                  textureTarget = GL_TEXTURE_2D;                  break;
    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR: textureTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
    case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR: textureTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR: textureTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
    case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR: textureTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR: textureTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
    case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR: textureTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
    }

    if(textureTarget != GL_NONE)
    {
        es2::Texture *texture = getTexture(name);

        return texture->createSharedImage(textureTarget, textureLevel);
    }
    else if(target == EGL_GL_RENDERBUFFER_KHR)
    {
        es2::Renderbuffer *renderbuffer = getRenderbuffer(name);

        return renderbuffer->createSharedImage();
    }
    else UNREACHABLE();

	return 0;
}

Device *Context::getDevice()
{
	if(!device)
	{
		sw::Context *context = new sw::Context();
		device = new es2::Device(context);
	}

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
