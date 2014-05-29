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

// Texture.h: Defines the abstract Texture class and its concrete derived
// classes Texture2D and TextureCubeMap. Implements GL texture objects and
// related functionality. [OpenGL ES 2.0.24] section 3.7 page 63.

#ifndef LIBGLESV2_TEXTURE_H_
#define LIBGLESV2_TEXTURE_H_

#include "Renderbuffer.h"
#include "RefCountObject.h"
#include "utilities.h"
#include "common/debug.h"

#define GL_APICALL
#include <GLES2/gl2.h>

#include <vector>

namespace egl
{
class Surface;
class Config;
}

namespace gl
{
class Framebuffer;

enum
{
	IMPLEMENTATION_MAX_TEXTURE_LEVELS = MIPMAP_LEVELS,
    IMPLEMENTATION_MAX_TEXTURE_SIZE = 1 << (MIPMAP_LEVELS - 1),
    IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE = 1 << (MIPMAP_LEVELS - 1),
	IMPLEMENTATION_MAX_RENDERBUFFER_SIZE = OUTLINE_RESOLUTION,
	IMPLEMENTATION_MAX_SAMPLES = 4
};

class Texture : public RefCountObject
{
public:
    explicit Texture(GLuint id);

    virtual ~Texture();

	sw::Resource *getResource() const;

	virtual void addProxyRef(const Renderbuffer *proxy) = 0;
    virtual void releaseProxy(const Renderbuffer *proxy) = 0;

    virtual GLenum getTarget() const = 0;

    bool setMinFilter(GLenum filter);
    bool setMagFilter(GLenum filter);
    bool setWrapS(GLenum wrap);
    bool setWrapT(GLenum wrap);
	bool setMaxAnisotropy(GLfloat textureMaxAnisotropy);

    GLenum getMinFilter() const;
    GLenum getMagFilter() const;
    GLenum getWrapS() const;
    GLenum getWrapT() const;
	GLfloat getMaxAnisotropy() const;

    virtual GLsizei getWidth(GLenum target, GLint level) const = 0;
    virtual GLsizei getHeight(GLenum target, GLint level) const = 0;
    virtual GLenum getFormat(GLenum target, GLint level) const = 0;
    virtual GLenum getType(GLenum target, GLint level) const = 0;
    virtual sw::Format getInternalFormat(GLenum target, GLint level) const = 0;
	virtual int getLevelCount() const = 0;

    virtual bool isSamplerComplete() const = 0;
    virtual bool isCompressed(GLenum target, GLint level) const = 0;
	virtual bool isDepth(GLenum target, GLint level) const = 0;

    virtual Renderbuffer *getRenderbuffer(GLenum target) = 0;
    virtual Image *getRenderTarget(GLenum target, unsigned int level) = 0;
    virtual Image *createSharedImage(GLenum target, unsigned int level);
    virtual bool isShared(GLenum target, unsigned int level) const = 0;

    virtual void generateMipmaps() = 0;
    virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source) = 0;

protected:
    void setImage(GLenum format, GLenum type, GLint unpackAlignment, const void *pixels, Image *image);
    void subImage(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels, Image *image);
    void setCompressedImage(GLsizei imageSize, const void *pixels, Image *image);
    void subImageCompressed(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels, Image *image);

	bool copy(Image *source, const sw::Rect &sourceRect, GLenum destFormat, GLint xoffset, GLint yoffset, Image *dest);

	bool isMipmapFiltered() const;

    GLenum mMinFilter;
    GLenum mMagFilter;
    GLenum mWrapS;
    GLenum mWrapT;
	GLfloat mMaxAnisotropy;

	sw::Resource *resource;
};

class Texture2D : public Texture
{
public:
    explicit Texture2D(GLuint id);

    virtual ~Texture2D();

	void addProxyRef(const Renderbuffer *proxy);
    void releaseProxy(const Renderbuffer *proxy);

    virtual GLenum getTarget() const;

