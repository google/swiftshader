//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Texture.cpp: Implements the Texture class and its derived classes
// Texture2D and TextureCubeMap. Implements GL texture objects and related
// functionality. [OpenGL ES 2.0.24] section 3.7 page 63.

#include "Texture.h"

#include "main.h"
#include "mathutil.h"
#include "utilities.h"
#include "Framebuffer.h"
#include "Device.hpp"
#include "libEGL/Display.h"
#include "common/debug.h"

#include <algorithm>
#include <intrin.h>

namespace gl
{

sw::Format Image::selectInternalFormat(GLenum format, GLenum type)
{
	#if S3TC_SUPPORT
    if(format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
       format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
    {
        return sw::FORMAT_DXT1;
    }
    else
	#endif
	if(type == GL_FLOAT)
    {
        return sw::FORMAT_A32B32G32R32F;
    }
    else if(type == GL_HALF_FLOAT_OES)
    {
        return sw::FORMAT_A16B16G16R16F;
    }
    else if(type == GL_UNSIGNED_BYTE)
    {
        if(format == GL_LUMINANCE)
        {
            return sw::FORMAT_L8;
        }
        else if(format == GL_LUMINANCE_ALPHA)
        {
            return sw::FORMAT_A8L8;
        }
        else if(format == GL_RGB)
        {
            return sw::FORMAT_X8R8G8B8;
        }

        return sw::FORMAT_A8R8G8B8;
    }

    return sw::FORMAT_A8R8G8B8;
}

Texture::Texture(GLuint id) : RefCountObject(id)
{
    mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    mMagFilter = GL_LINEAR;
    mWrapS = GL_REPEAT;
    mWrapT = GL_REPEAT;

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

bool Texture::isTexture2D()
{
	return false;
}

bool Texture::isTextureCubeMap()
{
	return false;
}

// Returns true on successful filter state update (valid enum parameter)
bool Texture::setMinFilter(GLenum filter)
{
    switch(filter)
    {
    case GL_NEAREST:
    case GL_LINEAR:
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
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
    case GL_CLAMP_TO_EDGE:
    case GL_MIRRORED_REPEAT:
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
    case GL_CLAMP_TO_EDGE:
    case GL_MIRRORED_REPEAT:
         mWrapT = wrap;
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

// Store the pixel rectangle designated by xoffset,yoffset,width,height with pixels stored as format/type at input
// into the target pixel rectangle at output with outputPitch bytes in between each line.
void Texture::loadImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
                            GLint unpackAlignment, const void *input, size_t outputPitch, void *output, Image *image) const
{
    GLsizei inputPitch = -ComputePitch(width, format, type, unpackAlignment);
    input = ((char*)input) - inputPitch * (height - 1);

    switch (type)
    {
      case GL_UNSIGNED_BYTE:
        switch (format)
        {
          case GL_ALPHA:
            loadAlphaImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_LUMINANCE:
            loadLuminanceImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output, image->getInternalFormat() == sw::FORMAT_L8);
            break;
          case GL_LUMINANCE_ALPHA:
            loadLuminanceAlphaImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output, image->getInternalFormat() == sw::FORMAT_A8L8);
            break;
          case GL_RGB:
            loadRGBUByteImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_RGBA:
            if(supportsSSE2())
            {
                loadRGBAUByteImageDataSSE2(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            }
            else
            {
                loadRGBAUByteImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            }
            break;
          case GL_BGRA_EXT:
            loadBGRAImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          default: UNREACHABLE();
        }
        break;
      case GL_UNSIGNED_SHORT_5_6_5:
        switch (format)
        {
          case GL_RGB:
            loadRGB565ImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          default: UNREACHABLE();
        }
        break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
        switch (format)
        {
          case GL_RGBA:
            loadRGBA4444ImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          default: UNREACHABLE();
        }
        break;
      case GL_UNSIGNED_SHORT_5_5_5_1:
        switch (format)
        {
          case GL_RGBA:
            loadRGBA5551ImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          default: UNREACHABLE();
        }
        break;
      case GL_FLOAT:
        switch (format)
        {
          // float textures are converted to RGBA, not BGRA
          case GL_ALPHA:
            loadAlphaFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_LUMINANCE:
            loadLuminanceFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_LUMINANCE_ALPHA:
            loadLuminanceAlphaFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_RGB:
            loadRGBFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_RGBA:
            loadRGBAFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          default: UNREACHABLE();
        }
        break;
      case GL_HALF_FLOAT_OES:
        switch (format)
        {
          // float textures are converted to RGBA, not BGRA
          case GL_ALPHA:
            loadAlphaHalfFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_LUMINANCE:
            loadLuminanceHalfFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_LUMINANCE_ALPHA:
            loadLuminanceAlphaHalfFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_RGB:
            loadRGBHalfFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          case GL_RGBA:
            loadRGBAHalfFloatImageData(xoffset, yoffset, width, height, inputPitch, input, outputPitch, output);
            break;
          default: UNREACHABLE();
        }
        break;
      default: UNREACHABLE();
    }
}

void Texture::loadAlphaImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                 int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned char *source = NULL;
    unsigned char *dest = NULL;
    
    for(int y = 0; y < height; y++)
    {
        source = static_cast<const unsigned char*>(input) + y * inputPitch;
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4;
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = 0;
            dest[4 * x + 1] = 0;
            dest[4 * x + 2] = 0;
            dest[4 * x + 3] = source[x];
        }
    }
}

void Texture::loadAlphaFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const float *source = NULL;
    float *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const float*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<float*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 16);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = 0;
            dest[4 * x + 1] = 0;
            dest[4 * x + 2] = 0;
            dest[4 * x + 3] = source[x];
        }
    }
}

