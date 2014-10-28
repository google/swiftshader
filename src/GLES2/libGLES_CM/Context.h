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

// Context.h: Defines the Context class, managing all GL state and performing
// rendering operations. It is the GLES2 specific implementation of EGLContext.

#ifndef LIBGLES_CM_CONTEXT_H_
#define LIBGLES_CM_CONTEXT_H_

#include "libEGL/Context.hpp"
#include "ResourceManager.h"
#include "HandleAllocator.h"
#include "RefCountObject.h"
#include "Image.hpp"
#include "Renderer/Sampler.hpp"

#define GL_API
#include <GLES/gl.h>
#define EGLAPI
#include <EGL/egl.h>

#include <map>
#include <string>

namespace egl
{
class Display;
class Surface;
class Config;
}

namespace gl
{
struct TranslatedAttribute;
struct TranslatedIndexData;

class Device;
class Buffer;
class Texture;
class Texture2D;
class TextureExternal;
class Framebuffer;
class Renderbuffer;
class RenderbufferStorage;
class Colorbuffer;
class Depthbuffer;
class StreamingIndexBuffer;
class Stencilbuffer;
class DepthStencilbuffer;
class VertexDataManager;
class IndexDataManager;

enum
{
    MAX_VERTEX_ATTRIBS = 16,
	MAX_UNIFORM_VECTORS = 256,   // Device limit
    MAX_VERTEX_UNIFORM_VECTORS = 256 - 3,   // Reserve space for gl_DepthRange
    MAX_VARYING_VECTORS = 10,
    MAX_TEXTURE_IMAGE_UNITS = 16,
    MAX_VERTEX_TEXTURE_IMAGE_UNITS = 4,
    MAX_COMBINED_TEXTURE_IMAGE_UNITS = MAX_TEXTURE_IMAGE_UNITS + MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    MAX_FRAGMENT_UNIFORM_VECTORS = 224 - 3,    // Reserve space for gl_DepthRange
    MAX_DRAW_BUFFERS = 1,

    IMPLEMENTATION_COLOR_READ_FORMAT = GL_RGB,
    IMPLEMENTATION_COLOR_READ_TYPE = GL_UNSIGNED_SHORT_5_6_5
};

enum QueryType
{
    QUERY_ANY_SAMPLES_PASSED,
    QUERY_ANY_SAMPLES_PASSED_CONSERVATIVE,

    QUERY_TYPE_COUNT
};

const float ALIASED_LINE_WIDTH_RANGE_MIN = 1.0f;
const float ALIASED_LINE_WIDTH_RANGE_MAX = 1.0f;
const float ALIASED_POINT_SIZE_RANGE_MIN = 0.125f;
const float ALIASED_POINT_SIZE_RANGE_MAX = 8192.0f;
const float MAX_TEXTURE_MAX_ANISOTROPY = 16.0f;

struct Color
{
    float red;
    float green;
    float blue;
    float alpha;
};

// Helper structure describing a single vertex attribute
class VertexAttribute
{
  public:
    VertexAttribute() : mType(GL_FLOAT), mSize(0), mNormalized(false), mStride(0), mPointer(NULL), mArrayEnabled(false)
    {
        mCurrentValue[0] = 0.0f;
        mCurrentValue[1] = 0.0f;
        mCurrentValue[2] = 0.0f;
        mCurrentValue[3] = 1.0f;
    }

    int typeSize() const
    {
        switch (mType)
        {
        case GL_BYTE:           return mSize * sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:  return mSize * sizeof(GLubyte);
        case GL_SHORT:          return mSize * sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return mSize * sizeof(GLushort);
        case GL_FIXED:          return mSize * sizeof(GLfixed);
        case GL_FLOAT:          return mSize * sizeof(GLfloat);
        default: UNREACHABLE(); return mSize * sizeof(GLfloat);
        }
    }

    GLsizei stride() const
    {
        return mStride ? mStride : typeSize();
    }

    // From glVertexAttribPointer
    GLenum mType;
    GLint mSize;
    bool mNormalized;
    GLsizei mStride;   // 0 means natural stride

    union
    {
        const void *mPointer;
        intptr_t mOffset;
    };

    BindingPointer<Buffer> mBoundBuffer;   // Captured when glVertexAttribPointer is called.

    bool mArrayEnabled;   // From glEnable/DisableVertexAttribArray
    float mCurrentValue[4];   // From glVertexAttrib
};

typedef VertexAttribute VertexAttributeArray[MAX_VERTEX_ATTRIBS];

// Helper structure to store all raw state
struct State
{
    Color colorClearValue;
    GLclampf depthClearValue;
    int stencilClearValue;

