// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

// Query.h: Defines the es2::Query class

#ifndef LIBGLESV2_QUERY_H_
#define LIBGLESV2_QUERY_H_

#include "RefCountObject.h"
#include "Renderer/Renderer.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace es2
{

class Query : public RefCountObject
{
  public:
    Query(GLuint id, GLenum type);
    virtual ~Query();

    void begin();
    void end();
    GLuint getResult();
    GLboolean isResultAvailable();

    GLenum getType() const;

  private:
    GLboolean testQuery();

    sw::Query* mQuery;
    GLenum mType;
    GLboolean mStatus;
    GLint mResult;
};

}

#endif   // LIBGLESV2_QUERY_H_
