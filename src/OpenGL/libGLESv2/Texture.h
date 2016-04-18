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
#include "common/Object.hpp"
#include "utilities.h"
#include "libEGL/Texture.hpp"
#include "common/debug.h"

#include <GLES2/gl2.h>

#include <vector>

namespace egl
{
class Surface;
class Config;
}

namespace es2
{
class Framebuffer;

enum
{
	IMPLEMENTATION_MAX_TEXTURE_LEVELS = sw::MIPMAP_LEVELS,
    IMPLEMENTATION_MAX_TEXTURE_SIZE = 1 << (IMPLEMENTATION_MAX_TEXTURE_LEVELS - 1),
    IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE = 1 << (IMPLEMENTATION_MAX_TEXTURE_LEVELS - 1),
	IMPLEMENTATION_MAX_RENDERBUFFER_SIZE = sw::OUTLINE_RESOLUTION,
};

class Texture : public egl::Texture
{
public:
    explicit Texture(GLuint name);

	virtual sw::Resource *getResource() const;

	virtual void addProxyRef(const Renderbuffer *proxy) = 0;
    virtual void releaseProxy(const Renderbuffer *proxy) = 0;

    virtual GLenum getTarget() const = 0;

    bool setMinFilter(GLenum filter);
    bool setMagFilter(GLenum filter);
    bool setWrapS(GLenum wrap);
	bool setWrapT(GLenum wrap);
	bool setWrapR(GLenum wrap);
	bool setMaxAnisotropy(GLfloat textureMaxAnisotropy);
	bool setBaseLevel(GLint baseLevel);
	bool setCompareFunc(GLenum compareFunc);
	bool setCompareMode(GLenum compareMode);
	void makeImmutable(GLsizei levels);
	bool setMaxLevel(GLint maxLevel);
	bool setMaxLOD(GLfloat maxLOD);
	bool setMinLOD(GLfloat minLOD);
	bool setSwizzleR(GLenum swizzleR);
	bool setSwizzleG(GLenum swizzleG);
	bool setSwizzleB(GLenum swizzleB);
	bool setSwizzleA(GLenum swizzleA);

    GLenum getMinFilter() const;
    GLenum getMagFilter() const;
    GLenum getWrapS() const;
	GLenum getWrapT() const;
	GLenum getWrapR() const;
	GLfloat getMaxAnisotropy() const;
	GLint getBaseLevel() const;
	GLenum getCompareFunc() const;
	GLenum getCompareMode() const;
	GLboolean getImmutableFormat() const;
	GLsizei getImmutableLevels() const;
	GLint getMaxLevel() const;
	GLfloat getMaxLOD() const;
	GLfloat getMinLOD() const;
	GLenum getSwizzleR() const;
	GLenum getSwizzleG() const;
	GLenum getSwizzleB() const;
	GLenum getSwizzleA() const;

    virtual GLsizei getWidth(GLenum target, GLint level) const = 0;
	virtual GLsizei getHeight(GLenum target, GLint level) const = 0;
	virtual GLsizei getDepth(GLenum target, GLint level) const;
    virtual GLenum getFormat(GLenum target, GLint level) const = 0;
    virtual GLenum getType(GLenum target, GLint level) const = 0;
    virtual sw::Format getInternalFormat(GLenum target, GLint level) const = 0;
	virtual int getLevelCount() const = 0;

    virtual bool isSamplerComplete() const = 0;
    virtual bool isCompressed(GLenum target, GLint level) const = 0;
	virtual bool isDepth(GLenum target, GLint level) const = 0;

    virtual Renderbuffer *getRenderbuffer(GLenum target, GLint level, GLint layer) = 0;
    virtual egl::Image *getRenderTarget(GLenum target, unsigned int level) = 0;
    virtual egl::Image *createSharedImage(GLenum target, unsigned int level);
    virtual bool isShared(GLenum target, unsigned int level) const = 0;

