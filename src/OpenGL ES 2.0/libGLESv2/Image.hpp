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

#ifndef dx_Surface_hpp
#define dx_Surface_hpp

#include "Unknown.hpp"

#include "Renderer/Surface.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace gl
{
	class Image : public Unknown, public sw::Surface
	{
	public:
		Image(sw::Resource *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type);
		Image(sw::Resource *parentTexture, GLsizei width, GLsizei height, sw::Format internalFormat, GLenum format, GLenum type, int multiSampleDepth, bool lockable, bool renderTarget);

		virtual ~Image();

		virtual void *lock(unsigned int left, unsigned int top, sw::Lock lock);
		virtual unsigned int getPitch();
		virtual void unlock();
		
		virtual int getWidth();
		virtual int getHeight();
		virtual GLenum getFormat();
		virtual GLenum getType();
		virtual sw::Format getInternalFormat();
		virtual int getMultiSampleDepth();

		static sw::Format selectInternalFormat(GLenum format, GLenum type);
		static int bytes(sw::Format format);

	private:
		const GLsizei width;
		const GLsizei height;
		const GLenum format;
		const GLenum type;
		const sw::Format internalFormat;
		const int multiSampleDepth;
	};
}

#endif   // dx_Surface_hpp
