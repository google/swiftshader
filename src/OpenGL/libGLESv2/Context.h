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

#ifndef LIBGLESV2_CONTEXT_H_
#define LIBGLESV2_CONTEXT_H_

#include "libEGL/Context.hpp"
#include "ResourceManager.h"
#include "common/NameSpace.hpp"
#include "common/Object.hpp"
#include "common/Image.hpp"
#include "Renderer/Sampler.hpp"
#include "TransformFeedback.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include <map>
#include <string>

namespace egl
{
class Display;
class Surface;
class Config;
}

namespace es2
{
struct TranslatedAttribute;
struct TranslatedIndexData;

class Device;
class Buffer;
class Shader;
class Program;
class Texture;
class Texture2D;
class Texture3D;
class TextureCubeMap;
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
class Fence;
class Query;
class Sampler;
class VertexArray;

enum
{
    MAX_VERTEX_ATTRIBS = 16,
	MAX_UNIFORM_VECTORS = 256,   // Device limit
    MAX_VERTEX_UNIFORM_VECTORS = VERTEX_UNIFORM_VECTORS - 3,   // Reserve space for gl_DepthRange
    MAX_VARYING_VECTORS = 10,
    MAX_TEXTURE_IMAGE_UNITS = TEXTURE_IMAGE_UNITS,
    MAX_VERTEX_TEXTURE_IMAGE_UNITS = VERTEX_TEXTURE_IMAGE_UNITS,
    MAX_COMBINED_TEXTURE_IMAGE_UNITS = MAX_TEXTURE_IMAGE_UNITS + MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    MAX_FRAGMENT_UNIFORM_VECTORS = FRAGMENT_UNIFORM_VECTORS - 3,    // Reserve space for gl_DepthRange
    MAX_DRAW_BUFFERS = 1,
	MAX_ELEMENT_INDEX = 0x7FFFFFFF,
	MAX_ELEMENTS_INDICES = 0x7FFFFFFF,
	MAX_ELEMENTS_VERTICES = 0x7FFFFFFF
};

const GLenum compressedTextureFormats[] =
{
	GL_ETC1_RGB8_OES,
#if (S3TC_SUPPORT)
	GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE,
	GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE,
#endif
#if (GL_ES_VERSION_3_0)
	GL_COMPRESSED_R11_EAC,
	GL_COMPRESSED_SIGNED_R11_EAC,
	GL_COMPRESSED_RG11_EAC,
	GL_COMPRESSED_SIGNED_RG11_EAC,
	GL_COMPRESSED_RGB8_ETC2,
	GL_COMPRESSED_SRGB8_ETC2,
	GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
	GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
	GL_COMPRESSED_RGBA8_ETC2_EAC,
	GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
#endif
};

const GLint NUM_COMPRESSED_TEXTURE_FORMATS = sizeof(compressedTextureFormats) / sizeof(compressedTextureFormats[0]);

const float ALIASED_LINE_WIDTH_RANGE_MIN = 1.0f;
const float ALIASED_LINE_WIDTH_RANGE_MAX = 1.0f;
const float ALIASED_POINT_SIZE_RANGE_MIN = 0.125f;
const float ALIASED_POINT_SIZE_RANGE_MAX = 8192.0f;
const float MAX_TEXTURE_MAX_ANISOTROPY = 16.0f;

enum QueryType
{
    QUERY_ANY_SAMPLES_PASSED,
    QUERY_ANY_SAMPLES_PASSED_CONSERVATIVE,