    virtual void generateMipmaps() = 0;
	virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source) = 0;

protected:
	virtual ~Texture();

    void setImage(GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels, egl::Image *image);
    void subImage(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels, egl::Image *image);
    void setCompressedImage(GLsizei imageSize, const void *pixels, egl::Image *image);
    void subImageCompressed(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *pixels, egl::Image *image);

	bool copy(egl::Image *source, const sw::SliceRect &sourceRect, GLenum destFormat, GLint xoffset, GLint yoffset, GLint zoffset, egl::Image *dest);

	bool isMipmapFiltered() const;

    GLenum mMinFilter;
    GLenum mMagFilter;
    GLenum mWrapS;
    GLenum mWrapT;
    GLenum mWrapR;
	GLfloat mMaxAnisotropy;
	GLint mBaseLevel;
	GLenum mCompareFunc;
	GLenum mCompareMode;
	GLboolean mImmutableFormat;
	GLsizei mImmutableLevels;
	GLint mMaxLevel;
	GLfloat mMaxLOD;
	GLfloat mMinLOD;
	GLenum mSwizzleR;
	GLenum mSwizzleG;
	GLenum mSwizzleB;
	GLenum mSwizzleA;

	sw::Resource *resource;
};

class Texture2D : public Texture
{
public:
    explicit Texture2D(GLuint name);

	void addProxyRef(const Renderbuffer *proxy) override;
    void releaseProxy(const Renderbuffer *proxy) override;
	void sweep() override;

    virtual GLenum getTarget() const;

    virtual GLsizei getWidth(GLenum target, GLint level) const;
    virtual GLsizei getHeight(GLenum target, GLint level) const;
    virtual GLenum getFormat(GLenum target, GLint level) const;
    virtual GLenum getType(GLenum target, GLint level) const;
    virtual sw::Format getInternalFormat(GLenum target, GLint level) const;
	virtual int getLevelCount() const;

    void setImage(GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels);
    void setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);
    void subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels);
    void subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels);
    void copyImage(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);
	virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

	void setImage(egl::Image *image);

    virtual bool isSamplerComplete() const;
    virtual bool isCompressed(GLenum target, GLint level) const;
	virtual bool isDepth(GLenum target, GLint level) const;
    virtual void bindTexImage(egl::Surface *surface);
    virtual void releaseTexImage();

    virtual void generateMipmaps();

	virtual Renderbuffer *getRenderbuffer(GLenum target, GLint level, GLint layer);
    virtual egl::Image *getRenderTarget(GLenum target, unsigned int level);
	virtual bool isShared(GLenum target, unsigned int level) const;

    egl::Image *getImage(unsigned int level);

protected:
	virtual ~Texture2D();

	bool isMipmapComplete() const;

	egl::Image *image[IMPLEMENTATION_MAX_TEXTURE_LEVELS];

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
    explicit TextureCubeMap(GLuint name);

	void addProxyRef(const Renderbuffer *proxy) override;
    void releaseProxy(const Renderbuffer *proxy) override;
	void sweep() override;

    virtual GLenum getTarget() const;

    virtual GLsizei getWidth(GLenum target, GLint level) const;
    virtual GLsizei getHeight(GLenum target, GLint level) const;
    virtual GLenum getFormat(GLenum target, GLint level) const;
    virtual GLenum getType(GLenum target, GLint level) const;
    virtual sw::Format getInternalFormat(GLenum target, GLint level) const;
	virtual int getLevelCount() const;

	void setImage(GLenum target, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels);
    void setCompressedImage(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);

    void subImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels);
    void subImageCompressed(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels);
    void copyImage(GLenum target, GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);
	virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

    virtual bool isSamplerComplete() const;
    virtual bool isCompressed(GLenum target, GLint level) const;
	virtual bool isDepth(GLenum target, GLint level) const;
	virtual void releaseTexImage();

    virtual void generateMipmaps();

    virtual Renderbuffer *getRenderbuffer(GLenum target, GLint level, GLint layer);
	virtual egl::Image *getRenderTarget(GLenum target, unsigned int level);
	virtual bool isShared(GLenum target, unsigned int level) const;

	egl::Image *getImage(int face, unsigned int level);

