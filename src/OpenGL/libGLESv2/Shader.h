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

// Shader.h: Defines the abstract Shader class and its concrete derived
// classes VertexShader and FragmentShader. Implements GL shader objects and
// related functionality. [OpenGL ES 2.0.24] section 2.10 page 24 and section
// 3.8 page 84.

#ifndef LIBGLESV2_SHADER_H_
#define LIBGLESV2_SHADER_H_

#include "ResourceManager.h"

#include "compiler/TranslatorASM.h"

#include <GLES2/gl2.h>

#include <string>
#include <list>
#include <vector>

namespace glsl
{
	class OutputASM;
}

namespace es2
{

class Shader : public glsl::Shader
{
    friend class Program;

public:
    Shader(ResourceManager *manager, GLuint handle);

    virtual ~Shader();

    virtual GLenum getType() const = 0;
    GLuint getName() const;

    void deleteSource();
    void setSource(GLsizei count, const char *const *string, const GLint *length);
    size_t getInfoLogLength() const;
    void getInfoLog(GLsizei bufSize, GLsizei *length, char *infoLog);
    size_t getSourceLength() const;
    void getSource(GLsizei bufSize, GLsizei *length, char *source);

    void compile();
    bool isCompiled();
    
    void addRef();
    void release();
    unsigned int getRefCount() const;
    bool isFlaggedForDeletion() const;
    void flagForDeletion();

    static void releaseCompiler();

protected:
	static bool compilerInitialized;
	TranslatorASM *createCompiler(GLenum shaderType);
	void clear();

    static bool compareVarying(const glsl::Varying &x, const glsl::Varying &y);

	char *mSource;
	std::string infoLog;

private:
	virtual void createShader() = 0;
	virtual void deleteShader() = 0;

	const GLuint mHandle;
    unsigned int mRefCount;     // Number of program objects this shader is attached to
    bool mDeleteStatus;         // Flag to indicate that the shader can be deleted when no longer in use

	ResourceManager *mResourceManager;
};

class VertexShader : public Shader
{
    friend class Program;

public:
    VertexShader(ResourceManager *manager, GLuint handle);

    ~VertexShader();

    virtual GLenum getType() const;
    int getSemanticIndex(const std::string &attributeName);

	virtual sw::Shader *getShader() const;
	virtual sw::VertexShader *getVertexShader() const;

private:
	virtual void createShader();
	virtual void deleteShader();

	sw::VertexShader *vertexShader;
};

class FragmentShader : public Shader
{
public:
    FragmentShader(ResourceManager *manager, GLuint handle);

    ~FragmentShader();

    virtual GLenum getType() const;

	virtual sw::Shader *getShader() const;
	virtual sw::PixelShader *getPixelShader() const;

private:
	virtual void createShader();
	virtual void deleteShader();

	sw::PixelShader *pixelShader;
};
}

#endif   // LIBGLESV2_SHADER_H_