    QUERY_TYPE_COUNT
};

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
    VertexAttribute() : mType(GL_FLOAT), mSize(0), mNormalized(false), mStride(0), mDivisor(0), mPointer(NULL), mArrayEnabled(false)
    {
        mCurrentValue[0].f = 0.0f;
        mCurrentValue[1].f = 0.0f;
        mCurrentValue[2].f = 0.0f;
        mCurrentValue[3].f = 1.0f;
		mCurrentValueType = ValueUnion::FloatType;
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

	inline float getCurrentValue(int i) const
	{
		switch(mCurrentValueType)
		{
		case ValueUnion::FloatType:	return mCurrentValue[i].f;
		case ValueUnion::IntType:	return static_cast<float>(mCurrentValue[i].i);
		case ValueUnion::UIntType:	return static_cast<float>(mCurrentValue[i].ui);
		default: UNREACHABLE();		return mCurrentValue[i].f;
		}
	}

	inline GLint getCurrentValueI(int i) const
	{
		switch(mCurrentValueType)
		{
		case ValueUnion::FloatType:	return static_cast<GLint>(mCurrentValue[i].f);
		case ValueUnion::IntType:	return mCurrentValue[i].i;
		case ValueUnion::UIntType:	return static_cast<GLint>(mCurrentValue[i].ui);
		default: UNREACHABLE();		return mCurrentValue[i].i;
		}
	}

	inline GLuint getCurrentValueUI(int i) const
	{
		switch(mCurrentValueType)
		{
		case ValueUnion::FloatType:	return static_cast<GLuint>(mCurrentValue[i].f);
		case ValueUnion::IntType:	return static_cast<GLuint>(mCurrentValue[i].i);
		case ValueUnion::UIntType:	return mCurrentValue[i].ui;
		default: UNREACHABLE();		return mCurrentValue[i].ui;
		}
	}

	inline void setCurrentValue(const GLfloat *values)
	{
		mCurrentValue[0].f = values[0];
		mCurrentValue[1].f = values[1];
		mCurrentValue[2].f = values[2];
		mCurrentValue[3].f = values[3];
		mCurrentValueType = ValueUnion::FloatType;
	}

	inline void setCurrentValue(const GLint *values)
	{
		mCurrentValue[0].i = values[0];
		mCurrentValue[1].i = values[1];
		mCurrentValue[2].i = values[2];
		mCurrentValue[3].i = values[3];
		mCurrentValueType = ValueUnion::IntType;
	}

	inline void setCurrentValue(const GLuint *values)
	{
		mCurrentValue[0].ui = values[0];
		mCurrentValue[1].ui = values[1];
		mCurrentValue[2].ui = values[2];
		mCurrentValue[3].ui = values[3];
		mCurrentValueType = ValueUnion::UIntType;
	}

    // From glVertexAttribPointer
    GLenum mType;
    GLint mSize;
    bool mNormalized;
    GLsizei mStride;   // 0 means natural stride
    GLuint mDivisor;   // From glVertexAttribDivisor

    union
    {
        const void *mPointer;
        intptr_t mOffset;
    };

    gl::BindingPointer<Buffer> mBoundBuffer;   // Captured when glVertexAttribPointer is called.

    bool mArrayEnabled;   // From glEnable/DisableVertexAttribArray
private:
	union ValueUnion
	{
		enum Type { FloatType, IntType, UIntType };

		float f;
		GLint i;
		GLuint ui;
	};
	ValueUnion mCurrentValue[4];   // From glVertexAttrib
	ValueUnion::Type mCurrentValueType;
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
    Color blendColor;
    bool stencilTest;
    GLenum stencilFunc;
    GLint stencilRef;
    GLuint stencilMask;
    GLenum stencilFail;
    GLenum stencilPassDepthFail;
    GLenum stencilPassDepthPass;
    GLuint stencilWritemask;
    GLenum stencilBackFunc;
    GLint stencilBackRef;
    GLuint stencilBackMask;
    GLenum stencilBackFail;
    GLenum stencilBackPassDepthFail;
    GLenum stencilBackPassDepthPass;
    GLuint stencilBackWritemask;
    bool polygonOffsetFill;
    GLfloat polygonOffsetFactor;
    GLfloat polygonOffsetUnits;
    bool sampleAlphaToCoverage;
    bool sampleCoverage;
    GLclampf sampleCoverageValue;
    bool sampleCoverageInvert;
    bool scissorTest;
    bool dither;
    bool primitiveRestartFixedIndex;
    bool rasterizerDiscard;

    GLfloat lineWidth;

    GLenum generateMipmapHint;
    GLenum fragmentShaderDerivativeHint;

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
    gl::BindingPointer<Buffer> arrayBuffer;
	gl::BindingPointer<Buffer> copyReadBuffer;
	gl::BindingPointer<Buffer> copyWriteBuffer;
	gl::BindingPointer<Buffer> pixelPackBuffer;
	gl::BindingPointer<Buffer> pixelUnpackBuffer;
	gl::BindingPointer<Buffer> uniformBuffer;

    GLuint readFramebuffer;
    GLuint drawFramebuffer;
    gl::BindingPointer<Renderbuffer> renderbuffer;
    GLuint currentProgram;
	GLuint vertexArray;
	GLuint transformFeedback;
	gl::BindingPointer<Sampler> sampler[MAX_COMBINED_TEXTURE_IMAGE_UNITS];

    VertexAttribute vertexAttribute[MAX_VERTEX_ATTRIBS];
    gl::BindingPointer<Texture> samplerTexture[TEXTURE_TYPE_COUNT][MAX_COMBINED_TEXTURE_IMAGE_UNITS];
	gl::BindingPointer<Query> activeQuery[QUERY_TYPE_COUNT];

    GLint unpackAlignment;
    GLint packAlignment;
};

class Context : public egl::Context
{
public:
    Context(const egl::Config *config, const Context *shareContext, EGLint clientVersion);

