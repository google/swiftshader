// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Image.hpp"

#include "../common/debug.h"

namespace gl
{
	Image::Image(sw::Resource *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type) : sw::Surface(parentTexture, width, height, 1, selectInternalFormat(format, type), true, true), width(width), height(height), internalFormat(selectInternalFormat(format, type)), format(format), type(type), multiSampleDepth(1)
	{
	}

	Image::Image(sw::Resource *parentTexture, GLsizei width, GLsizei height, sw::Format internalFormat, GLenum format, GLenum type, int multiSampleDepth, bool lockable, bool renderTarget) : sw::Surface(parentTexture, width, height, multiSampleDepth, internalFormat, lockable, renderTarget), width(width), height(height), internalFormat(internalFormat), format(format), type(type), multiSampleDepth(multiSampleDepth)
	{
	}

	Image::~Image()
	{
	}

	void *Image::lock(unsigned int left, unsigned int top, sw::Lock lock)
	{
		return lockExternal(left, top, 0, lock, sw::PUBLIC);
	}

	unsigned int Image::getPitch()
	{
		return getExternalPitchB();
	}

	void Image::unlock()
	{
		unlockExternal();
	}

	int Image::getWidth()
	{
		return width;
	}
	
	int Image::getHeight()
	{
		return height;
	}

	GLenum Image::getFormat()
	{
		return format;
	}
	
	GLenum Image::getType()
	{
		return type;
	}
	
	sw::Format Image::getInternalFormat()
	{
		return internalFormat;
	}
	
	int Image::getMultiSampleDepth()
	{
		return multiSampleDepth;
	}

	int Image::bytes(sw::Format format)
	{
		return sw::Surface::bytes(format);
	}
}