    virtual GLsizei getWidth(GLenum target, GLint level) const;
    virtual GLsizei getHeight(GLenum target, GLint level) const;
    virtual GLenum getFormat(GLenum target, GLint level) const;
    virtual GLenum getType(GLenum target, GLint level) const;
    virtual sw::Format getInternalFormat(GLenum target, GLint level) const;
	virtual int getLevelCount() const;

    void setImage(GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);
    void subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels);
    void copyImage(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);
    void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

    virtual bool isSamplerComplete() const;
    virtual bool isCompressed(GLenum target, GLint level) const;
	virtual bool isDepth(GLenum target, GLint level) const;
    virtual void bindTexImage(egl::Surface *surface);
    virtual void releaseTexImage();

    virtual void generateMipmaps();

	virtual Renderbuffer *getRenderbuffer(GLenum target);
    virtual Image *getRenderTarget(GLenum target, unsigned int level);
	virtual bool isShared(GLenum target, unsigned int level) const;

    Image *getImage(unsigned int level);

protected:
	bool isMipmapComplete() const;

	Image *image[IMPLEMENTATION_MAX_TEXTURE_LEVELS];
    
    egl::Surface *mSurface;
    
	// A specific internal reference count is kept for colorbuffer proxy references,
    // because, as the renderbuffer acting as proxy will maintain a binding pointer
    // back to this texture, there would be a circular reference if we used a binding
    // pointer here. This reference count will cause the pointer to be set to NULL if
    // the count drops to zero, but will not cause deletion of the Renderbuffer.
    Renderbuffer *mColorbufferProxy;
    unsigned int mProxyRefs;
};

class TextureCubeMap : public Texture
{
public:
    explicit TextureCubeMap(GLuint id);

    virtual ~TextureCubeMap();

	void addProxyRef(const Renderbuffer *proxy);
    void releaseProxy(const Renderbuffer *proxy);

    virtual GLenum getTarget() const;
    
    virtual GLsizei getWidth(GLenum target, GLint level) const;
    virtual GLsizei getHeight(GLenum target, GLint level) const;
    virtual GLenum getFormat(GLenum target, GLint level) const;
    virtual GLenum getType(GLenum target, GLint level) const;
    virtual sw::Format getInternalFormat(GLenum target, GLint level) const;
	virtual int getLevelCount() const;

	void setImage(GLenum target, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void setCompressedImage(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);

    void subImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void subImageCompressed(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels);
    void copyImage(GLenum target, GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);
    virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

    virtual bool isSamplerComplete() const;
    virtual bool isCompressed(GLenum target, GLint level) const;
	virtual bool isDepth(GLenum target, GLint level) const;

    virtual void generateMipmaps();

    virtual Renderbuffer *getRenderbuffer(GLenum target);
	virtual Image *getRenderTarget(GLenum target, unsigned int level);
	virtual bool isShared(GLenum target, unsigned int level) const;

	Image *getImage(int face, unsigned int level);

private:
	bool isCubeComplete() const;
	bool isMipmapCubeComplete() const;

    // face is one of the GL_TEXTURE_CUBE_MAP_* enumerants. Returns NULL on failure.
    Image *getImage(GLenum face, unsigned int level);

    Image *image[6][IMPLEMENTATION_MAX_TEXTURE_LEVELS];
	
	// A specific internal reference count is kept for colorbuffer proxy references,
    // because, as the renderbuffer acting as proxy will maintain a binding pointer
    // back to this texture, there would be a circular reference if we used a binding
    // pointer here. This reference count will cause the pointer to be set to NULL if
    // the count drops to zero, but will not cause deletion of the Renderbuffer.
    Renderbuffer *mFaceProxies[6];
	unsigned int mFaceProxyRefs[6];
};

class TextureExternal : public Texture2D
{
public:
    explicit TextureExternal(GLuint id);

    virtual ~TextureExternal();

    virtual GLenum getTarget() const;

    void setImage(Image *image);
};
}

#endif   // LIBGLESV2_TEXTURE_H_
