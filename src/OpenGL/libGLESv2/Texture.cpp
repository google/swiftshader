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

// Texture.cpp: Implements the Texture class and its derived classes
// Texture2D, TextureCubeMap, Texture3D and Texture2DArray. Implements GL texture objects
// and related functionality. [OpenGL ES 2.0.24] section 3.7 page 63.

#include "Texture.h"

#include "main.h"
#include "mathutil.h"
#include "Framebuffer.h"
#include "Device.hpp"
#include "libEGL/Display.h"
#include "libEGL/Surface.h"
#include "common/debug.h"

#include <algorithm>

namespace es2
{

Texture::Texture(GLuint name) : egl::Texture(name)
{
    mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    mMagFilter = GL_LINEAR;
    mWrapS = GL_REPEAT;
    mWrapT = GL_REPEAT;
	mWrapR = GL_REPEAT;
	mMaxAnisotropy = 1.0f;
	mBaseLevel = 0;
	mCompareFunc = GL_LEQUAL;
	mCompareMode = GL_NONE;
	mImmutableFormat = GL_FALSE;
	mMaxLevel = 1000;
	mMaxLOD = 1000;
	mMinLOD = -1000;
	mSwizzleR = GL_RED;
	mSwizzleG = GL_GREEN;
	mSwizzleB = GL_BLUE;
	mSwizzleA = GL_ALPHA;

	resource = new sw::Resource(0);
}

Texture::~Texture()
{
	resource->destruct();
}

sw::Resource *Texture::getResource() const
{
	return resource;
}

// Returns true on successful filter state update (valid enum parameter)
bool Texture::setMinFilter(GLenum filter)
{
    switch(filter)
    {
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
        if(getTarget() == GL_TEXTURE_EXTERNAL_OES)
        {
            return false;
        }
        // Fall through
    case GL_NEAREST:
    case GL_LINEAR:
        mMinFilter = filter;
        return true;
    default:
        return false;
    }
}

// Returns true on successful filter state update (valid enum parameter)
bool Texture::setMagFilter(GLenum filter)
{
    switch(filter)
    {
    case GL_NEAREST:
    case GL_LINEAR:
        mMagFilter = filter;
        return true;
    default:
        return false;
    }
}

// Returns true on successful wrap state update (valid enum parameter)
bool Texture::setWrapS(GLenum wrap)
{
    switch(wrap)
    {
    case GL_REPEAT:
    case GL_MIRRORED_REPEAT:
        if(getTarget() == GL_TEXTURE_EXTERNAL_OES)
        {
            return false;
        }
        // Fall through
    case GL_CLAMP_TO_EDGE:
        mWrapS = wrap;
        return true;
    default:
        return false;
    }
}

// Returns true on successful wrap state update (valid enum parameter)
bool Texture::setWrapT(GLenum wrap)
{
	switch(wrap)
	{
	case GL_REPEAT:
	case GL_MIRRORED_REPEAT:
		if(getTarget() == GL_TEXTURE_EXTERNAL_OES)
		{
			return false;
		}
		// Fall through
	case GL_CLAMP_TO_EDGE:
		mWrapT = wrap;
		return true;
	default:
		return false;
	}
}

// Returns true on successful wrap state update (valid enum parameter)
bool Texture::setWrapR(GLenum wrap)
{
	switch(wrap)
	{
	case GL_REPEAT:
	case GL_MIRRORED_REPEAT:
		if(getTarget() == GL_TEXTURE_EXTERNAL_OES)
		{
			return false;
		}
		// Fall through
	case GL_CLAMP_TO_EDGE:
		mWrapR = wrap;
		return true;
	default:
		return false;
	}
}

// Returns true on successful max anisotropy update (valid anisotropy value)
bool Texture::setMaxAnisotropy(float textureMaxAnisotropy)
{
    textureMaxAnisotropy = std::min(textureMaxAnisotropy, MAX_TEXTURE_MAX_ANISOTROPY);

    if(textureMaxAnisotropy < 1.0f)
    {
        return false;
    }

	if(mMaxAnisotropy != textureMaxAnisotropy)
    {
        mMaxAnisotropy = textureMaxAnisotropy;
    }

    return true;
}

bool Texture::setBaseLevel(GLint baseLevel)
{
	mBaseLevel = baseLevel;
	return true;
}

bool Texture::setCompareFunc(GLenum compareFunc)
{
	switch(compareFunc)
	{
	case GL_LEQUAL:
	case GL_GEQUAL:
	case GL_LESS:
	case GL_GREATER:
	case GL_EQUAL:
	case GL_NOTEQUAL:
	case GL_ALWAYS:
	case GL_NEVER:
		mCompareFunc = compareFunc;
		return true;
	default:
		return false;
	}
}

bool Texture::setCompareMode(GLenum compareMode)
{
	switch(compareMode)
	{
	case GL_COMPARE_REF_TO_TEXTURE:
	case GL_NONE:
		mCompareMode = compareMode;
		return true;
	default:
		return false;
	}
}

bool Texture::setImmutableFormat(GLboolean immutableFormat)
{
	mImmutableFormat = immutableFormat;
	return true;
}

bool Texture::setMaxLevel(GLint maxLevel)
{
	mMaxLevel = maxLevel;
	return true;
}

bool Texture::setMaxLOD(GLfloat maxLOD)
{
	mMaxLOD = maxLOD;
	return true;
}

bool Texture::setMinLOD(GLfloat minLOD)
{
	mMinLOD = minLOD;
	return true;
}

bool Texture::setSwizzleR(GLenum swizzleR)
{
	switch(swizzleR)
	{
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_ZERO:
	case GL_ONE:
		mSwizzleR = swizzleR;
		return true;
	default:
		return false;
	}
}

bool Texture::setSwizzleG(GLenum swizzleG)
{
	switch(swizzleG)
	{
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_ZERO:
	case GL_ONE:
		mSwizzleG = swizzleG;
		return true;
	default:
		return false;
	}
}

bool Texture::setSwizzleB(GLenum swizzleB)
{
	switch(swizzleB)
	{
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_ZERO:
	case GL_ONE:
		mSwizzleB = swizzleB;
		return true;
	default:
		return false;
	}
}

bool Texture::setSwizzleA(GLenum swizzleA)
{
	switch(swizzleA)
	{
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_ZERO:
	case GL_ONE:
		mSwizzleA = swizzleA;
		return true;
	default:
		return false;
	}
}

GLenum Texture::getMinFilter() const
{
    return mMinFilter;
}

GLenum Texture::getMagFilter() const
{
    return mMagFilter;
}

GLenum Texture::getWrapS() const
{
    return mWrapS;
}

GLenum Texture::getWrapT() const
{
	return mWrapT;
}

GLenum Texture::getWrapR() const
{
	return mWrapR;
}

GLfloat Texture::getMaxAnisotropy() const
{
    return mMaxAnisotropy;
}

GLint Texture::getBaseLevel() const
{
	return mBaseLevel;
}
GLenum Texture::getCompareFunc() const
{
	return mCompareFunc;
}
GLenum Texture::getCompareMode() const
{
	return mCompareMode;
}
GLboolean Texture::getImmutableFormat() const
{
	return mImmutableFormat;
}
GLint Texture::getMaxLevel() const
{
	return mMaxLevel;
}
GLfloat Texture::getMaxLOD() const
{
	return mMaxLOD;
}
GLfloat Texture::getMinLOD() const
{
	return mMinLOD;
}
GLenum Texture::getSwizzleR() const
{
	return mSwizzleR;
}
GLenum Texture::getSwizzleG() const
{
	return mSwizzleG;
}
GLenum Texture::getSwizzleB() const
{
	return mSwizzleB;
}
GLenum Texture::getSwizzleA() const
{
	return mSwizzleA;
}

GLsizei Texture::getDepth(GLenum target, GLint level) const
{
	return 1;
}

egl::Image *Texture::createSharedImage(GLenum target, unsigned int level)
{
    egl::Image *image = getRenderTarget(target, level);   // Increments reference count

    if(image)
    {
        image->markShared();
    }

    return image;
}

void Texture::setImage(GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels, egl::Image *image)
{
    if(pixels && image)
    {
		GLsizei depth = (getTarget() == GL_TEXTURE_3D_OES || getTarget() == GL_TEXTURE_2D_ARRAY) ? image->getDepth() : 1;
		image->loadImageData(0, 0, 0, image->getWidth(), image->getHeight(), depth, format, type, unpackInfo, pixels);
    }
}

void Texture::setCompressedImage(GLsizei imageSize, const void *pixels, egl::Image *image)
{
    if(pixels && image)
    {
		GLsizei depth = (getTarget() == GL_TEXTURE_3D_OES || getTarget() == GL_TEXTURE_2D_ARRAY) ? image->getDepth() : 1;
		image->loadCompressedData(0, 0, 0, image->getWidth(), image->getHeight(), depth, imageSize, pixels);
    }
}

void Texture::subImage(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels, egl::Image *image)
{
	if(!image)
	{
		return error(GL_INVALID_OPERATION);
	}

	if(width + xoffset > image->getWidth() || height + yoffset > image->getHeight() || depth + zoffset > image->getDepth())
    {
        return error(GL_INVALID_VALUE);
    }

    if(IsCompressed(image->getFormat(), egl::getClientVersion()))
    {
        return error(GL_INVALID_OPERATION);
    }

    if(format != image->getFormat())
    {
        return error(GL_INVALID_OPERATION);
    }

    if(pixels)
    {
		image->loadImageData(xoffset, yoffset, zoffset, width, height, depth, format, type, unpackInfo, pixels);
    }
}

void Texture::subImageCompressed(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *pixels, egl::Image *image)
{
	if(!image)
	{
		return error(GL_INVALID_OPERATION);
	}

    if(width + xoffset > image->getWidth() || height + yoffset > image->getHeight() || depth + zoffset > image->getDepth())
    {
        return error(GL_INVALID_VALUE);
    }

    if(format != image->getFormat())
    {
        return error(GL_INVALID_OPERATION);
    }

    if(pixels)
    {
		image->loadCompressedData(xoffset, yoffset, zoffset, width, height, depth, imageSize, pixels);
    }
}

bool Texture::copy(egl::Image *source, const sw::SliceRect &sourceRect, GLenum destFormat, GLint xoffset, GLint yoffset, GLint zoffset, egl::Image *dest)
{
    Device *device = getDevice();

    sw::SliceRect destRect(xoffset, yoffset, xoffset + (sourceRect.x1 - sourceRect.x0), yoffset + (sourceRect.y1 - sourceRect.y0), zoffset);
    bool success = device->stretchRect(source, &sourceRect, dest, &destRect, false);

    if(!success)
    {
        return error(GL_OUT_OF_MEMORY, false);
    }

    return true;
}

bool Texture::isMipmapFiltered() const
{
	switch(mMinFilter)
    {
    case GL_NEAREST:
    case GL_LINEAR:
        return false;
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
        return true;
    default: UNREACHABLE(mMinFilter);
    }

	return false;
}

Texture2D::Texture2D(GLuint name) : Texture(name)
{
	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		image[i] = nullptr;
	}

