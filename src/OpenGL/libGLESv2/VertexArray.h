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

#include "common/Object.hpp"
#include "Renderer/Renderer.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace es2
{

class VertexArray : public gl::Object
{
};

}

#endif // LIBGLESV2_VERTEX_ARRAY_H_