void Texture::loadAlphaHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                          int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned short *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<unsigned short*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 8);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = 0;
            dest[4 * x + 1] = 0;
            dest[4 * x + 2] = 0;
            dest[4 * x + 3] = source[x];
        }
    }
}

void Texture::loadLuminanceImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                     int inputPitch, const void *input, size_t outputPitch, void *output, bool native) const
{
    const int destBytesPerPixel = native? 1: 4;
    const unsigned char *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = static_cast<const unsigned char*>(input) + y * inputPitch;
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * destBytesPerPixel;

        if(!native)   // BGRA8 destination format
        {
            for(int x = 0; x < width; x++)
            {
                dest[4 * x + 0] = source[x];
                dest[4 * x + 1] = source[x];
                dest[4 * x + 2] = source[x];
                dest[4 * x + 3] = 0xFF;
            }
        }
        else   // L8 destination format
        {
            memcpy(dest, source, width);
        }
    }
}

void Texture::loadLuminanceFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                          int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const float *source = NULL;
    float *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const float*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<float*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 16);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[x];
            dest[4 * x + 1] = source[x];
            dest[4 * x + 2] = source[x];
            dest[4 * x + 3] = 1.0f;
        }
    }
}

void Texture::loadLuminanceHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                                   int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned short *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<unsigned short*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 8);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[x];
            dest[4 * x + 1] = source[x];
            dest[4 * x + 2] = source[x];
            dest[4 * x + 3] = 0x3C00; // SEEEEEMMMMMMMMMM, S = 0, E = 15, M = 0: 16bit flpt representation of 1
        }
    }
}

void Texture::loadLuminanceAlphaImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                          int inputPitch, const void *input, size_t outputPitch, void *output, bool native) const
{
    const int destBytesPerPixel = native? 2: 4;
    const unsigned char *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = static_cast<const unsigned char*>(input) + y * inputPitch;
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * destBytesPerPixel;
        
        if(!native)   // BGRA8 destination format
        {
            for(int x = 0; x < width; x++)
            {
                dest[4 * x + 0] = source[2*x+0];
                dest[4 * x + 1] = source[2*x+0];
                dest[4 * x + 2] = source[2*x+0];
                dest[4 * x + 3] = source[2*x+1];
            }
        }
        else
        {
            memcpy(dest, source, width * 2);
        }
    }
}

void Texture::loadLuminanceAlphaFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                               int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const float *source = NULL;
    float *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const float*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<float*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 16);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[2*x+0];
            dest[4 * x + 1] = source[2*x+0];
            dest[4 * x + 2] = source[2*x+0];
            dest[4 * x + 3] = source[2*x+1];
        }
    }
}

void Texture::loadLuminanceAlphaHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                                   int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned short *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<unsigned short*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 8);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[2*x+0];
            dest[4 * x + 1] = source[2*x+0];
            dest[4 * x + 2] = source[2*x+0];
            dest[4 * x + 3] = source[2*x+1];
        }
    }
}

void Texture::loadRGBUByteImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                    int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned char *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = static_cast<const unsigned char*>(input) + y * inputPitch;
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4;
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[x * 3 + 2];
            dest[4 * x + 1] = source[x * 3 + 1];
            dest[4 * x + 2] = source[x * 3 + 0];
            dest[4 * x + 3] = 0xFF;
        }
    }
}