    mSurface = nullptr;

	mColorbufferProxy = nullptr;
	mProxyRefs = 0;
}

Texture2D::~Texture2D()
{
	resource->lock(sw::DESTRUCT);

	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		if(image[i])
		{
			image[i]->unbind(this);
			image[i] = nullptr;
		}
	}

	resource->unlock();

    if(mSurface)
    {
        mSurface->setBoundTexture(nullptr);
        mSurface = nullptr;
    }

	mColorbufferProxy = nullptr;
}

// We need to maintain a count of references to renderbuffers acting as
// proxies for this texture, so that we do not attempt to use a pointer
// to a renderbuffer proxy which has been deleted.
void Texture2D::addProxyRef(const Renderbuffer *proxy)
{
    mProxyRefs++;
}

void Texture2D::releaseProxy(const Renderbuffer *proxy)
{
    if(mProxyRefs > 0)
	{
        mProxyRefs--;
	}

    if(mProxyRefs == 0)
	{
		mColorbufferProxy = nullptr;
	}
}

void Texture2D::sweep()
{
	int imageCount = 0;

	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		if(image[i] && image[i]->isChildOf(this))
		{
			if(!image[i]->hasSingleReference())
			{
				return;
			}

			imageCount++;
		}
	}

	if(imageCount == referenceCount)
	{
		destroy();
	}
}

