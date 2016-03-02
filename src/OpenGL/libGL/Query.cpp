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

// Query.cpp: Implements the gl::Query class

#include "Query.h"

#include "main.h"
#include "Common/Thread.hpp"

namespace gl
{

Query::Query(GLuint name, GLenum type) : NamedObject(name)
{ 
    mQuery = NULL;
    mStatus = GL_FALSE;
    mResult = GL_FALSE;
    mType = type;
}

Query::~Query()
{
    if(mQuery != NULL)
    {
        delete mQuery;
    }
}

void Query::begin()
{
    if(mQuery == NULL)
    {
		sw::Query::Type type;
		switch(mType)
		{
		case GL_ANY_SAMPLES_PASSED:
		case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
			type = sw::Query::FRAGMENTS_PASSED;
			break;
		default:
			ASSERT(false);
		}

		mQuery = new sw::Query(type);

		if(!mQuery)
        {
            return error(GL_OUT_OF_MEMORY);
        }
    }

	Device *device = getDevice();

	mQuery->begin();
	device->addQuery(mQuery);
	device->setOcclusionEnabled(true);
}

void Query::end()
{
    if(mQuery == NULL)
    {
        return error(GL_INVALID_OPERATION);
	}

	Device *device = getDevice();

    mQuery->end();
	device->removeQuery(mQuery);
	device->setOcclusionEnabled(false);
    
    mStatus = GL_FALSE;
    mResult = GL_FALSE;
}

GLuint Query::getResult()
{
    if(mQuery != NULL)
    {
        while(!testQuery())
        {
            sw::Thread::yield();
        }
    }

    return (GLuint)mResult;
}

GLboolean Query::isResultAvailable()
{
    if(mQuery != NULL)
    {
        testQuery();
    }
    
    return mStatus;
}

GLenum Query::getType() const
{
    return mType;
}

GLboolean Query::testQuery()
{
    if(mQuery != NULL && mStatus != GL_TRUE)
    {
        if(!mQuery->building && mQuery->reference == 0)
        {
			unsigned int numPixels = mQuery->data;
            mStatus = GL_TRUE;

            switch(mType)
            {
            case GL_ANY_SAMPLES_PASSED:
            case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
                mResult = (numPixels > 0) ? GL_TRUE : GL_FALSE;
                break;
            default:
                ASSERT(false);
            }
        }
        
        return mStatus;
    }

    return GL_TRUE;   // Prevent blocking when query is null
}
}