void Texture::loadRGB565ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                  int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4;
        for(int x = 0; x < width; x++)
        {
            unsigned short rgba = source[x];
            dest[4 * x + 0] = ((rgba & 0x001F) << 3) | ((rgba & 0x001F) >> 2);
            dest[4 * x + 1] = ((rgba & 0x07E0) >> 3) | ((rgba & 0x07E0) >> 9);
            dest[4 * x + 2] = ((rgba & 0xF800) >> 8) | ((rgba & 0xF800) >> 13);
            dest[4 * x + 3] = 0xFF;
        }
    }
}

void Texture::loadRGBFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                    int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const float *source = NULL;
    float *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const float*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<float*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 16);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[x * 3 + 0];
            dest[4 * x + 1] = source[x * 3 + 1];
            dest[4 * x + 2] = source[x * 3 + 2];
            dest[4 * x + 3] = 1.0f;
        }
    }
}

void Texture::loadRGBHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                        int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned short *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<unsigned short*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 8);
        for(int x = 0; x < width; x++)
        {
            dest[4 * x + 0] = source[x * 3 + 0];
            dest[4 * x + 1] = source[x * 3 + 1];
            dest[4 * x + 2] = source[x * 3 + 2];
            dest[4 * x + 3] = 0x3C00; // SEEEEEMMMMMMMMMM, S = 0, E = 15, M = 0: 16bit flpt representation of 1
        }
    }
}

void Texture::loadRGBAUByteImageDataSSE2(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                         int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned int *source = NULL;
    unsigned int *dest = NULL;
    __m128i brMask = _mm_set1_epi32(0x00ff00ff);

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned int*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4);
        int x = 0;

        // Make output writes aligned
        for(x = 0; ((reinterpret_cast<intptr_t>(&dest[x]) & 15) != 0) && x < width; x++)
        {
            unsigned int rgba = source[x];
            dest[x] = (_rotl(rgba, 16) & 0x00ff00ff) | (rgba & 0xff00ff00);
        }

        for(; x + 3 < width; x += 4)
        {
            __m128i sourceData = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&source[x]));
            // Mask out g and a, which don't change
            __m128i gaComponents = _mm_andnot_si128(brMask, sourceData);
            // Mask out b and r
            __m128i brComponents = _mm_and_si128(sourceData, brMask);
            // Swap b and r
            __m128i brSwapped = _mm_shufflehi_epi16(_mm_shufflelo_epi16(brComponents, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
            __m128i result = _mm_or_si128(gaComponents, brSwapped);
            _mm_store_si128(reinterpret_cast<__m128i*>(&dest[x]), result);
        }

        // Perform leftover writes
        for(; x < width; x++)
        {
            unsigned int rgba = source[x];
            dest[x] = (_rotl(rgba, 16) & 0x00ff00ff) | (rgba & 0xff00ff00);
        }
    }
}

void Texture::loadRGBAUByteImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                     int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned int *source = NULL;
    unsigned int *dest = NULL;
    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned int*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4);

        for(int x = 0; x < width; x++)
        {
            unsigned int rgba = source[x];
            dest[x] = (_rotl(rgba, 16) & 0x00ff00ff) | (rgba & 0xff00ff00);
        }
    }
}

void Texture::loadRGBA4444ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                    int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4;
        for(int x = 0; x < width; x++)
        {
            unsigned short rgba = source[x];
            dest[4 * x + 0] = ((rgba & 0x00F0) << 0) | ((rgba & 0x00F0) >> 4);
            dest[4 * x + 1] = ((rgba & 0x0F00) >> 4) | ((rgba & 0x0F00) >> 8);
            dest[4 * x + 2] = ((rgba & 0xF000) >> 8) | ((rgba & 0xF000) >> 12);
            dest[4 * x + 3] = ((rgba & 0x000F) << 4) | ((rgba & 0x000F) >> 0);
        }
    }
}

void Texture::loadRGBA5551ImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                    int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned short *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4;
        for(int x = 0; x < width; x++)
        {
            unsigned short rgba = source[x];
            dest[4 * x + 0] = ((rgba & 0x003E) << 2) | ((rgba & 0x003E) >> 3);
            dest[4 * x + 1] = ((rgba & 0x07C0) >> 3) | ((rgba & 0x07C0) >> 8);
            dest[4 * x + 2] = ((rgba & 0xF800) >> 8) | ((rgba & 0xF800) >> 13);
            dest[4 * x + 3] = (rgba & 0x0001) ? 0xFF : 0;
        }
    }
}