GLenum Texture2D::getTarget() const
{
    return GL_TEXTURE_2D;
}

GLsizei Texture2D::getWidth(GLenum target, GLint level) const
{
	ASSERT(target == GL_TEXTURE_2D);
    return image[level] ? image[level]->getWidth() : 0;
}

GLsizei Texture2D::getHeight(GLenum target, GLint level) const
{
	ASSERT(target == GL_TEXTURE_2D);
    return image[level] ? image[level]->getHeight() : 0;
}

GLenum Texture2D::getFormat(GLenum target, GLint level) const
{
	ASSERT(target == GL_TEXTURE_2D);
	return image[level] ? image[level]->getFormat() : GL_NONE;
}

GLenum Texture2D::getType(GLenum target, GLint level) const
{
	ASSERT(target == GL_TEXTURE_2D);
	return image[level] ? image[level]->getType() : GL_NONE;
}

sw::Format Texture2D::getInternalFormat(GLenum target, GLint level) const
{
	ASSERT(target == GL_TEXTURE_2D);
	return image[level] ? image[level]->getInternalFormat() : sw::FORMAT_NULL;
}

int Texture2D::getLevelCount() const
{
	ASSERT(isSamplerComplete());
	int levels = 0;

	while(levels < MIPMAP_LEVELS && image[levels])
	{
		levels++;
	}

	return levels;
}

void Texture2D::setImage(GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels)
{
	if(image[level])
	{
		image[level]->release();
	}

	image[level] = new egl::Image(this, width, height, format, type);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	Texture::setImage(format, type, unpackInfo, pixels, image[level]);
}

void Texture2D::bindTexImage(egl::Surface *surface)
{
    GLenum format;

    switch(surface->getInternalFormat())
    {
    case sw::FORMAT_A8R8G8B8:
		format = GL_BGRA_EXT;
        break;
	case sw::FORMAT_A8B8G8R8:
        format = GL_RGBA;
        break;
    case sw::FORMAT_X8B8G8R8:
	case sw::FORMAT_X8R8G8B8:
        format = GL_RGB;
        break;
    default:
        UNIMPLEMENTED();
        return;
    }

	for(int level = 0; level < MIPMAP_LEVELS; level++)
	{
		if(image[level])
		{
			image[level]->release();
			image[level] = nullptr;
		}
	}

	image[0] = surface->getRenderTarget();

    mSurface = surface;
    mSurface->setBoundTexture(this);
}

void Texture2D::releaseTexImage()
{
    for(int level = 0; level < MIPMAP_LEVELS; level++)
	{
		if(image[level])
		{
			image[level]->release();
			image[level] = nullptr;
		}
	}
}

void Texture2D::setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels)
{
	if(image[level])
	{
		image[level]->release();
	}

	GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_UNSIGNED_BYTE);
	image[level] = new egl::Image(this, width, height, sizedInternalFormat, GL_UNSIGNED_BYTE);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

    Texture::setCompressedImage(imageSize, pixels, image[level]);
}

void Texture2D::subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels)
{
	Texture::subImage(xoffset, yoffset, 0, width, height, 1, format, type, unpackInfo, pixels, image[level]);
}

void Texture2D::subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels)
{
    Texture::subImageCompressed(xoffset, yoffset, 0, width, height, 1, format, imageSize, pixels, image[level]);
}

void Texture2D::copyImage(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
    egl::Image *renderTarget = source->getRenderTarget(0);

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

	if(image[level])
	{
		image[level]->release();
	}

	GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_UNSIGNED_BYTE);
	image[level] = new egl::Image(this, width, height, sizedInternalFormat, GL_UNSIGNED_BYTE);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

    if(width != 0 && height != 0)
    {
		Renderbuffer* renderbuffer = source->getReadColorbuffer();

		if(!renderbuffer)
		{
			ERR("Failed to retrieve the source colorbuffer.");
			return;
		}

		sw::SliceRect sourceRect(x, y, x + width, y + height, 0);
		sourceRect.clip(0, 0, renderbuffer->getWidth(), renderbuffer->getHeight());

		copy(renderTarget, sourceRect, sizedInternalFormat, 0, 0, 0, image[level]);
    }

	renderTarget->release();
}

void Texture2D::copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	if(!image[level])
	{
		return error(GL_INVALID_OPERATION);
	}

    if(xoffset + width > image[level]->getWidth() || yoffset + height > image[level]->getHeight() || zoffset != 0)
    {
        return error(GL_INVALID_VALUE);
    }

    egl::Image *renderTarget = source->getRenderTarget(0);

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

	Renderbuffer* renderbuffer = source->getReadColorbuffer();

	if(!renderbuffer)
	{
		ERR("Failed to retrieve the source colorbuffer.");
		return;
	}

	sw::SliceRect sourceRect(x, y, x + width, y + height, 0);
	sourceRect.clip(0, 0, renderbuffer->getWidth(), renderbuffer->getHeight());

	copy(renderTarget, sourceRect, image[level]->getFormat(), xoffset, yoffset, zoffset, image[level]);

	renderTarget->release();
}

void Texture2D::setImage(egl::Image *sharedImage)
{
	if (sharedImage == image[0])
	{
		return;
	}

	sharedImage->addRef();

    if(image[0])
    {
        image[0]->release();
    }

    image[0] = sharedImage;
}