	virtual void makeCurrent(egl::Surface *surface);
	virtual EGLint getClientVersion() const;

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
    void setBlendColor(float red, float green, float blue, float alpha);
    void setBlendEquation(GLenum rgbEquation, GLenum alphaEquation);

    void setStencilTest(bool enabled);
    bool isStencilTestEnabled() const;
    void setStencilParams(GLenum stencilFunc, GLint stencilRef, GLuint stencilMask);
    void setStencilBackParams(GLenum stencilBackFunc, GLint stencilBackRef, GLuint stencilBackMask);
    void setStencilWritemask(GLuint stencilWritemask);
    void setStencilBackWritemask(GLuint stencilBackWritemask);
    void setStencilOperations(GLenum stencilFail, GLenum stencilPassDepthFail, GLenum stencilPassDepthPass);
    void setStencilBackOperations(GLenum stencilBackFail, GLenum stencilBackPassDepthFail, GLenum stencilBackPassDepthPass);

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

    void setPrimitiveRestartFixedIndex(bool enabled);
    bool isPrimitiveRestartFixedIndexEnabled() const;

    void setRasterizerDiscard(bool enabled);
    bool isRasterizerDiscardEnabled() const;

    void setLineWidth(GLfloat width);

    void setGenerateMipmapHint(GLenum hint);
    void setFragmentShaderDerivativeHint(GLenum hint);

    void setViewportParams(GLint x, GLint y, GLsizei width, GLsizei height);

	void setScissorTest(bool enabled);
    bool isScissorTestEnabled() const;
    void setScissorParams(GLint x, GLint y, GLsizei width, GLsizei height);

    void setColorMask(bool red, bool green, bool blue, bool alpha);
    void setDepthMask(bool mask);

    void setActiveSampler(unsigned int active);

    GLuint getReadFramebufferName() const;
    GLuint getDrawFramebufferName() const;
    GLuint getRenderbufferName() const;

	GLuint getActiveQuery(GLenum target) const;

    GLuint getArrayBufferName() const;
	GLuint getElementArrayBufferName() const;

    void setEnableVertexAttribArray(unsigned int attribNum, bool enabled);
    void setVertexAttribDivisor(unsigned int attribNum, GLuint divisor);
    const VertexAttribute &getVertexAttribState(unsigned int attribNum) const;
    void setVertexAttribState(unsigned int attribNum, Buffer *boundBuffer, GLint size, GLenum type,
                              bool normalized, GLsizei stride, const void *pointer);
    const void *getVertexAttribPointer(unsigned int attribNum) const;

	const VertexAttributeArray &getVertexArrayAttributes();
	// Context attribute current values can be queried independently from VAO current values
	const VertexAttributeArray &getCurrentVertexAttributes();

    void setUnpackAlignment(GLint alignment);
    GLint getUnpackAlignment() const;

    void setPackAlignment(GLint alignment);
    GLint getPackAlignment() const;

    // These create  and destroy methods are merely pass-throughs to 
    // ResourceManager, which owns these object types
    GLuint createBuffer();
    GLuint createShader(GLenum type);
    GLuint createProgram();
    GLuint createTexture();
    GLuint createRenderbuffer();

