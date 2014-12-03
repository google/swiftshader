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

// VertexDataManager.h: Defines the VertexDataManager, a class that
// runs the Buffer translation process.

#include "VertexDataManager.h"

#include "Buffer.h"
#include "Program.h"
#include "IndexDataManager.h"
#include "common/debug.h"

namespace es2
{

VertexDataManager::VertexDataManager(Context *context) : mContext(context)
{
}

VertexDataManager::~VertexDataManager()
{
}

GLenum VertexDataManager::prepareVertexData(GLint start, GLsizei count, TranslatedAttribute *translated)
{
    const VertexAttributeArray &attribs = mContext->getVertexAttributes();
    Program *program = mContext->getCurrentProgram();
    
    // Perform the vertex data translations
    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        if(program->getAttributeStream(i) != -1)
        {
            if(attribs[i].mArrayEnabled)
            {
				translated[i].vertexBuffer = attribs[i].buffer;
				translated[i].offset = start * attribs[i].stride() + attribs[i].mOffset;
				translated[i].stride = attribs[i].stride();
               
				switch(attribs[i].mType)
				{
				case GL_BYTE:           translated[i].type = sw::STREAMTYPE_SBYTE;  break;
				case GL_UNSIGNED_BYTE:  translated[i].type = sw::STREAMTYPE_BYTE;   break;
				case GL_SHORT:          translated[i].type = sw::STREAMTYPE_SHORT;  break;
				case GL_UNSIGNED_SHORT: translated[i].type = sw::STREAMTYPE_USHORT; break;
				case GL_FIXED:          translated[i].type = sw::STREAMTYPE_FIXED;  break;
				case GL_FLOAT:          translated[i].type = sw::STREAMTYPE_FLOAT;  break;
				default: UNREACHABLE(); translated[i].type = sw::STREAMTYPE_FLOAT;  break;
				}

				translated[i].count = attribs[i].mSize;
				translated[i].normalized = attribs[i].mNormalized;
            }
            else
            {
                translated[i].vertexBuffer = 0;
                translated[i].type = sw::STREAMTYPE_FLOAT;
				translated[i].count = 4;
                translated[i].stride = 0;
                translated[i].offset = 0;
            }
        }
    }

    return GL_NO_ERROR;
}

}