// Tests for 2D texture sampling completeness. [OpenGL ES 2.0.24] section 3.8.2 page 85.
bool Texture2D::isSamplerComplete() const
{
	if(!image[0])
	{
		return false;
	}

    GLsizei width = image[0]->getWidth();
    GLsizei height = image[0]->getHeight();

    if(width <= 0 || height <= 0)
    {
        return false;
    }

    if(isMipmapFiltered())
    {
        if(!isMipmapComplete())
        {
            return false;
        }
    }

    return true;
}

// Tests for 2D texture (mipmap) completeness. [OpenGL ES 2.0.24] section 3.7.10 page 81.
bool Texture2D::isMipmapComplete() const
{
    GLsizei width = image[mBaseLevel]->getWidth();
    GLsizei height = image[mBaseLevel]->getHeight();

    int q = std::min(log2(std::max(width, height)), mMaxLevel);

    for(int level = mBaseLevel + 1; level <= q; level++)
    {
		if(!image[level])
		{
			return false;
		}

        if(image[level]->getFormat() != image[0]->getFormat())
        {
            return false;
        }

        if(image[level]->getType() != image[0]->getType())
        {
            return false;
        }

        if(image[level]->getWidth() != std::max(1, width >> level))
        {
            return false;
        }

        if(image[level]->getHeight() != std::max(1, height >> level))
        {
            return false;
        }
    }

    return true;
}

bool Texture2D::isCompressed(GLenum target, GLint level) const
{
    return IsCompressed(getFormat(target, level), egl::getClientVersion());
}

bool Texture2D::isDepth(GLenum target, GLint level) const
{
    return IsDepthTexture(getFormat(target, level));
}

void Texture2D::generateMipmaps()
{
	if(!image[0])
	{
		return;   // FIXME: error?
	}

    unsigned int q = log2(std::max(image[0]->getWidth(), image[0]->getHeight()));

	for(unsigned int i = 1; i <= q; i++)
    {
		if(image[i])
		{
			image[i]->release();
		}

		image[i] = new egl::Image(this, std::max(image[0]->getWidth() >> i, 1), std::max(image[0]->getHeight() >> i, 1), image[0]->getFormat(), image[0]->getType());

		if(!image[i])
		{
			return error(GL_OUT_OF_MEMORY);
		}

		getDevice()->stretchRect(image[i - 1], 0, image[i], 0, true);
    }
}

egl::Image *Texture2D::getImage(unsigned int level)
{
	return image[level];
}

Renderbuffer *Texture2D::getRenderbuffer(GLenum target, GLint level, GLint layer)
{
    if(target != GL_TEXTURE_2D)
    {
        return error(GL_INVALID_OPERATION, (Renderbuffer*)nullptr);
    }

    if(!mColorbufferProxy)
    {
        mColorbufferProxy = new Renderbuffer(name, new RenderbufferTexture2D(this, level));
    }

    return mColorbufferProxy;
}

egl::Image *Texture2D::getRenderTarget(GLenum target, unsigned int level)
{
    ASSERT(target == GL_TEXTURE_2D);
	ASSERT(level < IMPLEMENTATION_MAX_TEXTURE_LEVELS);

	if(image[level])
	{
		image[level]->addRef();
	}

	return image[level];
}

bool Texture2D::isShared(GLenum target, unsigned int level) const
{
    ASSERT(target == GL_TEXTURE_2D);
    ASSERT(level < IMPLEMENTATION_MAX_TEXTURE_LEVELS);

    if(mSurface)   // Bound to an EGLSurface
    {
        return true;
    }

    if(!image[level])
    {
        return false;
    }

    return image[level]->isShared();
}

TextureCubeMap::TextureCubeMap(GLuint name) : Texture(name)
{
	for(int f = 0; f < 6; f++)
	{
		for(int i = 0; i < MIPMAP_LEVELS; i++)
		{
			image[f][i] = nullptr;
		}
	}

	for(int f = 0; f < 6; f++)
    {
        mFaceProxies[f] = nullptr;
        mFaceProxyRefs[f] = 0;
	}
}

TextureCubeMap::~TextureCubeMap()
{
	resource->lock(sw::DESTRUCT);

	for(int f = 0; f < 6; f++)
	{
		for(int i = 0; i < MIPMAP_LEVELS; i++)
		{
			if(image[f][i])
			{
				image[f][i]->unbind(this);
				image[f][i] = nullptr;
			}
		}
	}

	resource->unlock();

    for(int i = 0; i < 6; i++)
    {
        mFaceProxies[i] = nullptr;
    }
}

// We need to maintain a count of references to renderbuffers acting as
// proxies for this texture, so that the texture is not deleted while
// proxy references still exist. If the reference count drops to zero,
// we set our proxy pointer null, so that a new attempt at referencing
// will cause recreation.
void TextureCubeMap::addProxyRef(const Renderbuffer *proxy)
{
    for(int f = 0; f < 6; f++)
    {
        if(mFaceProxies[f] == proxy)
        {
			mFaceProxyRefs[f]++;
		}
	}
}

void TextureCubeMap::releaseProxy(const Renderbuffer *proxy)
{
    for(int f = 0; f < 6; f++)
    {
        if(mFaceProxies[f] == proxy)
        {
            if(mFaceProxyRefs[f] > 0)
			{
				mFaceProxyRefs[f]--;
			}

            if(mFaceProxyRefs[f] == 0)
			{
				mFaceProxies[f] = nullptr;
			}
		}
    }
}

void TextureCubeMap::sweep()
{
	int imageCount = 0;

	for(int f = 0; f < 6; f++)
	{
		for(int i = 0; i < MIPMAP_LEVELS; i++)
		{
			if(image[f][i] && image[f][i]->isChildOf(this))
			{
				if(!image[f][i]->hasSingleReference())
				{
					return;
				}

				imageCount++;
			}
		}
	}

	if(imageCount == referenceCount)
	{
		destroy();
	}
}

GLenum TextureCubeMap::getTarget() const
{
    return GL_TEXTURE_CUBE_MAP;
}

GLsizei TextureCubeMap::getWidth(GLenum target, GLint level) const
{
	int face = CubeFaceIndex(target);
    return image[face][level] ? image[face][level]->getWidth() : 0;
}

