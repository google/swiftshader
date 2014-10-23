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

#define GL_APICALL
#include <GLES2/gl2.h>

#include <list>
#include <vector>

namespace sh
{
	class OutputASM;
}

namespace gl
{
struct Varying
{
    Varying(GLenum type, const std::string &name, int arraySize, int reg = -1, int col = -1)
        : type(type), name(name), arraySize(arraySize), reg(reg), col(col)
    {
    }

	bool isArray() const
	{
		return arraySize >= 1;
	}

	int size() const   // Unify with gl::Uniform?
	{
		return arraySize > 0 ? arraySize : 1;
	}

    GLenum type;
    std::string name;
    int arraySize;

    int reg;    // First varying register, assigned during link
    int col;    // First register element, assigned during link
};

typedef std::list<Varying> VaryingList;

class Shader
{
    friend class Program;
	friend class sh::OutputASM;

public:
    Shader(ResourceManager *manager, GLuint handle);

    virtual ~Shader();

    virtual GLenum getType() = 0;
    GLuint getHandle() const;

    void deleteSource();
    void setSource(GLsizei count, const char **string, const GLint *length);
    int getInfoLogLength() const;
    void getInfoLog(GLsizei bufSize, GLsizei *length, char *infoLog);
    int getSourceLength() const;
    void getSource(GLsizei bufSize, GLsizei *length, char *source);

    virtual void compile() = 0;
    bool isCompiled();
    
	virtual sw::Shader *getShader() const = 0;
	virtual sw::PixelShader *getPixelShader() const;
	virtual sw::VertexShader *getVertexShader() const;

    void addRef();
    void release();
    unsigned int getRefCount() const;
    bool isFlaggedForDeletion() const;
    void flagForDeletion();

    static void releaseCompiler();

protected:
	TranslatorASM *createCompiler(ShShaderType type);
	void clear();

    static GLenum parseType(const std::string &type);
    static bool compareVarying(const Varying &x, const Varying &y);

	char *mSource;
	char *mInfoLog;

    VaryingList varyings;
	sh::ActiveUniforms activeUniforms;
	sh::ActiveAttributes activeAttributes;

private:
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

    virtual GLenum getType();
    virtual void compile();
    int getSemanticIndex(const std::string &attributeName);

	virtual sw::Shader *getShader() const;
	virtual sw::VertexShader *getVertexShader() const;

private:
	sw::VertexShader *vertexShader;
};

class FragmentShader : public Shader
{
public:
    FragmentShader(ResourceManager *manager, GLuint handle);

    ~FragmentShader();

    virtual GLenum getType();
    virtual void compile();

	virtual sw::Shader *getShader() const;
	virtual sw::PixelShader *getPixelShader() const;

private:
	sw::PixelShader *pixelShader;
};
}

#endif   // LIBGLESV2_SHADER_H_