    bool cullFace;
    GLenum cullMode;
    GLenum frontFace;
    bool depthTest;
    GLenum depthFunc;
    bool blend;
    GLenum sourceBlendRGB;
    GLenum destBlendRGB;
    GLenum sourceBlendAlpha;
    GLenum destBlendAlpha;
    GLenum blendEquationRGB;
    GLenum blendEquationAlpha;
    bool stencilTest;
    GLenum stencilFunc;
    GLint stencilRef;
    GLuint stencilMask;
    GLenum stencilFail;
    GLenum stencilPassDepthFail;
    GLenum stencilPassDepthPass;
    GLuint stencilWritemask;
    bool polygonOffsetFill;
    GLfloat polygonOffsetFactor;
    GLfloat polygonOffsetUnits;
    bool sampleAlphaToCoverage;
    bool sampleCoverage;
    GLclampf sampleCoverageValue;
    bool sampleCoverageInvert;
    bool scissorTest;
    bool dither;

    GLfloat lineWidth;

    GLenum generateMipmapHint;

    GLint viewportX;
    GLint viewportY;
    GLsizei viewportWidth;
    GLsizei viewportHeight;
    float zNear;
    float zFar;

    GLint scissorX;
    GLint scissorY;
    GLsizei scissorWidth;
    GLsizei scissorHeight;

    bool colorMaskRed;
    bool colorMaskGreen;
    bool colorMaskBlue;
    bool colorMaskAlpha;
    bool depthMask;

    unsigned int activeSampler;   // Active texture unit selector - GL_TEXTURE0
    BindingPointer<Buffer> arrayBuffer;
    BindingPointer<Buffer> elementArrayBuffer;
    GLuint framebuffer;
    BindingPointer<Renderbuffer> renderbuffer;

    VertexAttribute vertexAttribute[MAX_VERTEX_ATTRIBS];
    BindingPointer<Texture> samplerTexture[TEXTURE_TYPE_COUNT][MAX_COMBINED_TEXTURE_IMAGE_UNITS];

    GLint unpackAlignment;
    GLint packAlignment;
};

class Context : public egl::Context
{
public:
    Context(const egl::Config *config, const Context *shareContext);

	virtual void makeCurrent(egl::Surface *surface);
	virtual void destroy();
	virtual int getClientVersion();

    void markAllStateDirty();

    // State manipulation
    void setClearColor(float red, float green, float blue, float alpha);
    void setClearDepth(float depth);
    void setClearStencil(int stencil);

    void setCullFace(bool enabled);
    bool isCullFaceEnabled() const;
    void setCullMode(GLenum mode);
    void setFrontFace(GLenum front);

    void setDepthTest(bool enabled);
    bool isDepthTestEnabled() const;
    void setDepthFunc(GLenum depthFunc);
    void setDepthRange(float zNear, float zFar);
    
    void setBlend(bool enabled);
    bool isBlendEnabled() const;
    void setBlendFactors(GLenum sourceRGB, GLenum destRGB, GLenum sourceAlpha, GLenum destAlpha);
    void setBlendEquation(GLenum rgbEquation, GLenum alphaEquation);

    void setStencilTest(bool enabled);
    bool isStencilTestEnabled() const;
    void setStencilParams(GLenum stencilFunc, GLint stencilRef, GLuint stencilMask);
    void setStencilWritemask(GLuint stencilWritemask);
    void setStencilOperations(GLenum stencilFail, GLenum stencilPassDepthFail, GLenum stencilPassDepthPass);
    
    void setPolygonOffsetFill(bool enabled);
    bool isPolygonOffsetFillEnabled() const;
    void setPolygonOffsetParams(GLfloat factor, GLfloat units);

    void setSampleAlphaToCoverage(bool enabled);
    bool isSampleAlphaToCoverageEnabled() const;
    void setSampleCoverage(bool enabled);
    bool isSampleCoverageEnabled() const;
    void setSampleCoverageParams(GLclampf value, bool invert);

    void setDither(bool enabled);
    bool isDitherEnabled() const;

    void setLineWidth(GLfloat width);

    void setGenerateMipmapHint(GLenum hint);

    void setViewportParams(GLint x, GLint y, GLsizei width, GLsizei height);

	void setScissorTest(bool enabled);
    bool isScissorTestEnabled() const;
    void setScissorParams(GLint x, GLint y, GLsizei width, GLsizei height);

    void setColorMask(bool red, bool green, bool blue, bool alpha);
    void setDepthMask(bool mask);

    void setActiveSampler(unsigned int active);

    GLuint getFramebufferHandle() const;
    GLuint getRenderbufferHandle() const;

    GLuint getArrayBufferHandle() const;

    void setEnableVertexAttribArray(unsigned int attribNum, bool enabled);
    const VertexAttribute &getVertexAttribState(unsigned int attribNum);
    void setVertexAttribState(unsigned int attribNum, Buffer *boundBuffer, GLint size, GLenum type,
                              bool normalized, GLsizei stride, const void *pointer);
    const void *getVertexAttribPointer(unsigned int attribNum) const;

