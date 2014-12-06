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

#ifndef gl_Image_hpp
#define gl_Image_hpp

#include "Renderer/Surface.hpp"
#include "ImageEGL.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace es2
{
	class Texture;

	class Image : public egl::Image
	{
	public:
		Image(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type);
		Image(Texture *parentTexture, GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable, bool renderTarget);

		void loadImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *input);
		void loadCompressedData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);

		virtual void addRef();
		virtual void release();
		void unbind();   // Break parent ownership and release

		static sw::Format selectInternalFormat(GLenum format, GLenum type);

	private:
		virtual ~Image();

		void loadAlphaImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadAlphaFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadAlphaHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadLuminanceImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadLuminanceFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadLuminanceHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadLuminanceAlphaImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadLuminanceAlphaFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadLuminanceAlphaHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBUByteImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGB565ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBAUByteImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBA4444ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBA5551ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBAFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadRGBAHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadBGRAImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadD16ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadD32ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer) const;
		void loadD24S8ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, int inputPitch, const void *input, void *buffer);

		Texture *parentTexture;

		volatile int referenceCount;
	};
}

#endif   // gl_Image_hpp
