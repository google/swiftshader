// SwiftShader Software Renderer
//
// Copyright(c) 2014 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//
// libRAD.cpp: Implements the exported Radiance functions.

#include "main.h"
#include "Buffer.h"
#include "Context.h"
#include "Program.h"
#include "Shader.h"
#include "Texture.h"
#include "common/debug.h"
#include "Common/Version.h"
#include "Main/Register.hpp"
#include "../libEGL/Surface.h"

#define GL_APICALL
#include <RAD/rad.h>

#include <limits>
#include <deque>
#include <set>

namespace rad
{
class Object;
typedef std::set<const Object*> ObjectPool;

class Device
{
public:
	Device()
	{
		referenceCount = 1;
	}

	void reference()
	{
		sw::atomicIncrement(&referenceCount);
	}

	void release()
	{
		ASSERT(referenceCount > 0);

		if(referenceCount > 0)
		{
			sw::atomicDecrement(&referenceCount);
		}

		if(referenceCount == 0)
		{
			delete this;
		}
	}

	void addObject(const Object *object)
	{
		pool.insert(object);
	}

	void removeObject(const Object *object)
	{
		pool.erase(object);
	}

private:
	virtual ~Device()
	{
		ASSERT(referenceCount == 0);

		size_t leakedObjectCount = pool.size();

		if(leakedObjectCount != 0)
		{
			const Object *leakedObject = *pool.begin();
			ASSERT(!leakedObject);
		}
	}

	volatile int referenceCount;

	ObjectPool pool;
};

class Object
{
public:
	Object(Device *device) : device(device)
	{
		referenceCount = 1;

		device->addObject(this);
	}

	void reference()
	{
		sw::atomicIncrement(&referenceCount);
	}

	void release()
	{
		ASSERT(referenceCount > 0);

		if(referenceCount > 0)
		{
			sw::atomicDecrement(&referenceCount);
		}

		if(referenceCount == 0)
		{
			device->removeObject(this);

			delete this;
		}
	}

protected:
	virtual ~Object()
	{
		ASSERT(referenceCount == 0);
	}

	Device *const device;

private:
	volatile int referenceCount;
};

static void Release(Object *object)
{
	if(object)
	{
		object->release();
	}
}

class Buffer : public Object
{
public:
	Buffer(Device *device) : Object(device)
	{
		buffer = nullptr;
		access = 0;      // FIXME: default?
		mapAccess = 0;   // FIXME: default?
	}

	virtual ~Buffer()
	{
		if(buffer)
		{
			buffer->destruct();
		}
	}

	void storage(RADsizei size)
	{
		ASSERT(!buffer);
		const int padding = 1024;   // For SIMD processing of vertices
		buffer = new sw::Resource(size + padding);
	}

	void *map()
	{
		ASSERT(buffer);
		return const_cast<void*>(buffer->data());
	}

	sw::Resource *buffer;

	RADbitfield access;
	RADbitfield mapAccess;
};

class Sampler : public Object
{
public:
	Sampler(Device *device) : Object(device)
	{
		default();
	}

	virtual ~Sampler()
	{
	}

	void default()
	{
		minFilter = RAD_MIN_FILTER_NEAREST_MIPMAP_LINEAR;   // FIXME: default?
		magFilter = RAD_MAG_FILTER_LINEAR;                  // FIXME: default?

		wrapModeS = RAD_WRAP_MODE_REPEAT;   // FIXME: default?
		wrapModeT = RAD_WRAP_MODE_REPEAT;   // FIXME: default?
		wrapModeR = RAD_WRAP_MODE_REPEAT;   // FIXME: default?

		minLod = -1000.0f;   // FIXME: default?
		maxLod = 1000.0f;   // FIXME: default?
		lodBias = 0.0f;
		compareMode = RAD_COMPARE_MODE_NONE;
		compareFunc = RAD_COMPARE_FUNC_LEQUAL;   // FIXME: default?
		borderColor[0] = 0.0f;   // FIXME: default?
		borderColor[1] = 0.0f;   // FIXME: default?
		borderColor[2] = 0.0f;   // FIXME: default?
		borderColor[3] = 0.0f;   // FIXME: default?
		borderColorInt[0] = 0;   // FIXME: default?
		borderColorInt[1] = 0;   // FIXME: default?
		borderColorInt[2] = 0;   // FIXME: default?
		borderColorInt[3] = 0;   // FIXME: default?
	}

	RADminFilter minFilter;
	RADmagFilter magFilter;

	RADwrapMode wrapModeS;
	RADwrapMode wrapModeT;
	RADwrapMode wrapModeR;

	RADfloat minLod;
	RADfloat maxLod;
	RADfloat lodBias;
	RADcompareMode compareMode;
	RADcompareFunc compareFunc;
	RADfloat borderColor[4];
	RADuint borderColorInt[4];
};

class Texture;

struct TextureSampler
{
	TextureSampler(Texture *texture, Sampler *sampler, RADtextureTarget target, RADinternalFormat internalFormat, RADuint minLevel, RADuint numLevels, RADuint minLayer, RADuint numLayers);

	virtual TextureSampler::~TextureSampler();

	Texture *texture;
	Sampler *sampler;
	RADtextureTarget target;
	RADinternalFormat internalFormat;
	RADuint minLevel;
	RADuint numLevels;
	RADuint minLayer;
	RADuint numLayers;
};

class Texture : public Object
{
public:
	Texture(Device *device) : Object(device)
	{
		texture = nullptr;
		access = 0;   // FIXME: default?
		//target = 0;
		//levels = 0;
		//internalFormat = 0;
		//width = 0;
		//height = 0;
		//depth = 0;
		//samples = 0;
	}

	virtual ~Texture()
	{
		if(texture)
		{
			texture->release();
		}

		for(size_t i = 0; i < textureSamplers.size(); i++)
		{
			delete textureSamplers[i];
		}
	}

	es2::Texture *texture;

	RADbitfield access;

	RADtextureTarget target;
	RADsizei levels;
	RADinternalFormat internalFormat;
	RADsizei width;
	RADsizei height;
	RADsizei depth;
	RADsizei samples;

	std::vector<TextureSampler*> textureSamplers;
};

TextureSampler::TextureSampler(Texture *texture, Sampler *sampler, RADtextureTarget target, RADinternalFormat internalFormat, RADuint minLevel, RADuint numLevels, RADuint minLayer, RADuint numLayers)
{
	ASSERT(texture);
	ASSERT(sampler);
	//texture->reference();
	this->texture = texture;
	//sampler->reference();
	this->sampler = sampler;
	this->target = target;
	this->internalFormat = internalFormat;
	this->minLevel = minLevel;
	this->numLevels = numLevels;
	this->minLayer = minLayer;
	this->numLayers = numLayers;
}

TextureSampler::~TextureSampler()
{
	//texture->release();
	//sampler->release();
}

class Pass;

class Program : public Object
{
public:
	Program(Device *device) : Object(device)
	{
		program = new es2::Program(nullptr, 0);
	}

	virtual ~Program()
	{
		program->release();
	}

	es2::Program *program;
};

template<typename T, size_t n>
inline size_t arraySize(T(&)[n])
{
	return n;
}

const int RAD_MAX_COLOR_TARGETS = 1;

struct ColorState : public Object
{
	ColorState(Device *device) : Object(device)
	{
		default();
	}

	void default()
	{
		enable = RAD_FALSE;

		for(size_t i = 0; i < arraySize(blend); i++)
		{
			blend[i].default();
		}

		numTargets = 1;
		logicOpEnable = RAD_FALSE;
		logicOp = RAD_LOGIC_OP_COPY;
		alphaToCoverageEnable = RAD_FALSE;
		blendColor[0] = 0.0f;
		blendColor[1] = 0.0f;
		blendColor[2] = 0.0f;
		blendColor[3] = 0.0f;
	}

	RADboolean enable;
	
	struct Blend
	{
		Blend()
		{
			default();
		}

		void default()
		{
			enable = RAD_FALSE;
			srcFunc = RAD_BLEND_FUNC_ONE;
			dstFunc = RAD_BLEND_FUNC_ZERO;
			srcFuncAlpha = RAD_BLEND_FUNC_ONE;
			dstFuncAlpha = RAD_BLEND_FUNC_ZERO;
			modeRGB = RAD_BLEND_EQUATION_ADD;
			modeAlpha = RAD_BLEND_EQUATION_ADD;
			maskRGBA[0] = RAD_TRUE;
			maskRGBA[1] = RAD_TRUE;
			maskRGBA[2] = RAD_TRUE;
			maskRGBA[3] = RAD_TRUE;
		}

		RADboolean enable;
		RADblendFunc srcFunc;
		RADblendFunc dstFunc;
		RADblendFunc srcFuncAlpha;
		RADblendFunc dstFuncAlpha;
		RADblendEquation modeRGB;
		RADblendEquation modeAlpha;
		RADboolean maskRGBA[4];
	};

	Blend blend[RAD_MAX_COLOR_TARGETS];

	RADuint numTargets;
	RADboolean logicOpEnable;
	RADlogicOp logicOp;
	RADboolean alphaToCoverageEnable;
	RADfloat blendColor[4];
};

struct RasterState : public Object
{
	RasterState(Device *device) : Object(device)
	{
		default();
	}