void Texture::loadRGBAFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                     int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const float *source = NULL;
    float *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = reinterpret_cast<const float*>(static_cast<const unsigned char*>(input) + y * inputPitch);
        dest = reinterpret_cast<float*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 16);
        memcpy(dest, source, width * 16);
    }
}

void Texture::loadRGBAHalfFloatImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                        int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned char *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = static_cast<const unsigned char*>(input) + y * inputPitch;
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch  + xoffset * 8;
        memcpy(dest, source, width * 8);
    }
}

void Texture::loadBGRAImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    const unsigned char *source = NULL;
    unsigned char *dest = NULL;

    for(int y = 0; y < height; y++)
    {
        source = static_cast<const unsigned char*>(input) + y * inputPitch;
        dest = static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 4;
        memcpy(dest, source, width*4);
    }
}

void Texture::loadCompressedImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      int inputPitch, const void *input, size_t outputPitch, void *output) const
{
    ASSERT(xoffset % 4 == 0);
    ASSERT(yoffset % 4 == 0);
    ASSERT(width % 4 == 0 || width == 2 || width == 1);
    ASSERT(inputPitch % 8 == 0);
    ASSERT(outputPitch % 8 == 0);

    const unsigned int *source = reinterpret_cast<const unsigned int*>(input);
    unsigned int *dest = reinterpret_cast<unsigned int*>(output);

    switch (height)
    {
        case 1:
            // Round width up in case it is 1.
            for(int x = 0; x < (width + 1) / 2; x += 2)
            {
                // First 32-bits is two RGB565 colors shared by tile and does not need to be modified.
                dest[x] = source[x];

                // Second 32-bits contains 4 rows of 4 2-bit interpolants between the colors, the last 3 rows being unused. No flipping should occur.
                dest[x + 1] = source[x + 1];
            }
            break;
        case 2:
            // Round width up in case it is 1.
            for(int x = 0; x < (width + 1) / 2; x += 2)
            {
                // First 32-bits is two RGB565 colors shared by tile and does not need to be modified.
                dest[x] = source[x];

                // Second 32-bits contains 4 rows of 4 2-bit interpolants between the colors, the last 2 rows being unused. Only the top 2 rows should be flipped.
                dest[x + 1] = ((source[x + 1] << 8) & 0x0000FF00) |
                              ((source[x + 1] >> 8) & 0x000000FF);       
            }
            break;
        default:
            ASSERT(height % 4 == 0);
            for(int y = 0; y < height / 4; ++y)
            {
                const unsigned int *source = reinterpret_cast<const unsigned int*>(static_cast<const unsigned char*>(input) + y * inputPitch);
                unsigned int *dest = reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(output) + (y + yoffset) * outputPitch + xoffset * 8);

                // Round width up in case it is 1.
                for(int x = 0; x < (width + 1) / 2; x += 2)
                {
                    // First 32-bits is two RGB565 colors shared by tile and does not need to be modified.
                    dest[x] = source[x];

                    // Second 32-bits contains 4 rows of 4 2-bit interpolants between the colors. All rows should be flipped.
                    dest[x + 1] = (source[x + 1] >> 24) | 
                                  ((source[x + 1] << 8) & 0x00FF0000) |
                                  ((source[x + 1] >> 8) & 0x0000FF00) |
                                  (source[x + 1] << 24);                    
                }
            }
            break;
    }
}

void Texture::setImage(GLint unpackAlignment, const void *pixels, Image *image)
{
    if(pixels && image)
    {
        void *buffer = image->lock(0, 0, sw::LOCK_WRITEONLY);

        if(buffer)
        {
            loadImageData(0, 0, image->getWidth(), image->getHeight(), image->getFormat(), image->getType(), unpackAlignment, pixels, image->getPitch(), buffer, image);
            image->unlock();
        }
    }
}

void Texture::setCompressedImage(GLsizei imageSize, const void *pixels, Image *image)
{
    if(pixels && image)
    {
		void *buffer = image->lock(0, 0, sw::LOCK_WRITEONLY);

        if(buffer)
        {
            int inputPitch = ComputeCompressedPitch(image->getWidth(), image->getFormat());
            int inputSize = ComputeCompressedSize(image->getWidth(), image->getHeight(), image->getFormat());
            loadCompressedImageData(0, 0, image->getWidth(), image->getHeight(), -inputPitch, static_cast<const char*>(pixels) + inputSize - inputPitch, image->getPitch(), buffer);
            image->unlock();
        }
    }
}