    void deleteBuffer(GLuint buffer);
    void deleteShader(GLuint shader);
    void deleteProgram(GLuint program);
    void deleteTexture(GLuint texture);
    void deleteRenderbuffer(GLuint renderbuffer);

    // Framebuffers are owned by the Context, so these methods do not pass through
    GLuint createFramebuffer();
    void deleteFramebuffer(GLuint framebuffer);

    // Fences are owned by the Context
    GLuint createFence();
    void deleteFence(GLuint fence);

	// Queries are owned by the Context
    GLuint createQuery();
    void deleteQuery(GLuint query);

	// Vertex arrays are owned by the Context
	GLuint createVertexArray();
	void deleteVertexArray(GLuint array);

	// Transform feedbacks are owned by the Context
	GLuint createTransformFeedback();
	void deleteTransformFeedback(GLuint transformFeedback);

	// Samplers are owned by the Context
	GLuint createSampler();
	void deleteSampler(GLuint sampler);

    void bindArrayBuffer(GLuint buffer);
    void bindElementArrayBuffer(GLuint buffer);
	void bindCopyReadBuffer(GLuint buffer);
	void bindCopyWriteBuffer(GLuint buffer);
	void bindPixelPackBuffer(GLuint buffer);
	void bindPixelUnpackBuffer(GLuint buffer);
	void bindTransformFeedbackBuffer(GLuint buffer);
	void bindUniformBuffer(GLuint buffer);
    void bindTexture2D(GLuint texture);
    void bindTextureCubeMap(GLuint texture);
    void bindTextureExternal(GLuint texture);
	void bindTexture3D(GLuint texture);
    void bindReadFramebuffer(GLuint framebuffer);
    void bindDrawFramebuffer(GLuint framebuffer);
    void bindRenderbuffer(GLuint renderbuffer);
	bool bindVertexArray(GLuint array);
	bool bindTransformFeedback(GLuint transformFeedback);
	bool bindSampler(GLuint unit, GLuint sampler);
    void useProgram(GLuint program);

	void beginQuery(GLenum target, GLuint query);
    void endQuery(GLenum target);

    void setFramebufferZero(Framebuffer *framebuffer);

    void setRenderbufferStorage(RenderbufferStorage *renderbuffer);

    void setVertexAttrib(GLuint index, const GLfloat *values);
    void setVertexAttrib(GLuint index, const GLint *values);
    void setVertexAttrib(GLuint index, const GLuint *values);

	Buffer *getBuffer(GLuint handle) const;
	Fence *getFence(GLuint handle) const;
	Shader *getShader(GLuint handle) const;
	Program *getProgram(GLuint handle) const;
	virtual Texture *getTexture(GLuint handle) const;
	Framebuffer *getFramebuffer(GLuint handle) const;
	virtual Renderbuffer *getRenderbuffer(GLuint handle) const;
	Query *getQuery(GLuint handle) const;
	VertexArray *getVertexArray(GLuint array) const;
	VertexArray *getCurrentVertexArray() const;
	TransformFeedback *getTransformFeedback(GLuint transformFeedback) const;
	TransformFeedback *getTransformFeedback() const;
	Sampler *getSampler(GLuint sampler) const;

	Buffer *getArrayBuffer() const;
	Buffer *getElementArrayBuffer() const;
	Buffer *getCopyReadBuffer() const;
	Buffer *getCopyWriteBuffer() const;
	Buffer *getPixelPackBuffer() const;
	Buffer *getPixelUnpackBuffer() const;
	Buffer *getUniformBuffer() const;
	bool getBuffer(GLenum target, es2::Buffer **buffer) const;
	Program *getCurrentProgram() const;
	Texture2D *getTexture2D() const;
	Texture3D *getTexture3D() const;
	TextureCubeMap *getTextureCubeMap() const;
	TextureExternal *getTextureExternal() const;
	Texture *getSamplerTexture(unsigned int sampler, TextureType type) const;
	Framebuffer *getReadFramebuffer() const;
	Framebuffer *getDrawFramebuffer() const;