	void default()
	{
		pointSize = 1.0f;
		lineWidth = 1.0f;
		cullFace = RAD_FACE_NONE;
		frontFace = RAD_FRONT_FACE_CW;
		polygonMode = RAD_POLYGON_MODE_FILL;
	
		offsetFactor = 0.0f;
		offsetUnits = 0.0f;
		offsetClamp = 0.0f;

		polygonOffsetEnables = RAD_POLYGON_OFFSET_NONE;
		discardEnable = RAD_FALSE;
		multisampleEnable = RAD_TRUE;

		samples = 0;
		sampleMask = ~0;
	}

	RADfloat pointSize;
	RADfloat lineWidth;
	RADfaceBitfield cullFace;
	RADfrontFace frontFace;
	RADpolygonMode polygonMode;
	
	RADfloat offsetFactor;
	RADfloat offsetUnits;
	RADfloat offsetClamp;

	RADpolygonOffsetEnables polygonOffsetEnables;
	RADboolean discardEnable;
	RADboolean multisampleEnable;

	RADuint samples;
	RADuint sampleMask;
};

struct DepthStencilState : public Object
{
	DepthStencilState(Device *device) : Object(device)
	{
		default();
	}

	void default()
	{
		depthTestEnable = RAD_FALSE;
		depthWriteEnable = RAD_FALSE;
		depthFunc = RAD_DEPTH_FUNC_LESS;
		stencilTestEnable = RAD_FALSE;

		stencilFuncFront = RAD_STENCIL_FUNC_ALWAYS;
		stencilRefFront = 0;
		stencilMaskFront = ~0;

		stencilFuncBack = RAD_STENCIL_FUNC_ALWAYS;
		stencilRefBack = 0;
		stencilMaskBack = ~0;

		stencilFailOpFront = RAD_STENCIL_OP_KEEP;
		depthFailOpFront = RAD_STENCIL_OP_KEEP;
		depthPassOpFront = RAD_STENCIL_OP_KEEP;

		stencilFailOpBack = RAD_STENCIL_OP_KEEP;
		depthFailOpBack = RAD_STENCIL_OP_KEEP;
		depthPassOpBack = RAD_STENCIL_OP_KEEP;

		stencilWriteMaskFront = ~0;
		stencilWriteMaskBack = ~0;
	}

	RADboolean depthTestEnable;
	RADboolean depthWriteEnable;
	RADdepthFunc depthFunc;
	RADboolean stencilTestEnable;
	
	RADstencilFunc stencilFuncFront;
	RADint stencilRefFront;
	RADuint stencilMaskFront;

	RADstencilFunc stencilFuncBack;
	RADint stencilRefBack;
	RADuint stencilMaskBack;

	RADstencilOp stencilFailOpFront;
	RADstencilOp depthFailOpFront;
	RADstencilOp depthPassOpFront;

	RADstencilOp stencilFailOpBack;
	RADstencilOp depthFailOpBack;
	RADstencilOp depthPassOpBack;

	RADuint stencilWriteMaskFront;
	RADuint stencilWriteMaskBack;
};

const int RAD_MAX_VERTEX_ATTRIB = 16;
const int RAD_MAX_VERTEX_BINDING = 16;

struct VertexState : public Object
{
	VertexState(Device *device) : Object(device)
	{
		default();
	}

	void default()
	{
		for(size_t i = 0; i < arraySize(attrib); i++)
		{
			attrib[i].default();
		}

		for(size_t i = 0; i < arraySize(binding); i++)
		{
			binding[i].default();
		}
	}

	struct Attribute
	{
		Attribute()
		{
			default();
		}

		void default()
		{
			enable = RAD_FALSE;
			bindingIndex = 0;
			numComponents = 0;
			bytesPerComponent = 0;
			type = RAD_ATTRIB_TYPE_SNORM;
			relativeOffset = 0;
		}

		RADboolean enable;
		
		RADint bindingIndex;
		
		RADint numComponents;
		RADint bytesPerComponent;
		RADattribType type;
		RADuint relativeOffset;
	};

	Attribute attrib[RAD_MAX_VERTEX_ATTRIB];

	struct Binding
	{
		Binding()
		{
			default();
		}

		void default()
		{
			group = 0;
			index = 0;
			stride = 0;
		}

		RADint group;
		RADint index;

		RADuint stride;
	};

	Binding binding[RAD_MAX_VERTEX_BINDING];
};

struct FormatState : public Object
{
	FormatState(Device *device) : Object(device)
	{
		default();
	}

	void default()
	{
		for(size_t i = 0; i < arraySize(colorFormat); i++)
		{
			colorFormat[i] = RAD_FORMAT_NONE;
		}

		depthFormat = RAD_FORMAT_NONE;
		stencilFormat = RAD_FORMAT_NONE;
		colorSamples = 0;
		depthStencilSamples = 0;
	}

	RADinternalFormat colorFormat[RAD_MAX_COLOR_TARGETS];
	RADinternalFormat depthFormat;
	RADinternalFormat stencilFormat;
	RADuint colorSamples;
	RADuint depthStencilSamples;
};

class Pipeline : public Object
{
public:
	Pipeline(Device *device) : Object(device)
	{
		vertexProgram = nullptr;
		fragmentProgram = nullptr;
		vertexState = nullptr;
		colorState = nullptr;
		rasterState = nullptr;
		depthStencilState = nullptr;
		formatState = nullptr;
		primitiveType = RAD_TRIANGLES;
	}

	virtual ~Pipeline()
	{
		Release(vertexProgram);
		Release(fragmentProgram);
		Release(vertexState);
		Release(colorState);
		Release(rasterState);
		Release(depthStencilState);
		Release(formatState);
	}

	Program *vertexProgram;
	Program *fragmentProgram;
	VertexState *vertexState;
	ColorState *colorState;
	RasterState *rasterState;
	DepthStencilState *depthStencilState;
	FormatState *formatState;
	RADprimitiveType primitiveType;
};

const int RAD_MAX_ATTACHMENTS = 1 /*depth*/ + 1 /*stencil*/ + RAD_MAX_COLOR_TARGETS;

class Pass : public Object
{
public:
	Pass(Device *device) : Object(device)
	{
		default();
	}

	virtual ~Pass()
	{
		for(size_t i = 0; i < arraySize(colorTarget); i++)
		{
			if(colorTarget[i])
			{
				colorTarget[i]->release();
			}
		}

		if(depthTarget)
		{
			depthTarget->release();
		}

		if(stencilTarget)
		{
			stencilTarget->release();
		}
	}

	void default()
	{
		numColors = 0;

		for(size_t i = 0; i < arraySize(colorTarget); i++)
		{
			colorTarget[i] = nullptr;
		}

		depthTarget = nullptr;
		stencilTarget = nullptr;

		for(size_t i = 0; i < arraySize(preserveEnable); i++)
		{
			preserveEnable[i] = RAD_TRUE;
		}
	}

	RADuint numColors;
	es2::Image *colorTarget[RAD_MAX_COLOR_TARGETS];
	es2::Image *depthTarget;
	es2::Image *stencilTarget;

	RADboolean preserveEnable[RAD_MAX_ATTACHMENTS];

	//RADuint numDiscardTextures;
	//const RADtexture *discardTextures;
	//const RADoffset2D *discardOffsets;
	//
	//RADtexture resolveTexture[RAD_MAX_ATTACHMENTS];

	//RADuint numStoreTextures;
	//const RADtexture *storeTextures;
	//const RADoffset2D *storeOffsets;
	//
	//const RADrect2D *clipRect;

	//RADuint numDependencies;
	//const RADpass *dependentPasses;
	//const RADbitfield *srcMask;
	//const RADbitfield *dstMask;
	//const RADbitfield *flushMask;
	//const RADbitfield *invalidateMask;

	//RADboolean tilingBoundary;

	//RADuint tileFilterWidth;
	//RADuint tileTilterHeight;

	//RADuint bytesPerPixel;
	//RADuint footprintFilterWidth;
	//RADuint footprintFilterHeight;
};

class Command
{
public:
	Command()
	{
		//pipeline = nullptr;
		//pass = nullptr;
	}

	virtual ~Command()
	{
		//Release(pipeline);
		//Release(pass);
	}

	virtual void execute(Pipeline *pipeline, Pass *pass) = 0;   // FIXME: Just queue as parameter?

	virtual bool isPresent() const
	{
		return false;
	}

	//void setPipeline(Pipeline *pipeline)
	//{
	//	if(pipeline)
	//	{
	//		pipeline->reference();
	//		this->pipeline = pipeline;
	//	}
	//}

	//void setPass(Pass *pass)
	//{
	//	if(pass)
	//	{
	//		pass->reference();
	//		this->pass = pass;
	//	}
	//}

protected:
	//Pipeline *pipeline;   // FIXME: Command-specific state captured in their constructor?
	//Pass *pass;
};

class CopyBufferToImage : public Command
{
public:
	CopyBufferToImage(Buffer *buffer, RADintptr bufferOffset, Texture *texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth)
	{
		ASSERT(buffer);
		ASSERT(texture);
		buffer->reference();
		this->buffer = buffer;
		this->bufferOffset = bufferOffset;
		texture->reference();
		this->texture = texture;
		this->level = level;
		this->xoffset = xoffset;
		this->yoffset = yoffset;
		this->zoffset = zoffset;
		this->width = width;
		this->height = height;
		this->depth = depth;
	}