    const VertexAttributeArray &getVertexAttributes();

    void setUnpackAlignment(GLint alignment);
    GLint getUnpackAlignment() const;

    void setPackAlignment(GLint alignment);
    GLint getPackAlignment() const;

    // These create  and destroy methods are merely pass-throughs to 
    // ResourceManager, which owns these object types
    GLuint createBuffer();
    GLuint createTexture();
    GLuint createRenderbuffer();

    void deleteBuffer(GLuint buffer);
    void deleteTexture(GLuint texture);
    void deleteRenderbuffer(GLuint renderbuffer);

    // Framebuffers are owned by the Context, so these methods do not pass through
    GLuint createFramebuffer();
    void deleteFramebuffer(GLuint framebuffer);

    void bindArrayBuffer(GLuint buffer);
    void bindElementArrayBuffer(GLuint buffer);
    void bindTexture2D(GLuint texture);
    void bindTextureExternal(GLuint texture);
    void bindFramebuffer(GLuint framebuffer);
    void bindRenderbuffer(GLuint renderbuffer);

    void setFramebufferZero(Framebuffer *framebuffer);

    void setRenderbufferStorage(RenderbufferStorage *renderbuffer);

    void setVertexAttrib(GLuint index, const GLfloat *values);

    Buffer *getBuffer(GLuint handle);
    virtual Texture *getTexture(GLuint handle);
    Framebuffer *getFramebuffer(GLuint handle);
    virtual Renderbuffer *getRenderbuffer(GLuint handle);

    Buffer *getArrayBuffer();
    Buffer *getElementArrayBuffer();
    Texture2D *getTexture2D();
    TextureExternal *getTextureExternal();
    Texture *getSamplerTexture(unsigned int sampler, TextureType type);
    Framebuffer *getFramebuffer();

    bool getFloatv(GLenum pname, GLfloat *params);
    bool getIntegerv(GLenum pname, GLint *params);
    bool getBooleanv(GLenum pname, GLboolean *params);

    int getQueryParameterNum(GLenum pname);
	bool isQueryParameterInt(GLenum pname);
	bool isQueryParameterFloat(GLenum pname);
	bool isQueryParameterBool(GLenum pname);

    void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei *bufSize, void* pixels);
    void clear(GLbitfield mask);
    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);
    void finish();
    void flush();

    void recordInvalidEnum();
    void recordInvalidValue();
    void recordInvalidOperation();
    void recordOutOfMemory();
    void recordInvalidFramebufferOperation();

    GLenum getError();

    static int getSupportedMultiSampleDepth(sw::Format format, int requested);

	virtual void bindTexImage(egl::Surface *surface);
	virtual EGLenum validateSharedImage(EGLenum target, GLuint name, GLuint textureLevel);
	virtual egl::Image *createSharedImage(EGLenum target, GLuint name, GLuint textureLevel);

	Device *getDevice();

private:
	virtual ~Context();

    bool applyRenderTarget();
    void applyState(GLenum drawMode);
    GLenum applyVertexBuffer(GLint base, GLint first, GLsizei count);
    GLenum applyIndexBuffer(const void *indices, GLsizei count, GLenum mode, GLenum type, TranslatedIndexData *indexInfo);
    void applyTextures();
	void applyTexture(int sampler, Texture *texture);

    void detachBuffer(GLuint buffer);
    void detachTexture(GLuint texture);
    void detachFramebuffer(GLuint framebuffer);
    void detachRenderbuffer(GLuint renderbuffer);

    bool cullSkipsDraw(GLenum drawMode);
    bool isTriangleMode(GLenum drawMode);

    const egl::Config *const mConfig;

    State mState;

    BindingPointer<Texture2D> mTexture2DZero;
    BindingPointer<TextureExternal> mTextureExternalZero;

    typedef std::map<GLint, Framebuffer*> FramebufferMap;
    FramebufferMap mFramebufferMap;
    HandleAllocator mFramebufferHandleAllocator;

    VertexDataManager *mVertexDataManager;
    IndexDataManager *mIndexDataManager;

    // Recorded errors
    bool mInvalidEnum;
    bool mInvalidValue;
    bool mInvalidOperation;
    bool mOutOfMemory;
    bool mInvalidFramebufferOperation;

    bool mHasBeenCurrent;
    
    // state caching flags
    bool mDepthStateDirty;
    bool mMaskStateDirty;
    bool mPixelPackingStateDirty;
    bool mBlendStateDirty;
    bool mStencilStateDirty;
    bool mPolygonOffsetStateDirty;
    bool mSampleStateDirty;
    bool mFrontFaceDirty;
    bool mDitherStateDirty;

    ResourceManager *mResourceManager;

	static Device *device;
};
}

#endif   // INCLUDE_CONTEXT_H_
