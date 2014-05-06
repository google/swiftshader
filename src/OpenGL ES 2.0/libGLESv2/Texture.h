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
enum CubeFace;

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
	virtual bool isTexture2D();
	virtual bool isTextureCubeMap();

    virtual GLenum getTarget() const = 0;

    bool setMinFilter(GLenum filter);
    bool setMagFilter(GLenum filter);
    bool setWrapS(GLenum wrap);
    bool setWrapT(GLenum wrap);

    GLenum getMinFilter() const;
    GLenum getMagFilter() const;
    GLenum getWrapS() const;
    GLenum getWrapT() const;

    virtual GLsizei getWidth(GLint level = 0) const = 0;
    virtual GLsizei getHeight(GLint level = 0) const = 0;
    virtual GLenum getFormat() const = 0;
    virtual GLenum getType() const = 0;
    virtual sw::Format getInternalFormat() const = 0;
	virtual int getLevelCount() const = 0;

    virtual bool isSamplerComplete() const = 0;
    virtual bool isCompressed() const = 0;

    virtual Renderbuffer *getRenderbuffer(GLenum target) = 0;
	virtual Image *getRenderTarget(GLenum target) = 0;

    virtual void generateMipmaps() = 0;
    virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source) = 0;

    static const GLuint INCOMPLETE_TEXTURE_ID = static_cast<GLuint>(-1);   // Every texture takes an id at creation time. The value is arbitrary because it is never registered with the resource manager.

protected:
    void setImage(GLint unpackAlignment, const void *pixels, Image *image);
    void subImage(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels, Image *image);
    void setCompressedImage(GLsizei imageSize, const void *pixels, Image *image);
    void subImageCompressed(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels, Image *image);

	bool copy(Image *source, const sw::Rect &sourceRect, GLenum destFormat, GLint xoffset, GLint yoffset, Image *dest);

    GLenum mMinFilter;
    GLenum mMagFilter;
    GLenum mWrapS;
    GLenum mWrapT;

	sw::Resource *resource;

private:
    DISALLOW_COPY_AND_ASSIGN(Texture);

    void loadImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
                       GLint unpackAlignment, const void *input, std::size_t outputPitch, void *output, Image *image) const;

    void loadAlphaImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                            int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadAlphaFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                 int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadAlphaHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                     int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadLuminanceImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                int inputPitch, const void *input, size_t outputPitch, void *output, bool native) const;
    void loadLuminanceFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                     int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadLuminanceHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                         int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadLuminanceAlphaImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                     int inputPitch, const void *input, size_t outputPitch, void *output, bool native) const;
    void loadLuminanceAlphaFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                          int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadLuminanceAlphaHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                              int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBUByteImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                               int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGB565ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                             int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                               int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                   int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBAUByteImageDataSSE2(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                    int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBAUByteImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBA4444ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                               int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBA5551ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                               int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBAFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadRGBAHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                    int inputPitch, const void *input, size_t outputPitch, void *output) const;
    void loadBGRAImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                           int inputPitch, const void *input, size_t outputPitch, void *output) const;
};

class Texture2D : public Texture
{
  public:
    explicit Texture2D(GLuint id);

    ~Texture2D();

	virtual bool isTexture2D();

    virtual GLenum getTarget() const;

    virtual GLsizei getWidth(GLint level = 0) const;
    virtual GLsizei getHeight(GLint level = 0) const;
    virtual GLenum getFormat() const;
    virtual GLenum getType() const;
    virtual sw::Format getInternalFormat() const;
	virtual int getLevelCount() const;

    void setImage(GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);
    void subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels);
    void copyImage(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);
    void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

    virtual bool isSamplerComplete() const;
    virtual bool isCompressed() const;
    virtual void bindTexImage(egl::Surface *surface);
    virtual void releaseTexImage();

    virtual void generateMipmaps();

	virtual Image *getImage(unsigned int level);
    virtual Renderbuffer *getRenderbuffer(GLenum target);
	Image *getRenderTarget(GLenum target);

  private:
	bool isMipmapComplete() const;

	Image *image[IMPLEMENTATION_MAX_TEXTURE_LEVELS];
    
    egl::Surface *mSurface;
    BindingPointer<Renderbuffer> mColorbufferProxy;
};

class TextureCubeMap : public Texture
{
  public:
    explicit TextureCubeMap(GLuint id);

    ~TextureCubeMap();

	virtual bool isTextureCubeMap();

    virtual GLenum getTarget() const;
    
    virtual GLsizei getWidth(GLint level = 0) const;
    virtual GLsizei getHeight(GLint level = 0) const;
    virtual GLenum getFormat() const;
    virtual GLenum getType() const;
    virtual sw::Format getInternalFormat() const;
	virtual int getLevelCount() const;

	void setImage(GLenum target, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void setCompressedImage(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels);

    void subImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels);
    void subImageCompressed(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels);
    void copyImage(GLenum target, GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);
    virtual void copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source);

    virtual bool isSamplerComplete() const;
    virtual bool isCompressed() const;

    virtual void generateMipmaps();

    virtual Renderbuffer *getRenderbuffer(GLenum target);

	Image *getImage(CubeFace face, unsigned int level);

  private:
	bool isCubeComplete() const;
	bool isMipmapCubeComplete() const;

    virtual Image *getRenderTarget(GLenum target);

    // face is one of the GL_TEXTURE_CUBE_MAP_* enumerants. Returns NULL on failure.
    Image *getImage(GLenum face, unsigned int level);

    Image *image[6][IMPLEMENTATION_MAX_TEXTURE_LEVELS];
	
    BindingPointer<Renderbuffer> mFaceProxies[6];
};
}

extern "C"
{
	gl::Image *createBackBuffer(int width, int height, const egl::Config *config);
}

#endif   // LIBGLESV2_TEXTURE_H_