	virtual ~CopyBufferToImage()
	{
		buffer->release();
		texture->release();
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		ASSERT(depth == 1);   // FIXME: Unimplemented
		egl::Image *image = texture->texture->getRenderTarget(GL_TEXTURE_2D, level);
		uint8_t *output = static_cast<uint8_t*>(image->lock(xoffset, yoffset, sw::LOCK_WRITEONLY));   // FIXME: Discard if whole image
		unsigned int pitch = image->getPitch();
		const uint8_t *input = static_cast<const uint8_t*>(buffer->map()) + bufferOffset;   // FIXME: Necessary to lock?

		ASSERT(texture->internalFormat == RAD_RGBA8);   // FIXME: Unimplemented
		int bytesPerTexel = 4;

		for(int y = 0; y < height; y++)
		{
			const uint8_t *source = input + y * (width * bytesPerTexel);
			uint8_t *dest = output + y * pitch;

			memcpy(dest, source, width * bytesPerTexel);
		}

		image->unlock();
	}

	Buffer *buffer;
	RADintptr bufferOffset;
	Texture *texture;
	RADint level;
	RADuint xoffset;
	RADuint yoffset;
	RADuint zoffset;
	RADsizei width;
	RADsizei height;
	RADsizei depth;
};

class Scissor : public Command
{
public:
	Scissor(RADint x, RADint y, RADint w, RADint h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	virtual ~Scissor()
	{
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		es2::Context *context = es2::getContext();
		context->setScissorParams(x, y, w, h);
	}

	RADint x;
	RADint y;
	RADint w;
	RADint h;
};

class Viewport : public Command
{
public:
	Viewport(RADint x, RADint y, RADint w, RADint h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	virtual ~Viewport()
	{
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		es2::Context *context = es2::getContext();
		context->setViewportParams(x, y, w, h);
	}

	RADint x;
	RADint y;
	RADint w;
	RADint h;
};

class ClearColor : public Command
{
public:
	ClearColor(RADuint index, const RADfloat *color)
	{
		this->index = index;
		this->color[0] = color[0];
		this->color[1] = color[1];
		this->color[2] = color[2];
		this->color[3] = color[3];
	}

	virtual ~ClearColor()
	{
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		ASSERT(pass);   // FIXME: Error if no beginPass
		ASSERT(index < pass->numColors);
		es2::Image *image = pass->colorTarget[index];

		GLenum format = image->getFormat();
		GLenum type = image->getType();
		ASSERT(format == GL_RGBA);   // FIXME
		ASSERT(type == GL_UNSIGNED_BYTE);   //  FIXME

		es2::Context *context = es2::getContext();
		int x0 = context->mState.scissorX;
		int y0 = context->mState.scissorY;
		int width = context->mState.scissorWidth;
		int height = context->mState.scissorHeight;

		image->clearColorBuffer(sw::Color<float>(color[0], color[1], color[2], color[3]), 0xF, x0, y0, width, height);
	}

	RADuint index;
	RADfloat color[4];
};

class ClearDepth : public Command
{
public:
	ClearDepth(RADfloat depth)
	{
		this->depth = depth;
	}

	virtual ~ClearDepth()
	{
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		ASSERT(pass);   // FIXME: Error if no beginPass
		es2::Image *image = pass->depthTarget;
		
		es2::Context *context = es2::getContext();
		int x0 = context->mState.scissorX;
		int y0 = context->mState.scissorY;
		int width = context->mState.scissorWidth;
		int height = context->mState.scissorHeight;

		image->clearDepthBuffer(depth, x0, y0, width, height);
	}

	RADfloat depth;
};

class Present : public Command
{
public:
	Present(Texture *texture)
	{
		texture->reference();
		this->texture = texture;
	}

	virtual ~Present()
	{
		Release(texture);
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		es2::Context *context = es2::getContext();
		sw::FrameBuffer *frameBuffer = (*es2::getDisplay()->mSurfaceSet.begin())->frameBuffer;

		egl::Image *image = texture->texture->getRenderTarget(GL_TEXTURE_2D, 0);
		void *source = image->lockInternal(0, 0, 0, sw::LOCK_READONLY, sw::PUBLIC);
		frameBuffer->flip(source, image->getInternalFormat());
		image->unlockInternal();
		image->release();
	}

	virtual bool isPresent() const
	{
		return true;
	}

	Texture *texture;
};

class DrawElements : public Command
{
public:
	DrawElements(RADprimitiveType mode, RADindexType type, RADsizei count, sw::Resource *indexBuffer, RADuint offset)
	{
		this->mode = mode;
		this->type = type;
		this->count = count;
		this->indexBuffer = indexBuffer;
		this->offset = offset;
	}

	~DrawElements()
	{
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		es2::Context *context = es2::getContext();

		ASSERT(pipeline->vertexProgram == pipeline->fragmentProgram);   // FIXME
		context->mState.program = pipeline->vertexProgram->program;

		GLenum glType = GL_UNSIGNED_SHORT;
		switch(type)
		{
		case RAD_INDEX_UNSIGNED_BYTE:  glType = GL_UNSIGNED_BYTE;  break;
		case RAD_INDEX_UNSIGNED_SHORT: glType = GL_UNSIGNED_SHORT; break;
		case RAD_INDEX_UNSIGNED_INT:   glType = GL_UNSIGNED_INT;   break;
		default: UNREACHABLE();
		}

		GLenum glMode = GL_TRIANGLES;
		switch(mode)
		{
		case RAD_TRIANGLES: glMode = GL_TRIANGLES; break;
		default: UNREACHABLE();
		}

		context->mState.colorBuffer = pass->colorTarget[0];
		context->mState.depthBuffer = pass->depthTarget;

		context->mState.depthTest = pipeline->depthStencilState->depthTestEnable;

		GLenum glDepth = GL_LESS;
		switch(pipeline->depthStencilState->depthFunc)
		{
		case RAD_DEPTH_FUNC_NEVER:    glDepth = GL_NEVER;    break;
		case RAD_DEPTH_FUNC_LESS:     glDepth = GL_LESS;     break;
		case RAD_DEPTH_FUNC_EQUAL:    glDepth = GL_EQUAL;    break;
		case RAD_DEPTH_FUNC_LEQUAL:   glDepth = GL_LEQUAL;   break;
		case RAD_DEPTH_FUNC_GREATER:  glDepth = GL_GREATER;  break;
		case RAD_DEPTH_FUNC_NOTEQUAL: glDepth = GL_NOTEQUAL; break;
		case RAD_DEPTH_FUNC_GEQUAL:   glDepth = GL_GEQUAL;   break;
		case RAD_DEPTH_FUNC_ALWAYS:   glDepth = GL_ALWAYS;   break;
		default: UNREACHABLE();
		}

		context->mState.elementArrayBuffer = indexBuffer;
		context->drawElements(glMode, count, glType, 0);
	}

	RADprimitiveType mode;
	RADindexType type;
	RADsizei count;
	sw::Resource *indexBuffer;
	RADuint offset;
};

class BindGroup : public Command
{
public:
	BindGroup(RADbitfield stages, RADuint group, RADuint count, sw::Resource *buffer, RADuint offset)
	{
		this->stages = stages;
		this->group = group;
		this->count = count;
		this->buffer = buffer;
		this->offset = offset;
	}

	virtual ~BindGroup()
	{
	}

	virtual void execute(Pipeline *pipeline, Pass *pass)
	{
		es2::Context *context = es2::getContext();

		const RADbindGroupElement *groupElements = static_cast<const RADbindGroupElement*>(buffer->data());

		// FIXME: Should parse the layout out of the shaders
		es2::Program *program = pipeline->vertexProgram->program;

		sw::Resource *element0 = reinterpret_cast<sw::Resource*>(groupElements[0].handle);
		uintptr_t offset0 = static_cast<uintptr_t>(groupElements[0].offset);
		int position = program->getAttributeLocation("position");
		context->setVertexAttribState(position, element0, 3, GL_FLOAT, GL_TRUE, 0, offset0);
		context->setEnableVertexAttribArray(position, true);

		sw::Resource *element1 = reinterpret_cast<sw::Resource*>(groupElements[1].handle);
		uintptr_t offset1 = static_cast<uintptr_t>(groupElements[1].offset);
		int tc = program->getAttributeLocation("tc");
		context->setVertexAttribState(tc, element1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, offset1);
		context->setEnableVertexAttribArray(tc, true);

		sw::Resource *element2 = reinterpret_cast<sw::Resource*>(groupElements[2].handle);
		const void *uniform = static_cast<const uint8_t*>(element2->data()) + groupElements[2].offset;
		int scale = program->getUniformLocation("scale");
		program->setUniform4fv(scale, 1, (const GLfloat*)uniform);

		rad::TextureSampler *element3 = reinterpret_cast<rad::TextureSampler*>(groupElements[3].handle);
		int tex = program->getUniformLocation("tex");
		int sampler = 0;
		program->setUniform1iv(tex, 1, &sampler);
		context->applyTexture(sw::SAMPLER_PIXEL, sampler, element3->texture->texture);
	}

	RADbitfield stages;
	RADuint group;
	RADuint count;
	sw::Resource *buffer;
	RADuint offset;
};

class Queue : public Object
{
public:
	Queue(Device *device) : Object(device)
	{
		graphicsPipeline = nullptr;
		pass = nullptr;
	}

	virtual ~Queue()
	{
		for(size_t i = 0; i < commands.size(); i++)
		{
			delete commands[i];
		}

		Release(graphicsPipeline);

		ASSERT(!pass);   // FIXME: No matching endPass
		Release(pass);
	}