protected:
	virtual ~TextureCubeMap();

private:
	bool isCubeComplete() const;
	bool isMipmapCubeComplete() const;

    // face is one of the GL_TEXTURE_CUBE_MAP_* enumerants. Returns NULL on failure.
	egl::Image *getImage(GLenum face, unsigned int level);

	egl::Image *image[6][IMPLEMENTATION_MAX_TEXTURE_LEVELS];

	// A specific internal reference count is kept for colorbuffer proxy references,
    // because, as the renderbuffer acting as proxy will maintain a binding pointer
    // back to this texture, there would be a circular reference if we used a binding
    // pointer here. This reference count will cause the pointer to be set to NULL if
    // the count drops to zero, but will not cause deletion of the Renderbuffer.
    Renderbuffer *mFaceProxies[6];
	unsigned int mFaceProxyRefs[6];
};

class Texture3D : public Texture
{
public:
	explicit Texture3D(GLuint name);

	void addProxyRef(const Renderbuffer *proxy) override;
    void releaseProxy(const Renderbuffer *proxy) override;
	void sweep() override;

	virtual GLenum getTarget() const;

	virtual GLsizei getWidth(GLenum target, GLint level) const;
	virtual GLsizei getHeight(GLenum target, GLint level) const;
	virtual GLsizei getDepth(GLenum target, GLint level) const;
	virtual GLenum getFormat(GLenum target, GLint level) const;
	virtual GLenum getType(GLenum target, GLint level) const;
	virtual sw::Format getInternalFormat(GLenum target, GLint level) const;
	virtual int getLevelCount() const;

	void setImage(GLint level, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels);
	void setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels);
	void subImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels);
	void subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *pixels);
	void copyImage(GLint level, GLenum format, GLint x, GLint y, GLint z, GLsizei width, GLsizei height, GLsizei depth, Framebuffer *source);
	void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

	void setImage(egl::Image *image);

	virtual bool isSamplerComplete() const;
	virtual bool isCompressed(GLenum target, GLint level) const;
	virtual bool isDepth(GLenum target, GLint level) const;
	virtual void bindTexImage(egl::Surface *surface);
	virtual void releaseTexImage();

	virtual void generateMipmaps();

	virtual Renderbuffer *getRenderbuffer(GLenum target, GLint level, GLint layer);
	virtual egl::Image *getRenderTarget(GLenum target, unsigned int level);
	virtual bool isShared(GLenum target, unsigned int level) const;

	egl::Image *getImage(unsigned int level);

protected:
	virtual ~Texture3D();

	bool isMipmapComplete() const;

	egl::Image *image[IMPLEMENTATION_MAX_TEXTURE_LEVELS];

	egl::Surface *mSurface;

	// A specific internal reference count is kept for colorbuffer proxy references,
	// because, as the renderbuffer acting as proxy will maintain a binding pointer
	// back to this texture, there would be a circular reference if we used a binding
	// pointer here. This reference count will cause the pointer to be set to NULL if
	// the count drops to zero, but will not cause deletion of the Renderbuffer.
	Renderbuffer *mColorbufferProxy;
	unsigned int mProxyRefs;
};

class Texture2DArray : public Texture3D
{
public:
	explicit Texture2DArray(GLuint name);

	virtual GLenum getTarget() const;
	virtual void generateMipmaps();

protected:
	virtual ~Texture2DArray();
};

class TextureExternal : public Texture2D
{
public:
    explicit TextureExternal(GLuint name);

    virtual GLenum getTarget() const;

protected:
	virtual ~TextureExternal();
};
}

#endif   // LIBGLESV2_TEXTURE_H_