GLsizei TextureCubeMap::getHeight(GLenum target, GLint level) const
{
	int face = CubeFaceIndex(target);
    return image[face][level] ? image[face][level]->getHeight() : 0;
}

GLenum TextureCubeMap::getFormat(GLenum target, GLint level) const
{
	int face = CubeFaceIndex(target);
    return image[face][level] ? image[face][level]->getFormat() : 0;
}

GLenum TextureCubeMap::getType(GLenum target, GLint level) const
{
	int face = CubeFaceIndex(target);
    return image[face][level] ? image[face][level]->getType() : 0;
}

sw::Format TextureCubeMap::getInternalFormat(GLenum target, GLint level) const
{
	int face = CubeFaceIndex(target);
    return image[face][level] ? image[face][level]->getInternalFormat() : sw::FORMAT_NULL;
}

int TextureCubeMap::getLevelCount() const
{
	ASSERT(isSamplerComplete());
	int levels = 0;

	while(levels < MIPMAP_LEVELS && image[0][levels])
	{
		levels++;
	}

	return levels;
}

void TextureCubeMap::setCompressedImage(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels)
{
	int face = CubeFaceIndex(target);

	if(image[face][level])
	{
		image[face][level]->release();
	}

	GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_UNSIGNED_BYTE);
	image[face][level] = new egl::Image(this, width, height, sizedInternalFormat, GL_UNSIGNED_BYTE);

	if(!image[face][level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

    Texture::setCompressedImage(imageSize, pixels, image[face][level]);
}

void TextureCubeMap::subImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels)
{
	Texture::subImage(xoffset, yoffset, 0, width, height, 1, format, type, unpackInfo, pixels, image[CubeFaceIndex(target)][level]);
}

void TextureCubeMap::subImageCompressed(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels)
{
    Texture::subImageCompressed(xoffset, yoffset, 0, width, height, 1, format, imageSize, pixels, image[CubeFaceIndex(target)][level]);
}

// Tests for cube map sampling completeness. [OpenGL ES 2.0.24] section 3.8.2 page 86.
bool TextureCubeMap::isSamplerComplete() const
{
	for(int face = 0; face < 6; face++)
    {
		if(!image[face][0])
		{
			return false;
		}
	}

    int size = image[0][0]->getWidth();

    if(size <= 0)
    {
        return false;
    }

    if(!isMipmapFiltered())
    {
        if(!isCubeComplete())
        {
            return false;
        }
    }
    else
    {
        if(!isMipmapCubeComplete())   // Also tests for isCubeComplete()
        {
            return false;
        }
    }

    return true;
}

// Tests for cube texture completeness. [OpenGL ES 2.0.24] section 3.7.10 page 81.
bool TextureCubeMap::isCubeComplete() const
{
    if(image[0][mBaseLevel]->getWidth() <= 0 || image[0][mBaseLevel]->getHeight() != image[0][mBaseLevel]->getWidth())
    {
        return false;
    }

    for(unsigned int face = 1; face < 6; face++)
    {
        if(image[face][mBaseLevel]->getWidth()  != image[0][mBaseLevel]->getWidth() ||
           image[face][mBaseLevel]->getWidth()  != image[0][mBaseLevel]->getHeight() ||
           image[face][mBaseLevel]->getFormat() != image[0][mBaseLevel]->getFormat() ||
           image[face][mBaseLevel]->getType()   != image[0][mBaseLevel]->getType())
        {
            return false;
        }
    }

    return true;
}

bool TextureCubeMap::isMipmapCubeComplete() const
{
    if(!isCubeComplete())
    {
        return false;
    }

    GLsizei size = image[0][mBaseLevel]->getWidth();
    int q = std::min(log2(size), mMaxLevel);

    for(int face = 0; face < 6; face++)
    {
        for(int level = mBaseLevel + 1; level <= q; level++)
        {
			if(!image[face][level])
			{
				return false;
			}

            if(image[face][level]->getFormat() != image[0][mBaseLevel]->getFormat())
            {
                return false;
            }

            if(image[face][level]->getType() != image[0][mBaseLevel]->getType())
            {
                return false;
            }

            if(image[face][level]->getWidth() != std::max(1, size >> level))
            {
                return false;
            }
        }
    }

    return true;
}

bool TextureCubeMap::isCompressed(GLenum target, GLint level) const
{
    return IsCompressed(getFormat(target, level), egl::getClientVersion());
}

bool TextureCubeMap::isDepth(GLenum target, GLint level) const
{
    return IsDepthTexture(getFormat(target, level));
}

void TextureCubeMap::releaseTexImage()
{
    UNREACHABLE(0);   // Cube maps cannot have an EGL surface bound as an image
}