void Texture::subImage(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels, Image *image)
{
	if(!image)
	{
		return error(GL_INVALID_OPERATION);
	}

    if(width + xoffset > image->getWidth() || height + yoffset > image->getHeight())
    {
        return error(GL_INVALID_VALUE);
    }

    if(IsCompressed(image->getFormat()))
    {
        return error(GL_INVALID_OPERATION);
    }

    if(format != image->getFormat())
    {
        return error(GL_INVALID_OPERATION);
    }

    if(pixels)
    {
		void *buffer = image->lock(0, 0, sw::LOCK_WRITEONLY);
        loadImageData(xoffset, transformPixelYOffset(yoffset, height, image->getHeight()), width, height, format, type, unpackAlignment, pixels, image->getPitch(), buffer, image);
		image->unlock();
    }
}

void Texture::subImageCompressed(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels, Image *image)
{
	if(!image)
	{
		return error(GL_INVALID_OPERATION);
	}

    if(width + xoffset > image->getWidth() || height + yoffset > image->getHeight())
    {
        return error(GL_INVALID_VALUE);
    }

    if(format != image->getFormat())
    {
        return error(GL_INVALID_OPERATION);
    }

    if(pixels)
    {
        void *buffer = image->lock(xoffset, yoffset, sw::LOCK_WRITEONLY);
        int inputPitch = ComputeCompressedPitch(width, format);
        int inputSize = ComputeCompressedSize(width, height, format);
        loadCompressedImageData(xoffset, transformPixelYOffset(yoffset, height, image->getHeight()), width, height, -inputPitch, static_cast<const char*>(pixels) + inputSize - inputPitch, image->getPitch(), buffer);
        image->unlock();
    }
}

bool Texture::copy(Image *source, const sw::Rect &sourceRect, GLenum destFormat, GLint xoffset, GLint yoffset, Image *dest)
{
    Device *device = getDevice();
	
    sw::Rect destRect = {xoffset, yoffset, xoffset + (sourceRect.right - sourceRect.left), yoffset + (sourceRect.bottom - sourceRect.top)};
    bool success = device->stretchRect(source, &sourceRect, dest, &destRect, false);

    if(!success)
    {
        return error(GL_OUT_OF_MEMORY, false);
    }

    return true;
}

Texture2D::Texture2D(GLuint id) : Texture(id)
{
	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		image[i] = 0;
	}

    mSurface = NULL;
}

Texture2D::~Texture2D()
{
	resource->lock(sw::DESTRUCT);

	for(int i = 0; i < MIPMAP_LEVELS; i++)
	{
		if(image[i])
		{
			image[i]->unbind();
			image[i] = 0;
		}
	}

	resource->unlock();

    mColorbufferProxy.set(NULL);

    if(mSurface)
    {
        mSurface->setBoundTexture(NULL);
        mSurface = NULL;
    }
}

bool Texture2D::isTexture2D()
{
	return true;
}

GLenum Texture2D::getTarget() const
{
    return GL_TEXTURE_2D;
}

GLsizei Texture2D::getWidth() const
{
    return image[0] ? image[0]->getWidth() : 0;
}

GLsizei Texture2D::getHeight() const
{
    return image[0] ? image[0]->getHeight() : 0;
}

GLenum Texture2D::getFormat() const
{
    return image[0] ? image[0]->getFormat() : 0;
}

GLenum Texture2D::getType() const
{
    return image[0] ? image[0]->getType() : 0;
}

sw::Format Texture2D::getInternalFormat() const
{
	return image[0] ? image[0]->getInternalFormat() : sw::FORMAT_NULL;
}

int Texture2D::getLevelCount() const
{
	int levels = 0;

	while(levels < MIPMAP_LEVELS && image[levels])
	{
		levels++;
	}

	return levels;
}

void Texture2D::setImage(GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels)
{
	if(image[level])
	{
		image[level]->unbind();
	}

	image[level] = new Image(getResource(), width, height, format, type);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	image[level]->bind();

    Texture::setImage(unpackAlignment, pixels, image[level]);
}

void Texture2D::bindTexImage(egl::Surface *surface)
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
			image[level]->unbind();
			image[level] = 0;
		}
	}

	image[0] = surface->getRenderTarget();
	image[0]->bind();

    mSurface = surface;
    mSurface->setBoundTexture(this);
}

