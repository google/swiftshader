// SwiftShader Software Renderer
//
// Copyright(c) 2015 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of Google Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

// TransformFeedback.h: Defines the es2::TransformFeedback class

#ifndef LIBGLESV2_TRANSFORM_FEEDBACK_H_
#define LIBGLESV2_TRANSFORM_FEEDBACK_H_

#include "Buffer.h"
#include "Context.h"
#include "common/Object.hpp"
#include "Renderer/Renderer.hpp"

#include <GLES2/gl2.h>

namespace es2
{

class TransformFeedback : public gl::NamedObject
{
public:
	// FIXME: Change this when implementing transform feedback
	TransformFeedback(GLuint name);
	~TransformFeedback();

	Buffer* getGenericBuffer() const;
	Buffer* getBuffer(GLuint index) const;
	GLuint getGenericBufferName() const;
	GLuint getBufferName(GLuint index) const;
	int getOffset(GLuint index) const;
	int getSize(GLuint index) const;
	bool isActive() const;
	bool isPaused() const;
	GLenum primitiveMode() const;

	void setGenericBuffer(Buffer* buffer);
	void setBuffer(GLuint index, Buffer* buffer);
	void setBuffer(GLuint index, Buffer* buffer, GLintptr offset, GLsizeiptr size);
	void detachBuffer(GLuint buffer);
	void begin(GLenum primitiveMode);
	void end();
	void setPaused(bool paused);

private:
	gl::BindingPointer<Buffer> mGenericBuffer;
	BufferBinding mBuffer[MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS];

	bool mActive;
	bool mPaused;
	GLenum mPrimitiveMode;
};

}

#endif // LIBGLESV2_TRANSFORM_FEEDBACK_H_