void TextureCubeMap::setImage(GLenum target, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels)
{
	int face = CubeFaceIndex(target);

	if(image[face][level])
	{
		image[face][level]->release();
	}

	image[face][level] = new egl::Image(this, width, height, format, type);

	if(!image[face][level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	Texture::setImage(format, type, unpackInfo, pixels, image[face][level]);
}

void TextureCubeMap::copyImage(GLenum target, GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	egl::Image *renderTarget = source->getRenderTarget(0);

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

	int face = CubeFaceIndex(target);

	if(image[face][level])
	{
		image[face][level]->release();
	}

	GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_UNSIGNED_BYTE);
	image[face][level] = new egl::Image(this, width, height, sizedInternalFormat, GL_UNSIGNED_BYTE);

	if(!image[face][level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

    if(width != 0 && height != 0)
    {
		Renderbuffer* renderbuffer = source->getReadColorbuffer();

		if(!renderbuffer)
		{
			ERR("Failed to retrieve the source colorbuffer.");
			return;
		}

		sw::SliceRect sourceRect(x, y, x + width, y + height, 0);
		sourceRect.clip(0, 0, renderbuffer->getWidth(), renderbuffer->getHeight());

		copy(renderTarget, sourceRect, sizedInternalFormat, 0, 0, 0, image[face][level]);
    }

	renderTarget->release();
}

egl::Image *TextureCubeMap::getImage(int face, unsigned int level)
{
	return image[face][level];
}

egl::Image *TextureCubeMap::getImage(GLenum face, unsigned int level)
{
    return image[CubeFaceIndex(face)][level];
}

void TextureCubeMap::copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	int face = CubeFaceIndex(target);

	if(!image[face][level])
	{
		return error(GL_INVALID_OPERATION);
	}

    GLsizei size = image[face][level]->getWidth();

    if(xoffset + width > size || yoffset + height > size || zoffset != 0)
    {
        return error(GL_INVALID_VALUE);
    }

    egl::Image *renderTarget = source->getRenderTarget(0);

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

	Renderbuffer* renderbuffer = source->getReadColorbuffer();

	if(!renderbuffer)
	{
		ERR("Failed to retrieve the source colorbuffer.");
		return;
	}

	sw::SliceRect sourceRect(x, y, x + width, y + height, 0);
	sourceRect.clip(0, 0, renderbuffer->getWidth(), renderbuffer->getHeight());

	copy(renderTarget, sourceRect, image[face][level]->getFormat(), xoffset, yoffset, zoffset, image[face][level]);

	renderTarget->release();
}

void TextureCubeMap::generateMipmaps()
{
    if(!isCubeComplete())
    {
        return error(GL_INVALID_OPERATION);
    }

    unsigned int q = log2(image[0][0]->getWidth());

	for(unsigned int f = 0; f < 6; f++)
    {
		for(unsigned int i = 1; i <= q; i++)
		{
			if(image[f][i])
			{
				image[f][i]->release();
			}

			image[f][i] = new egl::Image(this, std::max(image[0][0]->getWidth() >> i, 1), std::max(image[0][0]->getHeight() >> i, 1), image[0][0]->getFormat(), image[0][0]->getType());

			if(!image[f][i])
			{
				return error(GL_OUT_OF_MEMORY);
			}

			getDevice()->stretchRect(image[f][i - 1], 0, image[f][i], 0, true);
		}
	}
}

Renderbuffer *TextureCubeMap::getRenderbuffer(GLenum target, GLint level, GLint layer)
{
    if(!IsCubemapTextureTarget(target))
    {
        return error(GL_INVALID_OPERATION, (Renderbuffer*)nullptr);
    }

    int face = CubeFaceIndex(target);

    if(!mFaceProxies[face])
    {
        mFaceProxies[face] = new Renderbuffer(name, new RenderbufferTextureCubeMap(this, target, level));
    }

    return mFaceProxies[face];
}

egl::Image *TextureCubeMap::getRenderTarget(GLenum target, unsigned int level)
{
    ASSERT(IsCubemapTextureTarget(target));
    ASSERT(level < IMPLEMENTATION_MAX_TEXTURE_LEVELS);

	int face = CubeFaceIndex(target);

	if(image[face][level])
	{
		image[face][level]->addRef();
	}

	return image[face][level];
}

bool TextureCubeMap::isShared(GLenum target, unsigned int level) const
{
    ASSERT(IsCubemapTextureTarget(target));
    ASSERT(level < IMPLEMENTATION_MAX_TEXTURE_LEVELS);

    int face = CubeFaceIndex(target);

    if(!image[face][level])
    {
        return false;
    }

    return image[face][level]->isShared();
}

Texture3D::Texture3D(GLuint name) : Texture(name)
{
	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		image[i] = nullptr;
	}

	mSurface = nullptr;

	mColorbufferProxy = nullptr;
	mProxyRefs = 0;
}

Texture3D::~Texture3D()
{
	resource->lock(sw::DESTRUCT);

	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		if(image[i])
		{
			image[i]->unbind(this);
			image[i] = nullptr;
		}
	}

	resource->unlock();

	if(mSurface)
	{
		mSurface->setBoundTexture(nullptr);
		mSurface = nullptr;
	}

	mColorbufferProxy = nullptr;
}

// We need to maintain a count of references to renderbuffers acting as
// proxies for this texture, so that we do not attempt to use a pointer
// to a renderbuffer proxy which has been deleted.
void Texture3D::addProxyRef(const Renderbuffer *proxy)
{
	mProxyRefs++;
}

void Texture3D::releaseProxy(const Renderbuffer *proxy)
{
	if(mProxyRefs > 0)
	{
		mProxyRefs--;
	}

	if(mProxyRefs == 0)
	{
		mColorbufferProxy = nullptr;
	}
}

void Texture3D::sweep()
{
	int imageCount = 0;

	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		if(image[i] && image[i]->isChildOf(this))
		{
			if(!image[i]->hasSingleReference())
			{
				return;
			}

			imageCount++;
		}
	}

	if(imageCount == referenceCount)
	{
		destroy();
	}
}

GLenum Texture3D::getTarget() const
{
	return GL_TEXTURE_3D_OES;
}

GLsizei Texture3D::getWidth(GLenum target, GLint level) const
{
	ASSERT(target == getTarget());
	return image[level] ? image[level]->getWidth() : 0;
}

GLsizei Texture3D::getHeight(GLenum target, GLint level) const
{
	ASSERT(target == getTarget());
	return image[level] ? image[level]->getHeight() : 0;
}

GLsizei Texture3D::getDepth(GLenum target, GLint level) const
{
	ASSERT(target == getTarget());
	return image[level] ? image[level]->getDepth() : 0;
}

GLenum Texture3D::getFormat(GLenum target, GLint level) const
{
	ASSERT(target == getTarget());
	return image[level] ? image[level]->getFormat() : GL_NONE;
}

GLenum Texture3D::getType(GLenum target, GLint level) const
{
	ASSERT(target == getTarget());
	return image[level] ? image[level]->getType() : GL_NONE;
}

sw::Format Texture3D::getInternalFormat(GLenum target, GLint level) const
{
	ASSERT(target == getTarget());
	return image[level] ? image[level]->getInternalFormat() : sw::FORMAT_NULL;
}

int Texture3D::getLevelCount() const
{
	ASSERT(isSamplerComplete());
	int levels = 0;

	while(levels < MIPMAP_LEVELS && image[levels])
	{
		levels++;
	}

	return levels;
}