	void submit(Command *command)
	{
		// FIXME: Make BeginPass/EndPass/BindPipeline commands too?
		//command->setPipeline(graphicsPipeline);
		//command->setPass(pass);

		if(false)   // Queued execution
		{
			commands.push_back(command);

			if(command->isPresent())
			{
				// FIXME: Flush
			}
		}
		else   // Immediate execution
		{
			command->execute(graphicsPipeline, pass);

			delete command;
		}
	}

	void bindPipeline(RADpipelineType pipelineType, Pipeline *pipeline)
	{
		if(pipelineType == RAD_PIPELINE_TYPE_GRAPHICS)
		{
			pipeline->reference();

			if(graphicsPipeline)
			{
				graphicsPipeline->release();
			}

			graphicsPipeline = pipeline;
		}
		else UNREACHABLE();
	}

	void beginPass(rad::Pass *pass)
	{
		ASSERT(!this->pass);   // FIXME: Can be nested?
		pass->reference();
		this->pass = pass;
	}

	void endPass(rad::Pass *pass)
	{
		ASSERT(this->pass == pass);   // FIXME: Can be nested?
		this->pass->release();
		this->pass = nullptr;
	}

private:
	std::deque<Command*> commands;
	Pipeline *graphicsPipeline;
	Pass *pass;
};
}

extern "C"
{
RADdevice RADAPIENTRY radCreateDevice(void)
{
	static rad::Device *device = new rad::Device();
	return reinterpret_cast<RADdevice>(device);
}

void RADAPIENTRY radReferenceDevice(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	return radDevice->reference();
}
void RADAPIENTRY radReleaseDevice(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	return radDevice->release();
}

RADuint RADAPIENTRY radGetTokenHeader(RADdevice device, RADtokenName name) {UNIMPLEMENTED(); return 0;}

RADqueue RADAPIENTRY radCreateQueue(RADdevice device, RADqueueType queuetype)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Queue *queue = new rad::Queue(radDevice);
	return reinterpret_cast<RADqueue>(queue);
}

void RADAPIENTRY radReferenceQueue(RADqueue queue)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	return radQueue->reference();
}

void RADAPIENTRY radReleaseQueue(RADqueue queue)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	return radQueue->release();
}

void RADAPIENTRY radQueueTagBuffer(RADqueue queue, RADbuffer buffer) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueTagTexture(RADqueue queue, RADtexture texture) {UNIMPLEMENTED();}

void RADAPIENTRY radQueueSubmitCommands(RADqueue queue, RADuint numCommands, const RADcommandHandle *handles)
{
	return;
}

void RADAPIENTRY radFlushQueue(RADqueue queue) {UNIMPLEMENTED();}
void RADAPIENTRY radFinishQueue(RADqueue queue) {UNIMPLEMENTED();}

void RADAPIENTRY radQueueViewport(RADqueue queue, RADint x, RADint y, RADint w, RADint h)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Viewport *command = new rad::Viewport(x, y, w, h);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueScissor(RADqueue queue, RADint x, RADint y, RADint w, RADint h)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Scissor *command = new rad::Scissor(x, y, w, h);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueCopyBufferToImage(RADqueue queue, RADbuffer buffer, RADintptr bufferOffset, RADtexture texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	rad::CopyBufferToImage *command = new rad::CopyBufferToImage(radBuffer, bufferOffset, radTexture, level, xoffset, yoffset, zoffset, width, height, depth);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueCopyImageToBuffer(RADqueue queue, RADbuffer buffer, RADintptr bufferOffset, RADtexture texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth) {UNIMPLEMENTED();}

void RADAPIENTRY radQueueCopyBuffer(RADqueue queue, RADbuffer srcBuffer, RADintptr srcOffset, RADbuffer dstBuffer, RADintptr dstOffset, RADsizei size)
{
	return;
}

void RADAPIENTRY radQueueClearColor(RADqueue queue, RADuint index, const RADfloat *color)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::ClearColor *command = new rad::ClearColor(index, color);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueClearDepth(RADqueue queue, RADfloat depth)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::ClearDepth *command = new rad::ClearDepth(depth);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueClearStencil(RADqueue queue, RADuint stencil) {UNIMPLEMENTED();}

void RADAPIENTRY radQueuePresent(RADqueue queue, RADtexture texture)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	rad::Present *command = new rad::Present(radTexture);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueDrawArrays(RADqueue queue, RADprimitiveType mode, RADint first, RADsizei count) {UNIMPLEMENTED();}

void RADAPIENTRY radQueueDrawElements(RADqueue queue, RADprimitiveType mode, RADindexType type, RADsizei count, RADindexHandle indexHandle, RADuint offset)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	sw::Resource *indexBuffer = reinterpret_cast<sw::Resource*>(indexHandle);
	rad::DrawElements *command = new rad::DrawElements(mode, type, count, indexBuffer, offset);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueBindPipeline(RADqueue queue, RADpipelineType pipelineType, RADpipelineHandle pipelineHandle)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipelineHandle);
	radQueue->bindPipeline(pipelineType, radPipeline);
}

void RADAPIENTRY radQueueBindGroup(RADqueue queue, RADbitfield stages, RADuint group, RADuint count, RADbindGroupHandle groupHandle, RADuint offset)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	sw::Resource *groupBuffer = reinterpret_cast<sw::Resource*>(groupHandle);
	rad::BindGroup *command = new rad::BindGroup(stages, group, count, groupBuffer, offset);
	radQueue->submit(command);
}

void RADAPIENTRY radQueueBeginPass(RADqueue queue, RADpass pass)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Pass *radPass = reinterpret_cast<rad::Pass*>(pass);
	radQueue->beginPass(radPass);
}

void RADAPIENTRY radQueueEndPass(RADqueue queue, RADpass pass)
{
	rad::Queue *radQueue = reinterpret_cast<rad::Queue*>(queue);
	rad::Pass *radPass = reinterpret_cast<rad::Pass*>(pass);
	radQueue->endPass(radPass);
}

void RADAPIENTRY radQueueSubmitDynamic(RADqueue queue, const void *dynamic, RADsizei length) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueStencilValueMask(RADqueue queue, RADfaceBitfield faces, RADuint mask) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueStencilMask(RADqueue queue, RADfaceBitfield faces, RADuint mask) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueStencilRef(RADqueue queue, RADfaceBitfield faces, RADint ref) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueBlendColor(RADqueue queue, const RADfloat *blendColor) {UNIMPLEMENTED();}
void RADAPIENTRY radQueuePointSize(RADqueue queue, RADfloat pointSize) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueLineWidth(RADqueue queue, RADfloat lineWidth) {UNIMPLEMENTED();}
void RADAPIENTRY radQueuePolygonOffsetClamp(RADqueue queue, RADfloat factor, RADfloat units, RADfloat clamp) {UNIMPLEMENTED();}
void RADAPIENTRY radQueueSampleMask(RADqueue queue, RADuint mask) {UNIMPLEMENTED();}

RADprogram RADAPIENTRY radCreateProgram(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Program *program = new rad::Program(radDevice);
	return reinterpret_cast<RADprogram>(program);
}

void RADAPIENTRY radReferenceProgram(RADprogram program)
{
	rad::Program *radProgram = reinterpret_cast<rad::Program*>(program);
	radProgram->reference();
}

void RADAPIENTRY radReleaseProgram(RADprogram program)
{
	rad::Program *radProgram = reinterpret_cast<rad::Program*>(program);
	radProgram->release();
}

void RADAPIENTRY radProgramSource(RADprogram program, RADprogramFormat format, RADsizei length, const void *source)
{
	rad::Program *radProgram = reinterpret_cast<rad::Program*>(program);

	// FIXME: Assumes first source is vertex shader, second is fragment shader
	ASSERT(length == 2);
	const char *vertexSource = static_cast<const char* const*>(source)[0];
	const char *fragmentSource = static_cast<const char* const*>(source)[1];
	GLint vertexLength = strlen(vertexSource);
	GLint fragmentLength = strlen(fragmentSource);

	es2::VertexShader *vertexShader = new es2::VertexShader(nullptr, 0);
	es2::FragmentShader *fragmentShader = new es2::FragmentShader(nullptr, 0);

	vertexShader->setSource(1, &vertexSource, &vertexLength);
	fragmentShader->setSource(1, &fragmentSource, &fragmentLength);

	vertexShader->compile();
	fragmentShader->compile();

	radProgram->program->attachShader(vertexShader);
	radProgram->program->attachShader(fragmentShader);
	radProgram->program->link();

	vertexShader->release();     // Still referenced by program
	fragmentShader->release();   // Still referenced by program
}

RADbuffer RADAPIENTRY radCreateBuffer(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Buffer *buffer = new rad::Buffer(radDevice);
	return reinterpret_cast<RADbuffer>(buffer);
}

void RADAPIENTRY radReferenceBuffer(RADbuffer buffer)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	radBuffer->reference();
}

void RADAPIENTRY radReleaseBuffer(RADbuffer buffer, RADtagMode tagMode)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	radBuffer->release();
}

void RADAPIENTRY radBufferAccess(RADbuffer buffer, RADbitfield access)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	radBuffer->access = access;
}

void RADAPIENTRY radBufferMapAccess(RADbuffer buffer, RADbitfield mapAccess)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	radBuffer->mapAccess = mapAccess;
}