	bool getFloatv(GLenum pname, GLfloat *params) const;
	bool getIntegerv(GLenum pname, GLint *params) const;
	bool getBooleanv(GLenum pname, GLboolean *params) const;
	bool getTransformFeedbackiv(GLuint xfb, GLenum pname, GLint *param) const;

	bool getQueryParameterInfo(GLenum pname, GLenum *type, unsigned int *numParams) const;

	bool hasZeroDivisor() const;

    void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei *bufSize, void* pixels);
    void clear(GLbitfield mask);
    void drawArrays(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount = 1);
    void drawElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount = 1);
    void finish();
    void flush();

    void recordInvalidEnum();
    void recordInvalidValue();
    void recordInvalidOperation();
    void recordOutOfMemory();
    void recordInvalidFramebufferOperation();

    GLenum getError();

    static int getSupportedMultiSampleDepth(sw::Format format, int requested);
    
    void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, 
                         GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                         GLbitfield mask);

	virtual void bindTexImage(egl::Surface *surface);
	virtual EGLenum validateSharedImage(EGLenum target, GLuint name, GLuint textureLevel);
	virtual egl::Image *createSharedImage(EGLenum target, GLuint name, GLuint textureLevel);

	Device *getDevice();

	const GLubyte* getExtensions(GLuint index, GLuint* numExt = nullptr) const;

private:
	virtual ~Context();

    bool applyRenderTarget();
    void applyState(GLenum drawMode);
	GLenum applyVertexBuffer(GLint base, GLint first, GLsizei count, GLsizei instanceId);
    GLenum applyIndexBuffer(const void *indices, GLuint start, GLuint end, GLsizei count, GLenum mode, GLenum type, TranslatedIndexData *indexInfo);
    void applyShaders();
    void applyTextures();
    void applyTextures(sw::SamplerType type);
	void applyTexture(sw::SamplerType type, int sampler, Texture *texture);

    void detachBuffer(GLuint buffer);
    void detachTexture(GLuint texture);
    void detachFramebuffer(GLuint framebuffer);
    void detachRenderbuffer(GLuint renderbuffer);

    bool cullSkipsDraw(GLenum drawMode);
    bool isTriangleMode(GLenum drawMode);

	Query *createQuery(GLuint handle, GLenum type);

	const EGLint clientVersion;
    const egl::Config *const mConfig;

    State mState;

	gl::BindingPointer<Texture2D> mTexture2DZero;
	gl::BindingPointer<Texture3D> mTexture3DZero;
	gl::BindingPointer<TextureCubeMap> mTextureCubeMapZero;
    gl::BindingPointer<TextureExternal> mTextureExternalZero;

    typedef std::map<GLint, Framebuffer*> FramebufferMap;
    FramebufferMap mFramebufferMap;
    gl::NameSpace mFramebufferNameSpace;

    typedef std::map<GLint, Fence*> FenceMap;
    FenceMap mFenceMap;
    gl::NameSpace mFenceNameSpace;

	typedef std::map<GLint, Query*> QueryMap;
    QueryMap mQueryMap;
    gl::NameSpace mQueryNameSpace;

	typedef std::map<GLint, VertexArray*> VertexArrayMap;
	VertexArrayMap mVertexArrayMap;
	gl::NameSpace mVertexArrayNameSpace;

	typedef std::map<GLint, TransformFeedback*> TransformFeedbackMap;
	TransformFeedbackMap mTransformFeedbackMap;
	gl::NameSpace mTransformFeedbackNameSpace;

	typedef std::map<GLint, Sampler*> SamplerMap;
	SamplerMap mSamplerMap;
	gl::NameSpace mSamplerNameSpace;

    VertexDataManager *mVertexDataManager;
    IndexDataManager *mIndexDataManager;

    // Recorded errors
    bool mInvalidEnum;
    bool mInvalidValue;
    bool mInvalidOperation;
    bool mOutOfMemory;
    bool mInvalidFramebufferOperation;

    bool mHasBeenCurrent;

    unsigned int mAppliedProgramSerial;
    
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

	Device *device;
    ResourceManager *mResourceManager;
};
}

#endif   // INCLUDE_CONTEXT_H_
