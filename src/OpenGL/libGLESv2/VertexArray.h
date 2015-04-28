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

// VertexArray.h: Defines the es2::VertexArray class

#ifndef LIBGLESV2_VERTEX_ARRAY_H_
#define LIBGLESV2_VERTEX_ARRAY_H_

#include "Buffer.h"
#include "Context.h"

namespace es2
{

class VertexArray : public gl::NamedObject
{
public:
	VertexArray(GLuint name);
	~VertexArray();

	const VertexAttribute& getVertexAttribute(size_t attributeIndex) const;
	VertexAttributeArray& getVertexAttributes() { return mVertexAttributes; }

	void detachBuffer(GLuint bufferName);
	void setVertexAttribDivisor(GLuint index, GLuint divisor);
	void enableAttribute(unsigned int attributeIndex, bool enabledState);
	void setAttributeState(unsigned int attributeIndex, Buffer *boundBuffer, GLint size, GLenum type,
	                       bool normalized, GLsizei stride, const void *pointer);

	Buffer *getElementArrayBuffer() const { return mElementArrayBuffer; }
	void setElementArrayBuffer(Buffer *buffer);

private:
	VertexAttributeArray mVertexAttributes;
	gl::BindingPointer<Buffer> mElementArrayBuffer;
};

}

#endif // LIBGLESV2_VERTEX_ARRAY_H_