void RADAPIENTRY radBufferStorage(RADbuffer buffer, RADsizei size)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	radBuffer->storage(size);
}

void* RADAPIENTRY radMapBuffer(RADbuffer buffer)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	return radBuffer->map();
}

RADvertexHandle RADAPIENTRY radGetVertexHandle(RADbuffer buffer)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	return reinterpret_cast<RADvertexHandle>(radBuffer->buffer);
}

RADindexHandle RADAPIENTRY radGetIndexHandle(RADbuffer buffer)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	return reinterpret_cast<RADvertexHandle>(radBuffer->buffer);
}

RADuniformHandle RADAPIENTRY radGetUniformHandle(RADbuffer buffer)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	return reinterpret_cast<RADuniformHandle>(radBuffer->buffer);
}

RADbindGroupHandle RADAPIENTRY radGetBindGroupHandle(RADbuffer buffer)
{
	rad::Buffer *radBuffer = reinterpret_cast<rad::Buffer*>(buffer);
	return reinterpret_cast<RADbindGroupHandle>(radBuffer->buffer);
}

RADtexture RADAPIENTRY radCreateTexture(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Texture *colorState = new rad::Texture(radDevice);
	return reinterpret_cast<RADtexture>(colorState);
}

void RADAPIENTRY radReferenceTexture(RADtexture texture)
{
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	radTexture->reference();
}

void RADAPIENTRY radReleaseTexture(RADtexture texture, RADtagMode tagMode)
{
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	radTexture->release();
}

void RADAPIENTRY radTextureAccess(RADtexture texture, RADbitfield access)
{
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	radTexture->access = access;
}

void RADAPIENTRY radTextureStorage(RADtexture texture, RADtextureTarget target, RADsizei levels, RADinternalFormat internalFormat, RADsizei width, RADsizei height, RADsizei depth, RADsizei samples)
{
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	ASSERT(!radTexture->texture);

	radTexture->target = target;
	radTexture->levels = levels;
	radTexture->internalFormat = internalFormat;
	radTexture->width = width;
	radTexture->height = height;
	radTexture->depth = depth;
	radTexture->samples = samples;

	GLenum format = GL_NONE;
	GLenum type = GL_NONE;
	switch(internalFormat)
	{
	case RAD_RGBA8:
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case RAD_DEPTH24_STENCIL8:
		format = GL_DEPTH_STENCIL_OES;
		type = GL_UNSIGNED_INT_24_8_OES;
		break;
	default:
		UNIMPLEMENTED();   // FIXME
	}

	switch(target)
	{
	case RAD_TEXTURE_2D:
		{
			es2::Texture2D *tex = new es2::Texture2D(0);
			for(int level = 0; level < levels; level++)
			{
				tex->setImage(level, width >> level, height >> level, format, type, 1, nullptr);
			}
			tex->addRef();
			radTexture->texture = tex;
		}
		break;
	default:
		UNIMPLEMENTED();   // FIXME
	}
}

RADtextureHandle RADAPIENTRY radGetTextureSamplerHandle(RADtexture texture, RADsampler sampler, RADtextureTarget target, RADinternalFormat internalFormat, RADuint minLevel, RADuint numLevels, RADuint minLayer, RADuint numLayers)
{
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	rad::TextureSampler *textureSampler = new rad::TextureSampler(radTexture, radSampler, target, internalFormat, minLevel, numLevels, minLayer, numLayers);
	radTexture->textureSamplers.push_back(textureSampler);   // FIXME: Check for matching existing textureSampler
	return reinterpret_cast<RADtextureHandle>(textureSampler);
}

RADrenderTargetHandle RADAPIENTRY radGetTextureRenderTargetHandle(RADtexture texture, RADtextureTarget target, RADinternalFormat internalFormat, RADuint level, RADuint minLayer, RADuint numLayers)
{
	rad::Texture *radTexture = reinterpret_cast<rad::Texture*>(texture);
	ASSERT(radTexture->texture);
	ASSERT(radTexture->internalFormat == internalFormat);
	ASSERT(minLayer == 0);
	ASSERT(numLayers == 1);
	GLenum glTarget = GL_NONE;
	switch(target)
	{
	case RAD_TEXTURE_2D:
		glTarget = GL_TEXTURE_2D;
		break;
	default:
		UNREACHABLE();
	}
	return reinterpret_cast<RADrenderTargetHandle>(radTexture->texture->getRenderTarget(glTarget, level));
}

RADsampler RADAPIENTRY radCreateSampler(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Sampler *sampler = new rad::Sampler(radDevice);
	return reinterpret_cast<RADsampler>(sampler);
}

void RADAPIENTRY radReferenceSampler(RADsampler sampler)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->reference();
}

void RADAPIENTRY radReleaseSampler(RADsampler sampler)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->release();
}

void RADAPIENTRY radSamplerDefault(RADsampler sampler)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->default();
}

void RADAPIENTRY radSamplerMinMagFilter(RADsampler sampler, RADminFilter min, RADmagFilter mag)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->minFilter = min;
	radSampler->magFilter = mag;
}

void RADAPIENTRY radSamplerWrapMode(RADsampler sampler, RADwrapMode s, RADwrapMode t, RADwrapMode r)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->wrapModeS = s;
	radSampler->wrapModeT = t;
	radSampler->wrapModeR = r;
}

void RADAPIENTRY radSamplerLodClamp(RADsampler sampler, RADfloat min, RADfloat max)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->minLod = min;
	radSampler->maxLod = max;
}

void RADAPIENTRY radSamplerLodBias(RADsampler sampler, RADfloat bias)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->lodBias = bias;
}

void RADAPIENTRY radSamplerCompare(RADsampler sampler, RADcompareMode mode, RADcompareFunc func)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->compareMode = mode;
	radSampler->compareFunc = func;
}

void RADAPIENTRY radSamplerBorderColorFloat(RADsampler sampler, const RADfloat *borderColor)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->borderColor[0] = borderColor[0];
	radSampler->borderColor[1] = borderColor[1];
	radSampler->borderColor[2] = borderColor[2];
	radSampler->borderColor[3] = borderColor[3];
}

void RADAPIENTRY radSamplerBorderColorInt(RADsampler sampler, const RADuint *borderColor)
{
	rad::Sampler *radSampler = reinterpret_cast<rad::Sampler*>(sampler);
	radSampler->borderColorInt[0] = borderColor[0];
	radSampler->borderColorInt[1] = borderColor[1];
	radSampler->borderColorInt[2] = borderColor[2];
	radSampler->borderColorInt[3] = borderColor[3];
}

RADcolorState RADAPIENTRY radCreateColorState(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::ColorState *colorState = new rad::ColorState(radDevice);
	return reinterpret_cast<RADcolorState>(colorState);
}

void RADAPIENTRY radReferenceColorState(RADcolorState color)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->reference();
}

void RADAPIENTRY radReleaseColorState(RADcolorState color)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->release();
}

void RADAPIENTRY radColorDefault(RADcolorState color)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->default();
}

void RADAPIENTRY radColorBlendEnable(RADcolorState color, RADuint index, RADboolean enable)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->blend[index].enable = enable;
}

void RADAPIENTRY radColorBlendFunc(RADcolorState color, RADuint index, RADblendFunc srcFunc, RADblendFunc dstFunc, RADblendFunc srcFuncAlpha, RADblendFunc dstFuncAlpha)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->blend[index].srcFunc = srcFunc;
	colorState->blend[index].dstFunc = dstFunc;
	colorState->blend[index].srcFuncAlpha = srcFuncAlpha;
	colorState->blend[index].dstFuncAlpha = dstFuncAlpha;
}

void RADAPIENTRY radColorBlendEquation(RADcolorState color, RADuint index, RADblendEquation modeRGB, RADblendEquation modeAlpha)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->blend[index].modeRGB = modeRGB;
	colorState->blend[index].modeAlpha = modeAlpha;
}

void RADAPIENTRY radColorMask(RADcolorState color, RADuint index, RADboolean r, RADboolean g, RADboolean b, RADboolean a)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->blend[index].maskRGBA[0] = r;
	colorState->blend[index].maskRGBA[1] = g;
	colorState->blend[index].maskRGBA[2] = b;
	colorState->blend[index].maskRGBA[3] = a;
}

void RADAPIENTRY radColorNumTargets(RADcolorState color, RADuint numTargets)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->numTargets = numTargets;
}

void RADAPIENTRY radColorLogicOpEnable(RADcolorState color, RADboolean enable)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->logicOpEnable = enable;
}

void RADAPIENTRY radColorLogicOp(RADcolorState color, RADlogicOp logicOp)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->logicOp = logicOp;
}

void RADAPIENTRY radColorAlphaToCoverageEnable(RADcolorState color, RADboolean enable)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->alphaToCoverageEnable = enable;
}

void RADAPIENTRY radColorBlendColor(RADcolorState color, const RADfloat *blendColor)
{
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	colorState->blendColor[0] = blendColor[0];
	colorState->blendColor[1] = blendColor[1];
	colorState->blendColor[2] = blendColor[2];
	colorState->blendColor[3] = blendColor[3];
}

void RADAPIENTRY radColorDynamic(RADcolorState color, RADcolorDynamic dynamic, RADboolean enable) {UNIMPLEMENTED();}

RADrasterState RADAPIENTRY radCreateRasterState(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::RasterState *rasterState = new rad::RasterState(radDevice);
	return reinterpret_cast<RADrasterState>(rasterState);
}