void Texture3D::setImage(GLint level, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels)
{
	if(image[level])
	{
		image[level]->release();
	}

	image[level] = new egl::Image(this, width, height, depth, format, type);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	Texture::setImage(format, type, unpackInfo, pixels, image[level]);
}

void Texture3D::bindTexImage(egl::Surface *surface)
{
	GLenum format;

	switch(surface->getInternalFormat())
	{
	case sw::FORMAT_A8R8G8B8:
		format = GL_RGBA;
		break;
	case sw::FORMAT_X8R8G8B8:
		format = GL_RGB;
		break;
	default:
		UNIMPLEMENTED();
		return;
	}

	for(int level = 0; level < MIPMAP_LEVELS; level++)
	{
		if(image[level])
		{
			image[level]->release();
			image[level] = nullptr;
		}
	}

	image[0] = surface->getRenderTarget();

	mSurface = surface;
	mSurface->setBoundTexture(this);
}

void Texture3D::releaseTexImage()
{
	for(int level = 0; level < MIPMAP_LEVELS; level++)
	{
		if(image[level])
		{
			image[level]->release();
			image[level] = nullptr;
		}
	}
}

void Texture3D::setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels)
{
	if(image[level])
	{
		image[level]->release();
	}

	GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_UNSIGNED_BYTE);
	image[level] = new egl::Image(this, width, height, depth, sizedInternalFormat, GL_UNSIGNED_BYTE);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	Texture::setCompressedImage(imageSize, pixels, image[level]);
}

void Texture3D::subImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const egl::Image::UnpackInfo& unpackInfo, const void *pixels)
{
	Texture::subImage(xoffset, yoffset, zoffset, width, height, depth, format, type, unpackInfo, pixels, image[level]);
}

void Texture3D::subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *pixels)
{
	Texture::subImageCompressed(xoffset, yoffset, zoffset, width, height, depth, format, imageSize, pixels, image[level]);
}

void Texture3D::copyImage(GLint level, GLenum format, GLint x, GLint y, GLint z, GLsizei width, GLsizei height, GLsizei depth, Framebuffer *source)
{
	egl::Image *renderTarget = source->getRenderTarget(0);

	if(!renderTarget)
	{
		ERR("Failed to retrieve the render target.");
		return error(GL_OUT_OF_MEMORY);
	}

	if(image[level])
	{
		image[level]->release();
	}

	GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_UNSIGNED_BYTE);
	image[level] = new egl::Image(this, width, height, depth, sizedInternalFormat, GL_UNSIGNED_BYTE);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	if(width != 0 && height != 0 && depth != 0)
	{
		Renderbuffer* renderbuffer = source->getReadColorbuffer();

		if(!renderbuffer)
		{
			ERR("Failed to retrieve the source colorbuffer.");
			return;
		}

		sw::SliceRect sourceRect(x, y, x + width, y + height, z);
		sourceRect.clip(0, 0, renderbuffer->getWidth(), renderbuffer->getHeight());
		for(GLint sliceZ = 0; sliceZ < depth; ++sliceZ, ++sourceRect.slice)
		{
			copy(renderTarget, sourceRect, sizedInternalFormat, 0, 0, sliceZ, image[level]);
		}
	}

	renderTarget->release();
}

void Texture3D::copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	if(!image[level])
	{
		return error(GL_INVALID_OPERATION);
	}

	if(xoffset + width > image[level]->getWidth() || yoffset + height > image[level]->getHeight() || zoffset >= image[level]->getDepth())
	{
		return error(GL_INVALID_VALUE);
	}

	egl::Image *renderTarget = source->getRenderTarget(0);

	if(!renderTarget)
	{
		ERR("Failed to retrieve the render target.");
		return error(GL_OUT_OF_MEMORY);
	}

	Renderbuffer* renderbuffer = source->getReadColorbuffer();

	if(!renderbuffer)
	{
		ERR("Failed to retrieve the source colorbuffer.");
		return;
	}

	sw::SliceRect sourceRect = {x, y, x + width, y + height, 0};
	sourceRect.clip(0, 0, renderbuffer->getWidth(), renderbuffer->getHeight());

	copy(renderTarget, sourceRect, image[level]->getFormat(), xoffset, yoffset, zoffset, image[level]);

	renderTarget->release();
}

void Texture3D::setImage(egl::Image *sharedImage)
{
	sharedImage->addRef();

	if(image[0])
	{
		image[0]->release();
	}

	image[0] = sharedImage;
}

// Tests for 3D texture sampling completeness. [OpenGL ES 2.0.24] section 3.8.2 page 85.
bool Texture3D::isSamplerComplete() const
{
	if(!image[0])
	{
		return false;
	}

	GLsizei width = image[0]->getWidth();
	GLsizei height = image[0]->getHeight();
	GLsizei depth = image[0]->getDepth();

	if(width <= 0 || height <= 0 || depth <= 0)
	{
		return false;
	}

	if(isMipmapFiltered())
	{
		if(!isMipmapComplete())
		{
			return false;
		}
	}

	return true;
}

// Tests for 3D texture (mipmap) completeness. [OpenGL ES 2.0.24] section 3.7.10 page 81.
bool Texture3D::isMipmapComplete() const
{
	GLsizei width = image[mBaseLevel]->getWidth();
	GLsizei height = image[mBaseLevel]->getHeight();
	GLsizei depth = image[mBaseLevel]->getDepth();
	bool isTexture2DArray = getTarget() == GL_TEXTURE_2D_ARRAY;

	int q = isTexture2DArray ? std::min(log2(std::max(width, height)), mMaxLevel) :
	        std::min(log2(std::max(std::max(width, height), depth)), mMaxLevel);

	for(int level = mBaseLevel + 1; level <= q; level++)
	{
		if(!image[level])
		{
			return false;
		}

		if(image[level]->getFormat() != image[0]->getFormat())
		{
			return false;
		}

		if(image[level]->getType() != image[0]->getType())
		{
			return false;
		}

		if(image[level]->getWidth() != std::max(1, width >> level))
		{
			return false;
		}

		if(image[level]->getHeight() != std::max(1, height >> level))
		{
			return false;
		}

		int levelDepth = isTexture2DArray ? depth : std::max(1, depth >> level);
		if(image[level]->getDepth() != levelDepth)
		{
			return false;
		}
	}

	return true;
}