void Texture2D::releaseTexImage()
{
    for(int level = 0; level < MIPMAP_LEVELS; level++)
	{
		if(image[level])
		{
			image[level]->unbind();
			image[level] = 0;
		}
	}
}

void Texture2D::setCompressedImage(GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels)
{
    Texture::setCompressedImage(imageSize, pixels, image[level]);
}

void Texture2D::subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels)
{
	Texture::subImage(xoffset, yoffset, width, height, format, type, unpackAlignment, pixels, image[level]);
}

void Texture2D::subImageCompressed(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels)
{
    Texture::subImageCompressed(xoffset, yoffset, width, height, format, imageSize, pixels, image[level]);
}

void Texture2D::copyImage(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
    Image *renderTarget = source->getRenderTarget();

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

	if(image[level])
	{
		image[level]->unbind();
	}

	image[level] = new Image(getResource(), width, height, format, GL_UNSIGNED_BYTE);

	if(!image[level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	image[level]->bind();

    if(width != 0 && height != 0)
    {
        sw::Rect sourceRect = transformPixelRect(x, y, width, height, source->getColorbuffer()->getHeight());
        sourceRect.left = clamp(sourceRect.left, 0, source->getColorbuffer()->getWidth());
        sourceRect.top = clamp(sourceRect.top, 0, source->getColorbuffer()->getHeight());
        sourceRect.right = clamp(sourceRect.right, 0, source->getColorbuffer()->getWidth());
        sourceRect.bottom = clamp(sourceRect.bottom, 0, source->getColorbuffer()->getHeight());

        GLint destYOffset = transformPixelYOffset(0, height, image[level]->getHeight());
        
        copy(source->getRenderTarget(), sourceRect, format, 0, destYOffset, image[level]);
    }
}

void Texture2D::copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	if(!image[level])
	{
		return error(GL_INVALID_OPERATION);
	}

    if(xoffset + width > image[level]->getWidth() || yoffset + height > image[level]->getHeight())
    {
        return error(GL_INVALID_VALUE);
    }

    Image *renderTarget = source->getRenderTarget();

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

    sw::Rect sourceRect = transformPixelRect(x, y, width, height, source->getColorbuffer()->getHeight());
    sourceRect.left = clamp(sourceRect.left, 0, source->getColorbuffer()->getWidth());
    sourceRect.top = clamp(sourceRect.top, 0, source->getColorbuffer()->getHeight());
    sourceRect.right = clamp(sourceRect.right, 0, source->getColorbuffer()->getWidth());
    sourceRect.bottom = clamp(sourceRect.bottom, 0, source->getColorbuffer()->getHeight());

    GLint destYOffset = transformPixelYOffset(yoffset, height, image[level]->getHeight());

	copy(source->getRenderTarget(), sourceRect, image[level]->getFormat(), xoffset, destYOffset, image[level]);
}

// Tests for GL texture object completeness. [OpenGL ES 2.0.24] section 3.7.10 page 81.
bool Texture2D::isComplete() const
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

    bool mipmapping = false;

    switch(mMinFilter)
    {
    case GL_NEAREST:
    case GL_LINEAR:
        mipmapping = false;
        break;
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
        mipmapping = true;
        break;
    default: UNREACHABLE();
    }

    if(mipmapping)
    {
        int q = log2(std::max(width, height));

        for(int level = 1; level <= q; level++)
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
    }

    return true;
}

bool Texture2D::isCompressed() const
{
    return IsCompressed(getFormat());
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
			image[i]->unbind();
		}

		image[i] = new Image(getResource(), std::max(image[0]->getWidth() >> i, 1), std::max(image[0]->getHeight() >> i, 1), image[0]->getFormat(), image[0]->getType());

		if(!image[i])
		{
			return error(GL_OUT_OF_MEMORY);
		}

		image[i]->bind();

		getDevice()->stretchRect(image[i - 1], 0, image[i], 0, true);
    }
}

Image *Texture2D::getImage(unsigned int level)
{
	return image[level];
}

Renderbuffer *Texture2D::getRenderbuffer(GLenum target)
{
    if(target != GL_TEXTURE_2D)
    {
        return error(GL_INVALID_OPERATION, (Renderbuffer *)NULL);
    }

    if(mColorbufferProxy.get() == NULL)
    {
        mColorbufferProxy.set(new Renderbuffer(id(), new Colorbuffer(this, target)));
    }

    return mColorbufferProxy.get();
}

Image *Texture2D::getRenderTarget(GLenum target)
{
    ASSERT(target == GL_TEXTURE_2D);

	if(image[0])
	{
		image[0]->addRef();
	}

	return image[0];
}