void RADAPIENTRY radReferenceRasterState(RADrasterState raster)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->reference();
}

void RADAPIENTRY radReleaseRasterState(RADrasterState raster)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->release();
}

void RADAPIENTRY radRasterDefault(RADrasterState raster)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->default();
}

void RADAPIENTRY radRasterPointSize(RADrasterState raster, RADfloat pointSize)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->pointSize = pointSize;
}

void RADAPIENTRY radRasterLineWidth(RADrasterState raster, RADfloat lineWidth)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->lineWidth = lineWidth;
}

void RADAPIENTRY radRasterCullFace(RADrasterState raster, RADfaceBitfield face)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->cullFace = face;
}

void RADAPIENTRY radRasterFrontFace(RADrasterState raster, RADfrontFace face)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->frontFace = face;
}

void RADAPIENTRY radRasterPolygonMode(RADrasterState raster, RADpolygonMode polygonMode)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->polygonMode = polygonMode;
}

void RADAPIENTRY radRasterPolygonOffsetClamp(RADrasterState raster, RADfloat factor, RADfloat units, RADfloat clamp)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->offsetFactor = factor;
	rasterState->offsetUnits = units;
	rasterState->offsetClamp = clamp;
}

void RADAPIENTRY radRasterPolygonOffsetEnables(RADrasterState raster, RADpolygonOffsetEnables enables)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->polygonOffsetEnables = enables;
}

void RADAPIENTRY radRasterDiscardEnable(RADrasterState raster, RADboolean enable)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->discardEnable = enable;
}

void RADAPIENTRY radRasterMultisampleEnable(RADrasterState raster, RADboolean enable)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->multisampleEnable = enable;
}

void RADAPIENTRY radRasterSamples(RADrasterState raster, RADuint samples)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->samples = samples;
}

void RADAPIENTRY radRasterSampleMask(RADrasterState raster, RADuint mask)
{
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	rasterState->sampleMask = mask;
}

void RADAPIENTRY radRasterDynamic(RADrasterState raster, RADrasterDynamic dynamic, RADboolean enable)
{
	UNIMPLEMENTED();
}

RADdepthStencilState RADAPIENTRY radCreateDepthStencilState(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::DepthStencilState *depthStencilState = new rad::DepthStencilState(radDevice);
	return reinterpret_cast<RADdepthStencilState>(depthStencilState);
}

void RADAPIENTRY radReferenceDepthStencilState(RADdepthStencilState depthStencil)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->reference();
}

void RADAPIENTRY radReleaseDepthStencilState(RADdepthStencilState depthStencil)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->release();
}

void RADAPIENTRY radDepthStencilDefault(RADdepthStencilState depthStencil)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->default();
}

void RADAPIENTRY radDepthStencilDepthTestEnable(RADdepthStencilState depthStencil, RADboolean enable)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->depthTestEnable = enable;
}

void RADAPIENTRY radDepthStencilDepthWriteEnable(RADdepthStencilState depthStencil, RADboolean enable)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->depthWriteEnable = enable;
}

void RADAPIENTRY radDepthStencilDepthFunc(RADdepthStencilState depthStencil, RADdepthFunc func)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->depthFunc = func;
}

void RADAPIENTRY radDepthStencilStencilTestEnable(RADdepthStencilState depthStencil, RADboolean enable)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	depthStencilState->stencilTestEnable = enable;
}

void RADAPIENTRY radDepthStencilStencilFunc(RADdepthStencilState depthStencil, RADfaceBitfield faces, RADstencilFunc func, RADint ref, RADuint mask)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);

	if(faces & RAD_FACE_FRONT)
	{
		depthStencilState->stencilFuncFront = func;
		depthStencilState->stencilRefFront = ref;
		depthStencilState->stencilMaskFront = mask;
	}

	if(faces & RAD_FACE_BACK)
	{
		depthStencilState->stencilFuncBack = func;
		depthStencilState->stencilRefBack = ref;
		depthStencilState->stencilMaskBack = mask;
	}
}

void RADAPIENTRY radDepthStencilStencilOp(RADdepthStencilState depthStencil, RADfaceBitfield faces, RADstencilOp fail, RADstencilOp depthFail, RADstencilOp depthPass)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	
	if(faces & RAD_FACE_FRONT)
	{
		depthStencilState->stencilFailOpFront = fail;
		depthStencilState->depthFailOpFront = depthFail;
		depthStencilState->depthPassOpFront = depthPass;
	}

	if(faces & RAD_FACE_BACK)
	{
		depthStencilState->stencilFailOpBack = fail;
		depthStencilState->depthFailOpBack = depthFail;
		depthStencilState->depthPassOpBack = depthPass;
	}
}

void RADAPIENTRY radDepthStencilStencilMask(RADdepthStencilState depthStencil, RADfaceBitfield faces, RADuint mask)
{
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	
	if(faces & RAD_FACE_FRONT)
	{
		depthStencilState->stencilMaskFront = mask;
		
	}

	if(faces & RAD_FACE_BACK)
	{
		depthStencilState->stencilMaskBack = mask;
	}
}

void RADAPIENTRY radDepthStencilDynamic(RADdepthStencilState depthStencil, RADdepthStencilDynamic dynamic, RADboolean enable)
{
	UNIMPLEMENTED();
}

RADvertexState RADAPIENTRY radCreateVertexState(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::VertexState *vertexState = new rad::VertexState(radDevice);
	return reinterpret_cast<RADvertexState>(vertexState);
}

void RADAPIENTRY radReferenceVertexState(RADvertexState vertex)
{
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->reference();
}

void RADAPIENTRY radReleaseVertexState(RADvertexState vertex)
{
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->release();
}

void RADAPIENTRY radVertexDefault(RADvertexState vertex) {UNIMPLEMENTED();}

void RADAPIENTRY radVertexAttribFormat(RADvertexState vertex, RADint attribIndex, RADint numComponents, RADint bytesPerComponent, RADattribType type, RADuint relativeOffset)
{
	ASSERT(attribIndex >= 0 && attribIndex < GL_MAX_VERTEX_ATTRIBS);
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->attrib[attribIndex].numComponents = numComponents;
	vertexState->attrib[attribIndex].bytesPerComponent = bytesPerComponent;
	vertexState->attrib[attribIndex].type = type;
	vertexState->attrib[attribIndex].relativeOffset = relativeOffset;
}

void RADAPIENTRY radVertexAttribBinding(RADvertexState vertex, RADint attribIndex, RADint bindingIndex)
{
	ASSERT(attribIndex >= 0 && attribIndex < GL_MAX_VERTEX_ATTRIBS);
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->attrib[attribIndex].bindingIndex = bindingIndex;
}

void RADAPIENTRY radVertexBindingGroup(RADvertexState vertex, RADint bindingIndex, RADint group, RADint index)
{
	ASSERT(bindingIndex >= 0 && bindingIndex < GL_MAX_VERTEX_ATTRIBS);
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->binding[bindingIndex].group = group;
	vertexState->binding[bindingIndex].index = index;
}

void RADAPIENTRY radVertexAttribEnable(RADvertexState vertex, RADint attribIndex, RADboolean enable)
{
	ASSERT(attribIndex >= 0 && attribIndex < GL_MAX_VERTEX_ATTRIBS);
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->attrib[attribIndex].bindingIndex = enable;
}

void RADAPIENTRY radVertexBindingStride(RADvertexState vertex, RADint bindingIndex, RADuint stride)
{
	ASSERT(bindingIndex >= 0 && bindingIndex < GL_MAX_VERTEX_ATTRIBS);
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	vertexState->binding[bindingIndex].stride = stride;
}

RADrtFormatState RADAPIENTRY radCreateRtFormatState(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::FormatState *formatState = new rad::FormatState(radDevice);
	return reinterpret_cast<RADrtFormatState>(formatState);
}

void RADAPIENTRY radReferenceRtFormatState(RADrtFormatState rtFormat)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->reference();
}

void RADAPIENTRY radReleaseRtFormatState(RADrtFormatState rtFormat)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->release();
}

void RADAPIENTRY radRtFormatDefault(RADrtFormatState rtFormat)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->default();
}

void RADAPIENTRY radRtFormatColorFormat(RADrtFormatState rtFormat, RADuint index, RADinternalFormat format)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->colorFormat[index] = format;
}

void RADAPIENTRY radRtFormatDepthFormat(RADrtFormatState rtFormat, RADinternalFormat format)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->depthFormat = format;
}

void RADAPIENTRY radRtFormatStencilFormat(RADrtFormatState rtFormat, RADinternalFormat format)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->stencilFormat = format;
}

void RADAPIENTRY radRtFormatColorSamples(RADrtFormatState rtFormat, RADuint samples)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->colorSamples = samples;
}

void RADAPIENTRY radRtFormatDepthStencilSamples(RADrtFormatState rtFormat, RADuint samples)
{
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	formatState->depthStencilSamples = samples;
}

RADpipeline RADAPIENTRY radCreatePipeline(RADdevice device, RADpipelineType pipelineType)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Pipeline *pipeline = new rad::Pipeline(radDevice);
	return reinterpret_cast<RADpipeline>(pipeline);
}

void RADAPIENTRY radReferencePipeline(RADpipeline pipeline)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	radPipeline->reference();
}

void RADAPIENTRY radReleasePipeline(RADpipeline pipeline)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	radPipeline->release();
}

