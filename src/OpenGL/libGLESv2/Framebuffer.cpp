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

// Framebuffer.cpp: Implements the Framebuffer class. Implements GL framebuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4 page 105.

#include "Framebuffer.h"

#include "main.h"
#include "Renderbuffer.h"
#include "Texture.h"
#include "utilities.h"

namespace es2
{

Framebuffer::Framebuffer()
{
	mColorbufferType = GL_NONE;
	mDepthbufferType = GL_NONE;
	mStencilbufferType = GL_NONE;
}

Framebuffer::~Framebuffer()
{
	mColorbufferPointer = NULL;
	mDepthbufferPointer = NULL;
	mStencilbufferPointer = NULL;
}

Renderbuffer *Framebuffer::lookupRenderbuffer(GLenum type, GLuint handle) const
{
	Context *context = getContext();
	Renderbuffer *buffer = NULL;

	if(type == GL_NONE)
	{
		buffer = NULL;
	}
	else if(type == GL_RENDERBUFFER)
	{
		buffer = context->getRenderbuffer(handle);
	}
	else if(IsTextureTarget(type))
	{
		buffer = context->getTexture(handle)->getRenderbuffer(type);
	}
	else
	{
		UNREACHABLE();
	}

	return buffer;
}

void Framebuffer::setColorbuffer(GLenum type, GLuint colorbuffer)
{
	mColorbufferType = (colorbuffer != 0) ? type : GL_NONE;
	mColorbufferPointer = lookupRenderbuffer(type, colorbuffer);
}

void Framebuffer::setDepthbuffer(GLenum type, GLuint depthbuffer)
{
	mDepthbufferType = (depthbuffer != 0) ? type : GL_NONE;
	mDepthbufferPointer = lookupRenderbuffer(type, depthbuffer);
}

void Framebuffer::setStencilbuffer(GLenum type, GLuint stencilbuffer)
{
	mStencilbufferType = (stencilbuffer != 0) ? type : GL_NONE;
	mStencilbufferPointer = lookupRenderbuffer(type, stencilbuffer);
}

void Framebuffer::detachTexture(GLuint texture)
{
	if(mColorbufferPointer.name() == texture && IsTextureTarget(mColorbufferType))
	{
		mColorbufferType = GL_NONE;
		mColorbufferPointer = NULL;
	}

	if(mDepthbufferPointer.name() == texture && IsTextureTarget(mDepthbufferType))
	{
		mDepthbufferType = GL_NONE;
		mDepthbufferPointer = NULL;
	}

	if(mStencilbufferPointer.name() == texture && IsTextureTarget(mStencilbufferType))
	{
		mStencilbufferType = GL_NONE;
		mStencilbufferPointer = NULL;
	}
}

void Framebuffer::detachRenderbuffer(GLuint renderbuffer)
{
	if(mColorbufferPointer.name() == renderbuffer && mColorbufferType == GL_RENDERBUFFER)
	{
		mColorbufferType = GL_NONE;
		mColorbufferPointer = NULL;
	}

	if(mDepthbufferPointer.name() == renderbuffer && mDepthbufferType == GL_RENDERBUFFER)
	{
		mDepthbufferType = GL_NONE;
		mDepthbufferPointer = NULL;
	}

	if(mStencilbufferPointer.name() == renderbuffer && mStencilbufferType == GL_RENDERBUFFER)
	{
		mStencilbufferType = GL_NONE;
		mStencilbufferPointer = NULL;
	}
}

// Increments refcount on surface.
// caller must Release() the returned surface
egl::Image *Framebuffer::getRenderTarget()
{
	Renderbuffer *colorbuffer = mColorbufferPointer;

	if(colorbuffer)
	{
		return colorbuffer->getRenderTarget();
	}

	return NULL;
}

// Increments refcount on surface.
// caller must Release() the returned surface
egl::Image *Framebuffer::getDepthStencil()
{
	Renderbuffer *depthstencilbuffer = mDepthbufferPointer;
	
	if(!depthstencilbuffer)
	{
		depthstencilbuffer = mStencilbufferPointer;
	}

	if(depthstencilbuffer)
	{
		return depthstencilbuffer->getRenderTarget();
	}

	return NULL;
}

Renderbuffer *Framebuffer::getColorbuffer()
{
	return mColorbufferPointer;
}

Renderbuffer *Framebuffer::getDepthbuffer()
{
	return mDepthbufferPointer;
}

Renderbuffer *Framebuffer::getStencilbuffer()
{
	return mStencilbufferPointer;
}

GLenum Framebuffer::getColorbufferType()
{
	return mColorbufferType;
}

GLenum Framebuffer::getDepthbufferType()
{
	return mDepthbufferType;
}

GLenum Framebuffer::getStencilbufferType()
{
	return mStencilbufferType;
}

GLuint Framebuffer::getColorbufferName()
{
	return mColorbufferPointer.name();
}

GLuint Framebuffer::getDepthbufferName()
{
	return mDepthbufferPointer.name();
}

GLuint Framebuffer::getStencilbufferName()
{
	return mStencilbufferPointer.name();
}

bool Framebuffer::hasStencil()
{
	if(mStencilbufferType != GL_NONE)
	{
		Renderbuffer *stencilbufferObject = getStencilbuffer();

		if(stencilbufferObject)
		{
			return stencilbufferObject->getStencilSize() > 0;
		}
	}

	return false;
}

GLenum Framebuffer::completeness()
{
	int width;
	int height;
	int samples;

	return completeness(width, height, samples);
}

GLenum Framebuffer::completeness(int &width, int &height, int &samples)
{
	width = -1;
	height = -1;
	samples = -1;

	if(mColorbufferType != GL_NONE)
	{
		Renderbuffer *colorbuffer = getColorbuffer();

		if(!colorbuffer)
		{
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(colorbuffer->getWidth() == 0 || colorbuffer->getHeight() == 0)
		{
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(mColorbufferType == GL_RENDERBUFFER)
		{
			if(!es2::IsColorRenderable(colorbuffer->getFormat()))
			{
				return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
			}
		}
		else if(IsTextureTarget(mColorbufferType))
		{
			GLenum format = colorbuffer->getFormat();

			if(IsCompressed(format) ||
			   format == GL_ALPHA ||
			   format == GL_LUMINANCE ||
			   format == GL_LUMINANCE_ALPHA)
			{
				return GL_FRAMEBUFFER_UNSUPPORTED;
			}

			if(es2::IsDepthTexture(format) || es2::IsStencilTexture(format))
			{
				return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
			}
		}
		else
		{
			UNREACHABLE();
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		width = colorbuffer->getWidth();
		height = colorbuffer->getHeight();
		samples = colorbuffer->getSamples();
	}

	Renderbuffer *depthbuffer = NULL;
	Renderbuffer *stencilbuffer = NULL;

	if(mDepthbufferType != GL_NONE)
	{
		depthbuffer = getDepthbuffer();

		if(!depthbuffer)
		{
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(depthbuffer->getWidth() == 0 || depthbuffer->getHeight() == 0)
		{
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(mDepthbufferType == GL_RENDERBUFFER)
		{
			if(!es2::IsDepthRenderable(depthbuffer->getFormat()))
			{
				return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
			}
		}
		else if(IsTextureTarget(mDepthbufferType))
		{
			if(!es2::IsDepthTexture(depthbuffer->getFormat()))
			{
				return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
			}
		}
		else
		{
			UNREACHABLE();
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(width == -1 || height == -1)
		{
			width = depthbuffer->getWidth();
			height = depthbuffer->getHeight();
			samples = depthbuffer->getSamples();
		}
		else if(width != depthbuffer->getWidth() || height != depthbuffer->getHeight())
		{
			return GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS;
		}
		else if(samples != depthbuffer->getSamples())
		{
			return GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_ANGLE;
		}
	}

	if(mStencilbufferType != GL_NONE)
	{
		stencilbuffer = getStencilbuffer();

		if(!stencilbuffer)
		{
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(stencilbuffer->getWidth() == 0 || stencilbuffer->getHeight() == 0)
		{
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(mStencilbufferType == GL_RENDERBUFFER)
		{
			if(!es2::IsStencilRenderable(stencilbuffer->getFormat()))
			{
				return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
			}
		}
		else if(IsTextureTarget(mStencilbufferType))
		{
			GLenum internalformat = stencilbuffer->getFormat();

			if(!es2::IsStencilTexture(internalformat))
			{
				return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
			}
		}
		else
		{
			UNREACHABLE();
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		}

		if(width == -1 || height == -1)
		{
			width = stencilbuffer->getWidth();
			height = stencilbuffer->getHeight();
			samples = stencilbuffer->getSamples();
		}
		else if(width != stencilbuffer->getWidth() || height != stencilbuffer->getHeight())
		{
			return GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS;
		}
		else if(samples != stencilbuffer->getSamples())
		{
			return GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_ANGLE;
		}
	}

	// If we have both a depth and stencil buffer, they must refer to the same object
	// since we only support packed_depth_stencil and not separate depth and stencil
	if(depthbuffer && stencilbuffer && (depthbuffer != stencilbuffer))
	{
		return GL_FRAMEBUFFER_UNSUPPORTED;
	}

	// We need to have at least one attachment to be complete
	if(width == -1 || height == -1)
	{
		return GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
	}

	return GL_FRAMEBUFFER_COMPLETE;
}

GLenum Framebuffer::getImplementationColorReadFormat()
{
	Renderbuffer *colorbuffer = mColorbufferPointer;
	
	if(colorbuffer)
	{
		switch(colorbuffer->getInternalFormat())
		{
		case sw::FORMAT_A16B16G16R16F: return GL_RGBA;
		case sw::FORMAT_A32B32G32R32F: return GL_RGBA;
		case sw::FORMAT_A8R8G8B8:      return GL_BGRA_EXT;
		case sw::FORMAT_X8R8G8B8:      return GL_BGRA_EXT;
		case sw::FORMAT_A1R5G5B5:      return GL_BGRA_EXT;
		case sw::FORMAT_R5G6B5:        return 0x80E0;   // GL_BGR_EXT
		default:
			UNREACHABLE();
		}
	}

	return GL_RGBA;
}

GLenum Framebuffer::getImplementationColorReadType()
{
	Renderbuffer *colorbuffer = mColorbufferPointer;
	
	if(colorbuffer)
	{
		switch(colorbuffer->getInternalFormat())
		{
		case sw::FORMAT_A16B16G16R16F: return GL_HALF_FLOAT_OES;
		case sw::FORMAT_A32B32G32R32F: return GL_FLOAT;
		case sw::FORMAT_A8R8G8B8:      return GL_UNSIGNED_BYTE;
		case sw::FORMAT_X8R8G8B8:      return GL_UNSIGNED_BYTE;
		case sw::FORMAT_A1R5G5B5:      return GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT;
		case sw::FORMAT_R5G6B5:        return GL_UNSIGNED_SHORT_5_6_5;
		default:
			UNREACHABLE();
		}
	}

	return GL_UNSIGNED_BYTE;
}

DefaultFramebuffer::DefaultFramebuffer(Colorbuffer *colorbuffer, DepthStencilbuffer *depthStencil)
{
	mColorbufferPointer = new Renderbuffer(0, colorbuffer);

	Renderbuffer *depthStencilRenderbuffer = new Renderbuffer(0, depthStencil);
	mDepthbufferPointer = depthStencilRenderbuffer;
	mStencilbufferPointer = depthStencilRenderbuffer;

	mColorbufferType = GL_RENDERBUFFER;
	mDepthbufferType = (depthStencilRenderbuffer->getDepthSize() != 0) ? GL_RENDERBUFFER : GL_NONE;
	mStencilbufferType = (depthStencilRenderbuffer->getStencilSize() != 0) ? GL_RENDERBUFFER : GL_NONE;
}

GLenum DefaultFramebuffer::completeness()
{
	// The default framebuffer should always be complete
	ASSERT(Framebuffer::completeness() == GL_FRAMEBUFFER_COMPLETE);

	return GL_FRAMEBUFFER_COMPLETE;
}

}