bool Texture3D::isCompressed(GLenum target, GLint level) const
{
	return IsCompressed(getFormat(target, level), egl::getClientVersion());
}

bool Texture3D::isDepth(GLenum target, GLint level) const
{
	return IsDepthTexture(getFormat(target, level));
}

void Texture3D::generateMipmaps()
{
	if(!image[0])
	{
		return;   // FIXME: error?
	}

	unsigned int q = log2(std::max(std::max(image[0]->getWidth(), image[0]->getHeight()), image[0]->getDepth()));

	for(unsigned int i = 1; i <= q; i++)
	{
		if(image[i])
		{
			image[i]->release();
		}

		image[i] = new egl::Image(this, std::max(image[0]->getWidth() >> i, 1), std::max(image[0]->getHeight() >> i, 1), std::max(image[0]->getDepth() >> i, 1), image[0]->getFormat(), image[0]->getType());

		if(!image[i])
		{
			return error(GL_OUT_OF_MEMORY);
		}

		getDevice()->stretchCube(image[i - 1], image[i]);
	}
}

egl::Image *Texture3D::getImage(unsigned int level)
{
	return image[level];
}

Renderbuffer *Texture3D::getRenderbuffer(GLenum target, GLint level, GLint layer)
{
	if(target != getTarget())
	{
		return error(GL_INVALID_OPERATION, (Renderbuffer*)nullptr);
	}

	if(!mColorbufferProxy)
	{
		mColorbufferProxy = new Renderbuffer(name, new RenderbufferTexture3D(this, level, layer));
	}

	return mColorbufferProxy;
}

egl::Image *Texture3D::getRenderTarget(GLenum target, unsigned int level)
{
	ASSERT(target == getTarget());
	ASSERT(level < IMPLEMENTATION_MAX_TEXTURE_LEVELS);

	if(image[level])
	{
		image[level]->addRef();
	}

	return image[level];
}

bool Texture3D::isShared(GLenum target, unsigned int level) const
{
	ASSERT(target == getTarget());
	ASSERT(level < IMPLEMENTATION_MAX_TEXTURE_LEVELS);

	if(mSurface)   // Bound to an EGLSurface
	{
		return true;
	}

	if(!image[level])
	{
		return false;
	}

	return image[level]->isShared();
}

Texture2DArray::Texture2DArray(GLuint name) : Texture3D(name)
{
}

Texture2DArray::~Texture2DArray()
{
}

GLenum Texture2DArray::getTarget() const
{
	return GL_TEXTURE_2D_ARRAY;
}

void Texture2DArray::generateMipmaps()
{
	int depth = image[0] ? image[0]->getDepth() : 0;
	if(!depth)
	{
		return;   // FIXME: error?
	}

	unsigned int q = log2(std::max(image[0]->getWidth(), image[0]->getHeight()));

	for(unsigned int i = 1; i <= q; i++)
	{
		if(image[i])
		{
			image[i]->release();
		}

		GLsizei w = std::max(image[0]->getWidth() >> i, 1);
		GLsizei h = std::max(image[0]->getHeight() >> i, 1);
		image[i] = new egl::Image(this, w, h, depth, image[0]->getFormat(), image[0]->getType());

		if(!image[i])
		{
			return error(GL_OUT_OF_MEMORY);
		}

		GLsizei srcw = image[i - 1]->getWidth();
		GLsizei srch = image[i - 1]->getHeight();
		for(int z = 0; z < depth; ++z)
		{
			sw::SliceRect srcRect(0, 0, srcw, srch, z);
			sw::SliceRect dstRect(0, 0, w, h, z);
			getDevice()->stretchRect(image[i - 1], &srcRect, image[i], &dstRect, true);
		}
	}
}

TextureExternal::TextureExternal(GLuint name) : Texture2D(name)
{
    mMinFilter = GL_LINEAR;
    mMagFilter = GL_LINEAR;
    mWrapS = GL_CLAMP_TO_EDGE;
	mWrapT = GL_CLAMP_TO_EDGE;
	mWrapR = GL_CLAMP_TO_EDGE;
}

TextureExternal::~TextureExternal()
{
}

GLenum TextureExternal::getTarget() const
{
    return GL_TEXTURE_EXTERNAL_OES;
}

}

egl::Image *createBackBuffer(int width, int height, const egl::Config *config)
{
	if(config)
	{
		return new egl::Image(width, height, config->mRenderTargetFormat, config->mSamples, false);
	}

	return nullptr;
}

egl::Image *createDepthStencil(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard)
{
	if(width == 0 || height == 0 || height > OUTLINE_RESOLUTION)
	{
		ERR("Invalid parameters: %dx%d", width, height);
		return 0;
	}

	bool lockable = true;

	switch(format)
	{
//	case sw::FORMAT_D15S1:
	case sw::FORMAT_D24S8:
	case sw::FORMAT_D24X8:
//	case sw::FORMAT_D24X4S4:
	case sw::FORMAT_D24FS8:
	case sw::FORMAT_D32:
	case sw::FORMAT_D16:
		lockable = false;
		break;
//	case sw::FORMAT_S8_LOCKABLE:
//	case sw::FORMAT_D16_LOCKABLE:
	case sw::FORMAT_D32F_LOCKABLE:
//	case sw::FORMAT_D32_LOCKABLE:
	case sw::FORMAT_DF24S8:
	case sw::FORMAT_DF16S8:
		lockable = true;
		break;
	default:
		UNREACHABLE(format);
	}

	egl::Image *surface = new egl::Image(width, height, format, multiSampleDepth, lockable);

	if(!surface)
	{
		ERR("Out of memory");
		return nullptr;
	}

	return surface;
}