void RADAPIENTRY radPipelineProgramStages(RADpipeline pipeline, RADbitfield stages, RADprogram program)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	rad::Program *radProgram = reinterpret_cast<rad::Program*>(program);

	if(stages & RAD_VERTEX_SHADER_BIT)
	{
		ASSERT(!radPipeline->vertexProgram);
		radProgram->reference();   // FIXME: here or at compile?
		radPipeline->vertexProgram = radProgram;
	}

	if(stages & RAD_FRAGMENT_SHADER_BIT)
	{
		ASSERT(!radPipeline->fragmentProgram);
		radProgram->reference();   // FIXME: here or at compile?
		radPipeline->fragmentProgram = radProgram;
	}
}

void RADAPIENTRY radPipelineVertexState(RADpipeline pipeline, RADvertexState vertex)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	rad::VertexState *vertexState = reinterpret_cast<rad::VertexState*>(vertex);
	ASSERT(!radPipeline->vertexState);
	vertexState->reference();   // FIXME: here or at compile?
	radPipeline->vertexState = vertexState;
}

void RADAPIENTRY radPipelineColorState(RADpipeline pipeline, RADcolorState color)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	rad::ColorState *colorState = reinterpret_cast<rad::ColorState*>(color);
	ASSERT(!radPipeline->colorState);
	colorState->reference();   // FIXME: here or at compile?
	radPipeline->colorState = colorState;
}

void RADAPIENTRY radPipelineRasterState(RADpipeline pipeline, RADrasterState raster)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	rad::RasterState *rasterState = reinterpret_cast<rad::RasterState*>(raster);
	ASSERT(!radPipeline->rasterState);
	rasterState->reference();   // FIXME: here or at compile?
	radPipeline->rasterState = rasterState;
}

void RADAPIENTRY radPipelineDepthStencilState(RADpipeline pipeline, RADdepthStencilState depthStencil)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	rad::DepthStencilState *depthStencilState = reinterpret_cast<rad::DepthStencilState*>(depthStencil);
	ASSERT(!radPipeline->depthStencilState);
	depthStencilState->reference();   // FIXME: here or at compile?
	radPipeline->depthStencilState = depthStencilState;
}

void RADAPIENTRY radPipelineRtFormatState(RADpipeline pipeline, RADrtFormatState rtFormat)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	rad::FormatState *formatState = reinterpret_cast<rad::FormatState*>(rtFormat);
	ASSERT(!radPipeline->formatState);
	formatState->reference();   // FIXME: here or at compile?
	radPipeline->formatState = formatState;
}

void RADAPIENTRY radPipelinePrimitiveType(RADpipeline pipeline, RADprimitiveType mode)
{
	rad::Pipeline *radPipeline = reinterpret_cast<rad::Pipeline*>(pipeline);
	radPipeline->primitiveType = mode;
}

void RADAPIENTRY radCompilePipeline(RADpipeline pipeline)
{
	// FIXME: Reference state objects here or when set?
	return;
}

RADpipelineHandle RADAPIENTRY radGetPipelineHandle(RADpipeline pipeline)
{
	return reinterpret_cast<RADpipelineHandle>(pipeline);
}

RADcommandBuffer RADAPIENTRY radCreateCommandBuffer(RADdevice device, RADqueueType queueType)
{
	return 0;
}

void RADAPIENTRY radReferenceCommandBuffer(RADcommandBuffer cmdBuf) {UNIMPLEMENTED();}

void RADAPIENTRY radReleaseCommandBuffer(RADcommandBuffer cmdBuf)
{
	return;
}

void RADAPIENTRY radCmdBindPipeline(RADcommandBuffer cmdBuf, RADpipelineType pipelineType, RADpipelineHandle pipelineHandle)
{
	return;
}

void RADAPIENTRY radCmdBindGroup(RADcommandBuffer cmdBuf, RADbitfield stages, RADuint group, RADuint count, RADbindGroupHandle groupHandle, RADuint offset)
{
	
}

void RADAPIENTRY radCmdDrawArrays(RADcommandBuffer cmdBuf, RADprimitiveType mode, RADint first, RADsizei count) {UNIMPLEMENTED();}

void RADAPIENTRY radCmdDrawElements(RADcommandBuffer cmdBuf, RADprimitiveType mode, RADindexType type, RADsizei count, RADindexHandle indexHandle, RADuint offset)
{
	return;
}

RADboolean RADAPIENTRY radCompileCommandBuffer(RADcommandBuffer cmdBuf)
{
	return false;
}

RADcommandHandle RADAPIENTRY radGetCommandHandle(RADcommandBuffer cmdBuf)
{
	return 0;
}

void RADAPIENTRY radCmdStencilValueMask(RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADuint mask) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdStencilMask(RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADuint mask) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdStencilRef(RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADint ref) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdBlendColor(RADcommandBuffer cmdBuf, const RADfloat *blendColor) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdPointSize(RADcommandBuffer cmdBuf, RADfloat pointSize) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdLineWidth(RADcommandBuffer cmdBuf, RADfloat lineWidth) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdPolygonOffsetClamp(RADcommandBuffer cmdBuf, RADfloat factor, RADfloat units, RADfloat clamp) {UNIMPLEMENTED();}
void RADAPIENTRY radCmdSampleMask(RADcommandBuffer cmdBuf, RADuint mask) {UNIMPLEMENTED();}

RADpass RADAPIENTRY radCreatePass(RADdevice device)
{
	rad::Device *radDevice = reinterpret_cast<rad::Device*>(device);
	rad::Pass *pass = new rad::Pass(radDevice);
	return reinterpret_cast<RADpass>(pass);
}

void RADAPIENTRY radReferencePass(RADpass pass)
{
	rad::Pass *radPass = reinterpret_cast<rad::Pass*>(pass);
	radPass->reference();
}

void RADAPIENTRY radReleasePass(RADpass pass)
{
	rad::Pass *radPass = reinterpret_cast<rad::Pass*>(pass);
	radPass->release();
}

void RADAPIENTRY radPassDefault(RADpass pass)
{
	rad::Pass *radPass = reinterpret_cast<rad::Pass*>(pass);
	radPass->default();
}

void RADAPIENTRY radCompilePass(RADpass pass)
{
	return;
}

void RADAPIENTRY radPassRenderTargets(RADpass pass, RADuint numColors, const RADrenderTargetHandle *colors, RADrenderTargetHandle depth, RADrenderTargetHandle stencil)
{
	rad::Pass *radPass = reinterpret_cast<rad::Pass*>(pass);
	radPass->numColors = numColors;
	for(unsigned int i = 0; i < numColors; i++)
	{
		ASSERT(colors[i]);
		es2::Image *colorTarget = reinterpret_cast<es2::Image*>(colors[i]);
		colorTarget->addRef();   // FIXME: here or at compile?
		radPass->colorTarget[i] = colorTarget;
	}

	if(depth)
	{
		es2::Image *depthTarget = reinterpret_cast<es2::Image*>(depth);
		depthTarget->addRef();   // FIXME: here or at compile?
		radPass->depthTarget = depthTarget;
	}

	if(stencil)
	{
		es2::Image *stencilTarget = reinterpret_cast<es2::Image*>(stencil);
		stencilTarget->addRef();   // FIXME: here or at compile?
		radPass->stencilTarget = stencilTarget;
	}
}

void RADAPIENTRY radPassPreserveEnable(RADpass pass, RADrtAttachment attachment, RADboolean enable) {UNIMPLEMENTED();}
void RADAPIENTRY radPassDiscard(RADpass pass, RADuint numTextures, const RADtexture *textures, const RADoffset2D *offsets) {UNIMPLEMENTED();}

void RADAPIENTRY radPassResolve(RADpass pass, RADrtAttachment attachment, RADtexture texture)
{
	return;
}

void RADAPIENTRY radPassStore(RADpass pass, RADuint numTextures, const RADtexture *textures, const RADoffset2D *offsets) {UNIMPLEMENTED();}
void RADAPIENTRY radPassClip(RADpass pass, const RADrect2D *rect) {UNIMPLEMENTED();}
void RADAPIENTRY radPassDependencies(RADpass pass, RADuint numPasses, const RADpass *otherPasses, const RADbitfield *srcMask, const RADbitfield *dstMask, const RADbitfield *flushMask, const RADbitfield *invalidateMask) {UNIMPLEMENTED();}
void RADAPIENTRY radPassTilingBoundary(RADpass pass, RADboolean boundary) {UNIMPLEMENTED();}
void RADAPIENTRY radPassTileFilterWidth(RADpass pass, RADuint filterWidth, RADuint filterHeight) {UNIMPLEMENTED();}
void RADAPIENTRY radPassTileFootprint(RADpass pass, RADuint bytesPerPixel, RADuint maxFilterWidth, RADuint maxFilterHeight) {UNIMPLEMENTED();}

RADsync RADAPIENTRY radCreateSync(RADdevice device)
{
	return 0;
}

void RADAPIENTRY radReferenceSync(RADsync sync) {UNIMPLEMENTED();}

void RADAPIENTRY radReleaseSync(RADsync sync)
{
	return;
}

void RADAPIENTRY radQueueFenceSync(RADqueue queue, RADsync sync, RADsyncCondition condition, RADbitfield flags)
{
	return;
}

RADwaitSyncResult RADAPIENTRY radWaitSync(RADsync sync, RADuint64 timeout) {UNIMPLEMENTED(); return RAD_WAIT_SYNC_FAILED;}