TextureCubeMap::TextureCubeMap(GLuint id) : Texture(id)
{
	for(int f = 0; f < 6; f++)
	{
		for(int i = 0; i < MIPMAP_LEVELS; i++)
		{
			image[f][i] = 0;
		}
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
				image[f][i]->unbind();
				image[f][i] = 0;
			}
		}
	}

	resource->unlock();

    for(int i = 0; i < 6; i++)
    {
        mFaceProxies[i].set(NULL);
    }
}

bool TextureCubeMap::isTextureCubeMap()
{
	return true;
}

GLenum TextureCubeMap::getTarget() const
{
    return GL_TEXTURE_CUBE_MAP;
}

GLsizei TextureCubeMap::getWidth() const
{
    return image[0][0] ? image[0][0]->getWidth() : 0;
}

GLsizei TextureCubeMap::getHeight() const
{
    return image[0][0] ? image[0][0]->getHeight() : 0;
}

GLenum TextureCubeMap::getFormat() const
{
    return image[0][0] ? image[0][0]->getFormat() : 0;
}

GLenum TextureCubeMap::getType() const
{
    return image[0][0] ? image[0][0]->getType() : 0;
}

sw::Format TextureCubeMap::getInternalFormat() const
{
    return image[0][0] ? image[0][0]->getInternalFormat() : sw::FORMAT_NULL;
}

int TextureCubeMap::getLevelCount() const
{
	int levels = 0;

	while(levels < MIPMAP_LEVELS && image[0][levels])
	{
		levels++;
	}

	return levels;
}

void TextureCubeMap::setCompressedImage(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels)
{
	CubeFace face = ConvertCubeFace(target);

	if(image[face][level])
	{
		image[face][level]->unbind();
	}

	image[face][level] = new Image(resource, width, height, format, GL_UNSIGNED_BYTE);

	if(!image[face][level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	image[face][level]->bind();

    Texture::setCompressedImage(imageSize, pixels, image[face][level]);
}

void TextureCubeMap::subImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels)
{
    Texture::subImage(xoffset, yoffset, width, height, format, type, unpackAlignment, pixels, image[ConvertCubeFace(target)][level]);
}

void TextureCubeMap::subImageCompressed(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *pixels)
{
    Texture::subImageCompressed(xoffset, yoffset, width, height, format, imageSize, pixels, image[ConvertCubeFace(target)][level]);
}

// Tests for GL texture object completeness. [OpenGL ES 2.0.24] section 3.7.10 page 81.
bool TextureCubeMap::isComplete() const
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

    bool mipmapping;

    switch (mMinFilter)
    {
    case GL_NEAREST:
    case GL_LINEAR:
        mipmapping = false;
        break;
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
        mipmapping = true;
        break;
    default: UNREACHABLE();
    }

    for(int face = 0; face < 6; face++)
    {
        if(image[face][0]->getWidth() != size || image[face][0]->getHeight() != size)
        {
            return false;
        }
    }

    if(mipmapping)
    {
        int q = log2(size);

        for(int face = 0; face < 6; face++)
        {
            for(int level = 1; level <= q; level++)
            {
				if(!image[face][level])
				{
					return false;
				}

                if(image[face][level]->getFormat() != image[0][0]->getFormat())
                {
                    return false;
                }

                if(image[face][level]->getType() != image[0][0]->getType())
                {
                    return false;
                }

                if(image[face][level]->getWidth() != std::max(1, size >> level))
                {
                    return false;
                }

                ASSERT(image[face][level]->getHeight() == image[face][level]->getWidth());
            }
        }
    }

    return true;
}

bool TextureCubeMap::isCompressed() const
{
    return IsCompressed(getFormat());
}

void TextureCubeMap::setImage(GLenum target, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *pixels)
{
	CubeFace face = ConvertCubeFace(target);

	if(image[face][level])
	{
		image[face][level]->unbind();
	}

	image[face][level] = new Image(resource, width, height, format, GL_UNSIGNED_BYTE);

	if(!image[face][level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	image[face][level]->bind();

    Texture::setImage(unpackAlignment, pixels, image[face][level]);
}

void TextureCubeMap::copyImage(GLenum target, GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	Image *renderTarget = source->getRenderTarget();

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

	CubeFace face = ConvertCubeFace(target);

	if(image[face][level])
	{
		image[face][level]->unbind();
	}

	image[face][level] = new Image(resource, width, height, format, GL_UNSIGNED_BYTE);

	if(!image[face][level])
	{
		return error(GL_OUT_OF_MEMORY);
	}

	image[face][level]->bind();

    if(width != 0 && height != 0)
    {
        sw::Rect sourceRect = transformPixelRect(x, y, width, height, source->getColorbuffer()->getHeight());
        sourceRect.left = clamp(sourceRect.left, 0, source->getColorbuffer()->getWidth());
        sourceRect.top = clamp(sourceRect.top, 0, source->getColorbuffer()->getHeight());
        sourceRect.right = clamp(sourceRect.right, 0, source->getColorbuffer()->getWidth());
        sourceRect.bottom = clamp(sourceRect.bottom, 0, source->getColorbuffer()->getHeight());

        GLint destYOffset = transformPixelYOffset(0, height, image[face][level]->getHeight());
        
        copy(source->getRenderTarget(), sourceRect, format, 0, destYOffset, image[face][level]);
    }
}

Image *TextureCubeMap::getImage(CubeFace face, unsigned int level)
{
	return image[face][level];
}

Image *TextureCubeMap::getImage(GLenum face, unsigned int level)
{
    return image[ConvertCubeFace(face)][level];
}

void TextureCubeMap::copySubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height, Framebuffer *source)
{
	CubeFace face = ConvertCubeFace(target);

	if(!image[face][level])
	{
		return error(GL_INVALID_OPERATION);
	}

    GLsizei size = image[face][level]->getWidth();

    if(xoffset + width > size || yoffset + height > size)
    {
        return error(GL_INVALID_VALUE);
    }

     Image *renderTarget = source->getRenderTarget();

    if(!renderTarget)
    {
        ERR("Failed to retrieve the render target.");
        return error(GL_OUT_OF_MEMORY);
    }

    sw::Rect sourceRect = transformPixelRect(x, y, width, height, source->getColorbuffer()->getHeight());
    sourceRect.left = clamp(sourceRect.left, 0, source->getColorbuffer()->getWidth());
    sourceRect.top = clamp(sourceRect.top, 0, source->getColorbuffer()->getHeight());
    sourceRect.right = clamp(sourceRect.right, 0, source->getColorbuffer()->getWidth());
    sourceRect.bottom = clamp(sourceRect.bottom, 0, source->getColorbuffer()->getHeight());

    GLint destYOffset = transformPixelYOffset(yoffset, height, image[face][level]->getHeight());

	copy(source->getRenderTarget(), sourceRect, image[face][level]->getFormat(), xoffset, destYOffset, image[face][level]);
}

bool TextureCubeMap::isCubeComplete() const
{
	for(unsigned int f = 1; f < 6; f++)
    {
		if(!image[f][0])
		{
			return false;
		}
	}

    if(image[0][0]->getWidth() == 0)
    {
        return false;
    }

    for(unsigned int f = 1; f < 6; f++)
    {
        if(image[f][0]->getWidth() != image[0][0]->getWidth() || image[f][0]->getFormat() != image[0][0]->getFormat())
        {
            return false;
        }
    }

    return true;
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
				image[f][i]->unbind();
			}

			image[f][i] = new Image(resource, std::max(image[0][0]->getWidth() >> i, 1), std::max(image[0][0]->getHeight() >> i, 1), image[0][0]->getFormat(), image[0][0]->getType());

			if(!image[f][i])
			{
				return error(GL_OUT_OF_MEMORY);
			}

			image[f][i]->bind();

			getDevice()->stretchRect(image[f][i - 1], 0, image[f][i], 0, true);
		}
	}
}

Renderbuffer *TextureCubeMap::getRenderbuffer(GLenum target)
{
    if(!IsCubemapTextureTarget(target))
    {
        return error(GL_INVALID_OPERATION, (Renderbuffer *)NULL);
    }

    unsigned int face = ConvertCubeFace(target);

    if(mFaceProxies[face].get() == NULL)
    {
        mFaceProxies[face].set(new Renderbuffer(id(), new Colorbuffer(this, target)));
    }

    return mFaceProxies[face].get();
}

Image *TextureCubeMap::getRenderTarget(GLenum target)
{
    ASSERT(IsCubemapTextureTarget(target));
    
	int face = ConvertCubeFace(target);

	if(image[face][0])
	{
		image[face][0]->addRef();
	}

	return image[face][0];
}

}

extern "C"
{
	gl::Image *createBackBuffer(int width, int height, const egl::Config *config)
	{
		if(config)
		{
			return new gl::Image(0, width, height, config->mAlphaSize ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE);
		}

		return 0;
	}
}