RADboolean RADAPIENTRY radQueueWaitSync(RADqueue queue, RADsync sync)
{
	return 0;
}

RADPROC RADAPIENTRY radGetProcAddress(const RADchar *procname)
{
	struct Extension
    {
        const char *name;
        RADPROC address;
    };

    static const Extension glExtensions[] =
    {
        #define EXTENSION(name) {#name, (RADPROC)name}
		
		EXTENSION(radGetProcAddress),
		EXTENSION(radCreateDevice),
		EXTENSION(radReferenceDevice),
		EXTENSION(radReleaseDevice),
		EXTENSION(radGetTokenHeader),
		EXTENSION(radCreateQueue),
		EXTENSION(radReferenceQueue),
		EXTENSION(radReleaseQueue),
		EXTENSION(radQueueTagBuffer),
		EXTENSION(radQueueTagTexture),
		EXTENSION(radQueueSubmitCommands),
		EXTENSION(radFlushQueue),
		EXTENSION(radFinishQueue),
		EXTENSION(radQueueViewport),
		EXTENSION(radQueueScissor),
		EXTENSION(radQueueCopyBufferToImage),
		EXTENSION(radQueueCopyImageToBuffer),
		EXTENSION(radQueueCopyBuffer),
		EXTENSION(radQueueClearColor),
		EXTENSION(radQueueClearDepth),
		EXTENSION(radQueueClearStencil),
		EXTENSION(radQueuePresent),
		EXTENSION(radQueueDrawArrays),
		EXTENSION(radQueueDrawElements),
		EXTENSION(radQueueBindPipeline),
		EXTENSION(radQueueBindGroup),
		EXTENSION(radQueueBeginPass),
		EXTENSION(radQueueEndPass),
		EXTENSION(radQueueSubmitDynamic),
		EXTENSION(radQueueStencilValueMask),
		EXTENSION(radQueueStencilMask),
		EXTENSION(radQueueStencilRef),
		EXTENSION(radQueueBlendColor),
		EXTENSION(radQueuePointSize),
		EXTENSION(radQueueLineWidth),
		EXTENSION(radQueuePolygonOffsetClamp),
		EXTENSION(radQueueSampleMask),
		EXTENSION(radCreateProgram),
		EXTENSION(radReferenceProgram),
		EXTENSION(radReleaseProgram),
		EXTENSION(radProgramSource),
		EXTENSION(radCreateBuffer),
		EXTENSION(radReferenceBuffer),
		EXTENSION(radReleaseBuffer),
		EXTENSION(radBufferAccess),
		EXTENSION(radBufferMapAccess),
		EXTENSION(radBufferStorage),
		EXTENSION(radMapBuffer),
		EXTENSION(radGetVertexHandle),
		EXTENSION(radGetIndexHandle),
		EXTENSION(radGetUniformHandle),
		EXTENSION(radGetBindGroupHandle),
		EXTENSION(radCreateTexture),
		EXTENSION(radReferenceTexture),
		EXTENSION(radReleaseTexture),
		EXTENSION(radTextureAccess),
		EXTENSION(radTextureStorage),
		EXTENSION(radGetTextureSamplerHandle),
		EXTENSION(radGetTextureRenderTargetHandle),
		EXTENSION(radCreateSampler),
		EXTENSION(radReferenceSampler),
		EXTENSION(radReleaseSampler),
		EXTENSION(radSamplerDefault),
		EXTENSION(radSamplerMinMagFilter),
		EXTENSION(radSamplerWrapMode),
		EXTENSION(radSamplerLodClamp),
		EXTENSION(radSamplerLodBias),
		EXTENSION(radSamplerCompare),
		EXTENSION(radSamplerBorderColorFloat),
		EXTENSION(radSamplerBorderColorInt),
		EXTENSION(radCreateColorState),
		EXTENSION(radReferenceColorState),
		EXTENSION(radReleaseColorState),
		EXTENSION(radColorDefault),
		EXTENSION(radColorBlendEnable),
		EXTENSION(radColorBlendFunc),
		EXTENSION(radColorBlendEquation),
		EXTENSION(radColorMask),
		EXTENSION(radColorNumTargets),
		EXTENSION(radColorLogicOpEnable),
		EXTENSION(radColorLogicOp),
		EXTENSION(radColorAlphaToCoverageEnable),
		EXTENSION(radColorBlendColor),
		EXTENSION(radColorDynamic),
		EXTENSION(radCreateRasterState),
		EXTENSION(radReferenceRasterState),
		EXTENSION(radReleaseRasterState),
		EXTENSION(radRasterDefault),
		EXTENSION(radRasterPointSize),
		EXTENSION(radRasterLineWidth),
		EXTENSION(radRasterCullFace),
		EXTENSION(radRasterFrontFace),
		EXTENSION(radRasterPolygonMode),
		EXTENSION(radRasterPolygonOffsetClamp),
		EXTENSION(radRasterPolygonOffsetEnables),
		EXTENSION(radRasterDiscardEnable),
		EXTENSION(radRasterMultisampleEnable),
		EXTENSION(radRasterSamples),
		EXTENSION(radRasterSampleMask),
		EXTENSION(radRasterDynamic),
		EXTENSION(radCreateDepthStencilState),
		EXTENSION(radReferenceDepthStencilState),
		EXTENSION(radReleaseDepthStencilState),
		EXTENSION(radDepthStencilDefault),
		EXTENSION(radDepthStencilDepthTestEnable),
		EXTENSION(radDepthStencilDepthWriteEnable),
		EXTENSION(radDepthStencilDepthFunc),
		EXTENSION(radDepthStencilStencilTestEnable),
		EXTENSION(radDepthStencilStencilFunc),
		EXTENSION(radDepthStencilStencilOp),
		EXTENSION(radDepthStencilStencilMask),
		EXTENSION(radDepthStencilDynamic),
		EXTENSION(radCreateVertexState),
		EXTENSION(radReferenceVertexState),
		EXTENSION(radReleaseVertexState),
		EXTENSION(radVertexDefault),
		EXTENSION(radVertexAttribFormat),
		EXTENSION(radVertexAttribBinding),
		EXTENSION(radVertexBindingGroup),
		EXTENSION(radVertexAttribEnable),
		EXTENSION(radVertexBindingStride),
		EXTENSION(radCreateRtFormatState),
		EXTENSION(radReferenceRtFormatState),
		EXTENSION(radReleaseRtFormatState),
		EXTENSION(radRtFormatDefault),
		EXTENSION(radRtFormatColorFormat),
		EXTENSION(radRtFormatDepthFormat),
		EXTENSION(radRtFormatStencilFormat),
		EXTENSION(radRtFormatColorSamples),
		EXTENSION(radRtFormatDepthStencilSamples),
		EXTENSION(radCreatePipeline),
		EXTENSION(radReferencePipeline),
		EXTENSION(radReleasePipeline),
		EXTENSION(radPipelineProgramStages),
		EXTENSION(radPipelineVertexState),
		EXTENSION(radPipelineColorState),
		EXTENSION(radPipelineRasterState),
		EXTENSION(radPipelineDepthStencilState),
		EXTENSION(radPipelineRtFormatState),
		EXTENSION(radPipelinePrimitiveType),
		EXTENSION(radCompilePipeline),
		EXTENSION(radGetPipelineHandle),
		EXTENSION(radCreateCommandBuffer),
		EXTENSION(radReferenceCommandBuffer),
		EXTENSION(radReleaseCommandBuffer),
		EXTENSION(radCmdBindPipeline),
		EXTENSION(radCmdBindGroup),
		EXTENSION(radCmdDrawArrays),
		EXTENSION(radCmdDrawElements),
		EXTENSION(radCompileCommandBuffer),
		EXTENSION(radGetCommandHandle),
		EXTENSION(radCmdStencilValueMask),
		EXTENSION(radCmdStencilMask),
		EXTENSION(radCmdStencilRef),
		EXTENSION(radCmdBlendColor),
		EXTENSION(radCmdPointSize),
		EXTENSION(radCmdLineWidth),
		EXTENSION(radCmdPolygonOffsetClamp),
		EXTENSION(radCmdSampleMask),
		EXTENSION(radCreatePass),
		EXTENSION(radReferencePass),
		EXTENSION(radReleasePass),
		EXTENSION(radPassDefault),
		EXTENSION(radCompilePass),
		EXTENSION(radPassRenderTargets),
		EXTENSION(radPassPreserveEnable),
		EXTENSION(radPassDiscard),
		EXTENSION(radPassResolve),
		EXTENSION(radPassStore),
		EXTENSION(radPassClip),
		EXTENSION(radPassDependencies),
		EXTENSION(radPassTilingBoundary),
		EXTENSION(radPassTileFilterWidth),
		EXTENSION(radPassTileFootprint),
		EXTENSION(radCreateSync),
		EXTENSION(radReferenceSync),
		EXTENSION(radReleaseSync),
		EXTENSION(radQueueFenceSync),
		EXTENSION(radWaitSync),
		EXTENSION(radQueueWaitSync),

		#undef EXTENSION
    };

    for(int ext = 0; ext < sizeof(glExtensions) / sizeof(Extension); ext++)
    {
        if(strcmp(procname, glExtensions[ext].name) == 0)
        {
            return (RADPROC)glExtensions[ext].address;
        }
    }

    return NULL;
}

void GL_APIENTRY Register(const char *licenseKey)
{
	RegisterLicenseKey(licenseKey);
}

}
