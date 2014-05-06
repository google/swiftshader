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

#include "Device.hpp"

#include "Image.hpp"
#include "Texture.h"

#include "Renderer/Renderer.hpp"
#include "Renderer/Clipper.hpp"
#include "Shader/PixelShader.hpp"
#include "Shader/VertexShader.hpp"
#include "Main/Config.hpp"
#include "Main/FrameBuffer.hpp"
#include "Common/Math.hpp"
#include "Common/Configurator.hpp"
#include "Common/Timer.hpp"
#include "../common/debug.h"

bool localShaderConstants = false;

namespace gl
{
	using namespace sw;

	Device::Device(Context *context) : Renderer(context, true, true, true, true, true), context(context)
	{
		depthStencil = 0;
		renderTarget = 0;

		setDepthBufferEnable(true);
		setFillMode(Context::FILL_SOLID);
		setShadingMode(Context::SHADING_GOURAUD);
		setDepthWriteEnable(true);
		setAlphaTestEnable(false);
		setSourceBlendFactor(Context::BLEND_ONE);
		setDestBlendFactor(Context::BLEND_ZERO);
		setCullMode(Context::CULL_COUNTERCLOCKWISE);
		setDepthCompare(Context::DEPTH_LESSEQUAL);
		setAlphaReference(0);
		setAlphaCompare(Context::ALPHA_ALWAYS);
		setAlphaBlendEnable(false);
		setFogEnable(false);
		setSpecularEnable(false);
		setFogColor(0);
		setPixelFogMode(Context::FOG_NONE);
		setFogStart(0.0f);
		setFogEnd(1.0f);
		setFogDensity(1.0f);
		setRangeFogEnable(false);
		setStencilEnable(false);
		setStencilFailOperation(Context::OPERATION_KEEP);
		setStencilZFailOperation(Context::OPERATION_KEEP);
		setStencilPassOperation(Context::OPERATION_KEEP);
		setStencilCompare(Context::STENCIL_ALWAYS);
		setStencilReference(0);
		setStencilMask(0xFFFFFFFF);
		setStencilWriteMask(0xFFFFFFFF);
		setVertexFogMode(Context::FOG_NONE);
		setClipFlags(0);
		setPointSize(1.0f);
		setPointSizeMin(0.125f);
		setPointSpriteEnable(false);
        setPointSizeMax(8192.0f);
		setColorWriteMask(0, 0x0000000F);
		setBlendOperation(Context::BLENDOP_ADD);
		scissorEnable = false;
		setSlopeDepthBias(0.0f);
		setTwoSidedStencil(false);
		setStencilFailOperationCCW(Context::OPERATION_KEEP);
		setStencilZFailOperationCCW(Context::OPERATION_KEEP);
		setStencilPassOperationCCW(Context::OPERATION_KEEP);
		setStencilCompareCCW(Context::STENCIL_ALWAYS);
		setColorWriteMask(1, 0x0000000F);
		setColorWriteMask(2, 0x0000000F);
		setColorWriteMask(3, 0x0000000F);
		setBlendConstant(0xFFFFFFFF);
		setWriteSRGB(false);
		setDepthBias(0.0f);
		setSeparateAlphaBlendEnable(false);
		setSourceBlendFactorAlpha(Context::BLEND_ONE);
		setDestBlendFactorAlpha(Context::BLEND_ZERO);
		setBlendOperationAlpha(Context::BLENDOP_ADD);

		for(int i = 0; i < 16; i++)
		{
			setAddressingModeU(sw::SAMPLER_PIXEL, i, ADDRESSING_WRAP);
			setAddressingModeV(sw::SAMPLER_PIXEL, i, ADDRESSING_WRAP);
			setAddressingModeW(sw::SAMPLER_PIXEL, i, ADDRESSING_WRAP);
			setBorderColor(sw::SAMPLER_PIXEL, i, 0x00000000);
			setTextureFilter(sw::SAMPLER_PIXEL, i, FILTER_POINT);
			setMipmapFilter(sw::SAMPLER_PIXEL, i, MIPMAP_NONE);
			setMipmapLOD(sw::SAMPLER_PIXEL, i, 0.0f);
		}

		for(int i = 0; i < 4; i++)
		{
			setAddressingModeU(sw::SAMPLER_VERTEX, i, ADDRESSING_WRAP);
			setAddressingModeV(sw::SAMPLER_VERTEX, i, ADDRESSING_WRAP);
			setAddressingModeW(sw::SAMPLER_VERTEX, i, ADDRESSING_WRAP);
			setBorderColor(sw::SAMPLER_VERTEX, i, 0x00000000);
			setTextureFilter(sw::SAMPLER_VERTEX, i, FILTER_POINT);
			setMipmapFilter(sw::SAMPLER_VERTEX, i, MIPMAP_NONE);
			setMipmapLOD(sw::SAMPLER_VERTEX, i, 0.0f);
		}

		for(int i = 0; i < 6; i++)
		{
			float plane[4] = {0, 0, 0, 0};

			setClipPlane(i, plane);
		}

		pixelShader = 0;
		vertexShader = 0;

		pixelShaderDirty = true;
		pixelShaderConstantsFDirty = 0;
		vertexShaderDirty = true;
		vertexShaderConstantsFDirty = 0;

		for(int i = 0; i < 224; i++)
		{
			float zero[4] = {0, 0, 0, 0};

			setPixelShaderConstantF(i, zero, 1);
		}

		for(int i = 0; i < 256; i++)
		{
			float zero[4] = {0, 0, 0, 0};

			setVertexShaderConstantF(i, zero, 1);
		}
	}

	Device::~Device()
	{		
		if(depthStencil)
		{
			depthStencil->unbind();
			depthStencil = 0;
		}
		
		if(renderTarget)
		{
			renderTarget->unbind();
			renderTarget = 0;
		}

		delete context;
	}

	void Device::clearColor(unsigned int color, unsigned int rgbaMask)
	{
		TRACE("unsigned long color = 0x%0.8X", color);

		int x0 = 0;
		int y0 = 0;
		int width = renderTarget->getExternalWidth();
		int height = renderTarget->getExternalHeight();

		if(scissorEnable)   // Clamp against scissor rectangle
		{
			if(x0 < scissorRect.x0) x0 = scissorRect.x0;
			if(y0 < scissorRect.y0) y0 = scissorRect.y0;
			if(width > scissorRect.x1 - scissorRect.x0) width = scissorRect.x1 - scissorRect.x0;
			if(height > scissorRect.y1 - scissorRect.y0) height = scissorRect.y1 - scissorRect.y0;
		}

		renderTarget->clearColorBuffer(color, rgbaMask, x0, y0, width, height);
	}

	void Device::clearDepth(float z)
	{
		TRACE("float z = %f", z);

		if(!depthStencil)
		{
			return;
		}

		if(z > 1) z = 1;
		if(z < 0) z = 0;

		int x0 = 0;
		int y0 = 0;
		int width = depthStencil->getExternalWidth();
		int height = depthStencil->getExternalHeight();

		if(scissorEnable)   // Clamp against scissor rectangle
		{
			if(x0 < scissorRect.x0) x0 = scissorRect.x0;
			if(y0 < scissorRect.y0) y0 = scissorRect.y0;
			if(width > scissorRect.x1 - scissorRect.x0) width = scissorRect.x1 - scissorRect.x0;
			if(height > scissorRect.y1 - scissorRect.y0) height = scissorRect.y1 - scissorRect.y0;
		}
			
		depthStencil->clearDepthBuffer(z, x0, y0, width, height);
	}

	void Device::clearStencil(unsigned int stencil, unsigned int mask)
	{
		TRACE("unsigned long stencil = %d", stencil);

		if(!depthStencil)
		{
			return;
		}

		int x0 = 0;
		int y0 = 0;
		int width = renderTarget->getExternalWidth();
		int height = renderTarget->getExternalHeight();

		if(scissorEnable)   // Clamp against scissor rectangle
		{
			if(x0 < scissorRect.x0) x0 = scissorRect.x0;
			if(y0 < scissorRect.y0) y0 = scissorRect.y0;
			if(width > scissorRect.x1 - scissorRect.x0) width = scissorRect.x1 - scissorRect.x0;
			if(height > scissorRect.y1 - scissorRect.y0) height = scissorRect.y1 - scissorRect.y0;
		}

		depthStencil->clearStencilBuffer(stencil, mask, x0, y0, width, height);
	}

	Image *Device::createDepthStencilSurface(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard)
	{
		TRACE("unsigned int width = %d, unsigned int height = %d, sw::Format format = %d, int multiSampleDepth = %d, bool discard = %d", width, height, format, multiSampleDepth, discard);

		if(width == 0 || height == 0 || height > OUTLINE_RESOLUTION)
		{
			ERR("Invalid parameters");
			return 0;
		}
		
		bool lockable = true;

		switch(format)
		{
	//	case FORMAT_D15S1:
		case FORMAT_D24S8:
		case FORMAT_D24X8:
	//	case FORMAT_D24X4S4:
		case FORMAT_D24FS8:
		case FORMAT_D32:
		case FORMAT_D16:
			lockable = false;
			break;
	//	case FORMAT_S8_LOCKABLE:
	//	case FORMAT_D16_LOCKABLE:
		case FORMAT_D32F_LOCKABLE:
	//	case FORMAT_D32_LOCKABLE:
		case FORMAT_DF24:
		case FORMAT_DF16:
			lockable = true;
			break;
		default:
			UNREACHABLE();
		}

		Image *surface = new Image(0, width, height, format, GL_NONE, GL_NONE, multiSampleDepth, lockable, true);

		if(!surface)
		{
			ERR("Out of memory");
			return 0;
		}

		surface->addRef();

		return surface;
	}

	Image *Device::createOffscreenPlainSurface(unsigned int width, unsigned int height, sw::Format format)
	{
		TRACE("unsigned int width = %d, unsigned int height = %d, sw::Format format = %d", width, height, format);
		
		Image *surface = new Image(0, width, height, format, GL_NONE, GL_NONE, 1, true, false);

		if(!surface)
		{
			ERR("Out of memory");
			return 0;
		}

		surface->addRef();

		return surface;
	}

	Image *Device::createRenderTarget(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool lockable)
	{
		TRACE("unsigned int width = %d, unsigned int height = %d, sw::Format format = %d, int multiSampleDepth = %d, bool lockable = %d", width, height, format, multiSampleDepth, lockable);

		if(height > OUTLINE_RESOLUTION)
		{
			ERR("Invalid parameters");
			return 0;
		}

		Image *surface = new Image(0, width, height, format, GL_NONE, GL_NONE, multiSampleDepth, lockable != FALSE, true);

		if(!surface)
		{
			ERR("Out of memory");
			return 0;
		}

		surface->addRef();
		
		return surface;
	}

	void Device::drawIndexedPrimitive(PrimitiveType type, unsigned int indexOffset, unsigned int primitiveCount, int indexSize)
	{
		TRACE("");

		if(!bindResources() || !primitiveCount)
		{
			return;
		}

		Context::DrawType drawType;

		if(indexSize == 4)
		{
			switch(type)
			{
			case DRAW_POINTLIST:     drawType = Context::DRAW_INDEXEDPOINTLIST32;     break;
			case DRAW_LINELIST:      drawType = Context::DRAW_INDEXEDLINELIST32;      break;
			case DRAW_LINESTRIP:     drawType = Context::DRAW_INDEXEDLINESTRIP32;     break;
			case DRAW_LINELOOP:      drawType = Context::DRAW_INDEXEDLINELOOP32;      break;
			case DRAW_TRIANGLELIST:  drawType = Context::DRAW_INDEXEDTRIANGLELIST32;  break;
			case DRAW_TRIANGLESTRIP: drawType = Context::DRAW_INDEXEDTRIANGLESTRIP32; break;
			case DRAW_TRIANGLEFAN:   drawType = Context::DRAW_INDEXEDTRIANGLEFAN32;	  break;
			default: UNREACHABLE();
			}
		}
		else if(indexSize == 2)
		{
			switch(type)
			{
			case DRAW_POINTLIST:     drawType = Context::DRAW_INDEXEDPOINTLIST16;     break;
			case DRAW_LINELIST:      drawType = Context::DRAW_INDEXEDLINELIST16;      break;
			case DRAW_LINESTRIP:     drawType = Context::DRAW_INDEXEDLINESTRIP16;     break;
			case DRAW_LINELOOP:      drawType = Context::DRAW_INDEXEDLINELOOP16;      break;
			case DRAW_TRIANGLELIST:  drawType = Context::DRAW_INDEXEDTRIANGLELIST16;  break;
			case DRAW_TRIANGLESTRIP: drawType = Context::DRAW_INDEXEDTRIANGLESTRIP16; break;
			case DRAW_TRIANGLEFAN:   drawType = Context::DRAW_INDEXEDTRIANGLEFAN16;   break;
			default: UNREACHABLE();
			}
		}
		else if(indexSize == 1)
		{
			switch(type)
			{
			case DRAW_POINTLIST:     drawType = Context::DRAW_INDEXEDPOINTLIST8;     break;
			case DRAW_LINELIST:      drawType = Context::DRAW_INDEXEDLINELIST8;      break;
			case DRAW_LINESTRIP:     drawType = Context::DRAW_INDEXEDLINESTRIP8;     break;
			case DRAW_LINELOOP:      drawType = Context::DRAW_INDEXEDLINELOOP8;      break;
			case DRAW_TRIANGLELIST:  drawType = Context::DRAW_INDEXEDTRIANGLELIST8;  break;
			case DRAW_TRIANGLESTRIP: drawType = Context::DRAW_INDEXEDTRIANGLESTRIP8; break;
			case DRAW_TRIANGLEFAN:   drawType = Context::DRAW_INDEXEDTRIANGLEFAN8;   break;
			default: UNREACHABLE();
			}
		}
		else UNREACHABLE();

		draw(drawType, indexOffset, primitiveCount);
	}

	void Device::drawPrimitive(PrimitiveType primitiveType, unsigned int primitiveCount)
	{
		TRACE("");

		if(!bindResources() || !primitiveCount)
		{
			return;
		}

		setIndexBuffer(0);
		
		Context::DrawType drawType;

		switch(primitiveType)
		{
		case DRAW_POINTLIST:     drawType = Context::DRAW_POINTLIST;     break;
		case DRAW_LINELIST:      drawType = Context::DRAW_LINELIST;      break;
		case DRAW_LINESTRIP:     drawType = Context::DRAW_LINESTRIP;     break;
		case DRAW_LINELOOP:      drawType = Context::DRAW_LINELOOP;      break;
		case DRAW_TRIANGLELIST:  drawType = Context::DRAW_TRIANGLELIST;  break;
		case DRAW_TRIANGLESTRIP: drawType = Context::DRAW_TRIANGLESTRIP; break;
		case DRAW_TRIANGLEFAN:   drawType = Context::DRAW_TRIANGLEFAN;   break;
		default: UNREACHABLE();
		}

		draw(drawType, 0, primitiveCount);
	}

	Image *Device::getDepthStencilSurface()
	{
		TRACE("void");

		if(depthStencil)
		{
			depthStencil->addRef();
		}

		return depthStencil;
	}

	void Device::setDepthStencilSurface(Image *depthStencil)
	{
		TRACE("Image *newDepthStencil = 0x%0.8p", depthStencil);

		if(this->depthStencil == depthStencil)
		{
			return;
		}

		if(depthStencil)
		{
			depthStencil->bind();
		}

		if(this->depthStencil)
		{
			this->depthStencil->unbind();
		}

		this->depthStencil = depthStencil;

		setDepthStencil(depthStencil);
	}

	void Device::setPixelShader(PixelShader *pixelShader)
	{
		TRACE("PixelShader *shader = 0x%0.8p", pixelShader);

		this->pixelShader = pixelShader;
		pixelShaderDirty = true;
	}

	void Device::setPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count)
	{
		TRACE("unsigned int startRegister = %d, const int *constantData = 0x%0.8p, unsigned int count = %d", startRegister, constantData, count);

		for(unsigned int i = 0; i < count && startRegister + i < 224; i++)
		{
			pixelShaderConstantF[startRegister + i][0] = constantData[i * 4 + 0];
			pixelShaderConstantF[startRegister + i][1] = constantData[i * 4 + 1];
			pixelShaderConstantF[startRegister + i][2] = constantData[i * 4 + 2];
			pixelShaderConstantF[startRegister + i][3] = constantData[i * 4 + 3];
		}

		pixelShaderConstantsFDirty = max(startRegister + count, pixelShaderConstantsFDirty);
		pixelShaderDirty = true;   // Reload DEF constants
	}

	void Device::setScissorEnable(bool enable)
	{
		scissorEnable = enable;
	}

	void Device::setRenderTarget(Image *iRenderTarget)
	{
		TRACE("Image *newRenderTarget = 0x%0.8p", iRenderTarget);

		Image *renderTarget = static_cast<Image*>(iRenderTarget);

		if(renderTarget)
		{
			renderTarget->bind();
		}

		if(this->renderTarget)
		{
			this->renderTarget->unbind();
		}

		this->renderTarget = renderTarget;

		Renderer::setRenderTarget(0, renderTarget);
	}

	void Device::setScissorRect(const sw::Rect &rect)
	{
		TRACE("const sw::Rect *rect = 0x%0.8p", rect);

		scissorRect = rect;
	}

	void Device::setVertexShader(VertexShader *vertexShader)
	{
		TRACE("VertexShader *shader = 0x%0.8p", vertexShader);

		this->vertexShader = vertexShader;
		vertexShaderDirty = true;
	}

	void Device::setVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count)
	{
		TRACE("unsigned int startRegister = %d, const int *constantData = 0x%0.8p, unsigned int count = %d", startRegister, constantData, count);

		for(unsigned int i = 0; i < count && startRegister + i < 256; i++)
		{
			vertexShaderConstantF[startRegister + i][0] = constantData[i * 4 + 0];
			vertexShaderConstantF[startRegister + i][1] = constantData[i * 4 + 1];
			vertexShaderConstantF[startRegister + i][2] = constantData[i * 4 + 2];
			vertexShaderConstantF[startRegister + i][3] = constantData[i * 4 + 3];
		}
			
		vertexShaderConstantsFDirty = max(startRegister + count, vertexShaderConstantsFDirty);
		vertexShaderDirty = true;   // Reload DEF constants
	}

	void Device::setViewport(const Viewport &viewport)
	{
		TRACE("const Viewport *viewport = 0x%0.8p", viewport);

		this->viewport = viewport;
	}

	bool Device::stretchRect(Image *sourceSurface, const sw::Rect *sourceRect, Image *destSurface, const sw::Rect *destRect, bool filter)
	{
		TRACE("Image *sourceSurface = 0x%0.8p, const sw::Rect *sourceRect = 0x%0.8p, Image *destSurface = 0x%0.8p, const sw::Rect *destRect = 0x%0.8p, bool filter = %d", sourceSurface, sourceRect, destSurface, destRect, filter);

		if(!sourceSurface || !destSurface || !validRectangle(sourceRect, sourceSurface) || !validRectangle(destRect, destSurface))
		{
			ERR("Invalid parameters");
			return false;
		}
		
		Image *source = static_cast<Image*>(sourceSurface);
		Image *dest = static_cast<Image*>(destSurface);

		int sWidth = source->getExternalWidth();
		int sHeight = source->getExternalHeight();
		int dWidth = dest->getExternalWidth();
		int dHeight = dest->getExternalHeight();

		Rect sRect;
		Rect dRect;

		if(sourceRect)
		{
			sRect = *sourceRect;
		}
		else
		{
			sRect.y0 = 0;
			sRect.x0 = 0;
			sRect.y1 = sHeight;
			sRect.x1 = sWidth;
		}

		if(destRect)
		{
			dRect = *destRect;
		}
		else
		{
			dRect.y0 = 0;
			dRect.x0 = 0;
			dRect.y1 = dHeight;
			dRect.x1 = dWidth;
		}

		bool scaling = (sRect.x1 - sRect.x0 != dRect.x1 - dRect.x0) || (sRect.y1 - sRect.y0 != dRect.y1 - dRect.y0);
		bool equalFormats = source->getInternalFormat() == dest->getInternalFormat();
		bool depthStencil = Image::isDepth(source->getInternalFormat()) || Image::isStencil(source->getInternalFormat());
		bool alpha0xFF = false;

		if((source->getInternalFormat() == FORMAT_A8R8G8B8 && dest->getInternalFormat() == FORMAT_X8R8G8B8) ||
		   (source->getInternalFormat() == FORMAT_X8R8G8B8 && dest->getInternalFormat() == FORMAT_A8R8G8B8))
		{
			equalFormats = true;
			alpha0xFF = true;
		}

		if(depthStencil)   // Copy entirely, internally   // FIXME: Check
		{
			if(source->hasDepth())
			{
				sw::byte *sourceBuffer = (sw::byte*)source->lockInternal(0, 0, 0, LOCK_READONLY, PUBLIC);
				sw::byte *destBuffer = (sw::byte*)dest->lockInternal(0, 0, 0, LOCK_DISCARD, PUBLIC);

				unsigned int width = source->getInternalWidth();
				unsigned int height = source->getInternalHeight();
				unsigned int pitch = source->getInternalPitchB();

				for(unsigned int y = 0; y < height; y++)
				{
					memcpy(destBuffer, sourceBuffer, pitch);   // FIXME: Only copy width * bytes

					sourceBuffer += pitch;
					destBuffer += pitch;
				}

				source->unlockInternal();
				dest->unlockInternal();
			}

			if(source->hasStencil())
			{
				sw::byte *sourceBuffer = (sw::byte*)source->lockStencil(0, PUBLIC);
				sw::byte *destBuffer = (sw::byte*)dest->lockStencil(0, PUBLIC);

				unsigned int width = source->getInternalWidth();
				unsigned int height = source->getInternalHeight();
				unsigned int pitch = source->getStencilPitchB();

				for(unsigned int y = 0; y < height; y++)
				{
					memcpy(destBuffer, sourceBuffer, pitch);   // FIXME: Only copy width * bytes

					sourceBuffer += pitch;
					destBuffer += pitch;
				}

				source->unlockStencil();
				dest->unlockStencil();
			}
		}
		else if(!scaling && equalFormats)
		{
			unsigned char *sourceBytes = (unsigned char*)source->lockInternal(sRect.x0, sRect.y0, 0, LOCK_READONLY, PUBLIC);
			unsigned char *destBytes = (unsigned char*)dest->lockInternal(dRect.x0, dRect.y0, 0, LOCK_READWRITE, PUBLIC);
			unsigned int sourcePitch = source->getInternalPitchB();
			unsigned int destPitch = dest->getInternalPitchB();

			unsigned int width = dRect.x1 - dRect.x0;
			unsigned int height = dRect.y1 - dRect.y0;
			unsigned int bytes = width * Image::bytes(source->getInternalFormat());

			for(unsigned int y = 0; y < height; y++)
			{
				memcpy(destBytes, sourceBytes, bytes);

				if(alpha0xFF)
				{
					for(unsigned int x = 0; x < width; x++)
					{
						destBytes[4 * x + 3] = 0xFF;
					}
				}
				
				sourceBytes += sourcePitch;
				destBytes += destPitch;
			}

			source->unlockInternal();
			dest->unlockInternal();
		}
		else
		{
			blit(source, sRect, dest, dRect, scaling && filter);
		}

		return true;
	}

	bool Device::updateSurface(Image *sourceSurface, const sw::Rect *sourceRect, Image *destinationSurface, const POINT *destPoint)
	{
		TRACE("Image *sourceSurface = 0x%0.8p, const sw::Rect *sourceRect = 0x%0.8p, Image *destinationSurface = 0x%0.8p, const POINT *destPoint = 0x%0.8p", sourceSurface, sourceRect, destinationSurface, destPoint);

		if(!sourceSurface || !destinationSurface)
		{
			ERR("Invalid parameters");
			return false;
		}

		Rect sRect;
		Rect dRect;
		
		if(sourceRect)
		{
			sRect.x0 = sourceRect->x0;
			sRect.y0 = sourceRect->y0;
			sRect.x1 = sourceRect->x1;
			sRect.y1 = sourceRect->y1;
		}
		else
		{
			sRect.x0 = 0;
			sRect.y0 = 0;
			sRect.x1 = sourceSurface->getWidth();
			sRect.y1 = sourceSurface->getHeight();
		}

		if(destPoint)
		{
			dRect.x0 = destPoint->x;
			dRect.y0 = destPoint->y;
			dRect.x1 = destPoint->x + sRect.x1 - sRect.x0;
			dRect.y1 = destPoint->y + sRect.y1 - sRect.y0;
		}
		else
		{
			dRect.x0 = 0;
			dRect.y0 = 0;
			dRect.x1 = sRect.x1 - sRect.x0;
			dRect.y1 = sRect.y1 - sRect.y0;
		}

		if(!validRectangle(&sRect, sourceSurface) || !validRectangle(&dRect, destinationSurface))
		{
			ERR("Invalid parameters");
			return false;
		}

		int sWidth = sRect.x1 - sRect.x0;
		int sHeight = sRect.y1 - sRect.y0;

		int dWidth = dRect.x1 - dRect.x0;
		int dHeight = dRect.y1 - dRect.y0;

		if(sourceSurface->getMultiSampleDepth() > 1 ||
		   destinationSurface->getMultiSampleDepth() > 1 ||
		   sourceSurface->getInternalFormat() != destinationSurface->getInternalFormat())
		{
			ERR("Invalid parameters");
			return false;
		}
		
		unsigned char *sourceBuffer = (unsigned char*)sourceSurface->lock(sRect.x0, sRect.y0, LOCK_READONLY);
		unsigned char *destinationBuffer = (unsigned char*)destinationSurface->lock(dRect.x0, dRect.y0, LOCK_WRITEONLY);

		unsigned int width;
		unsigned int height;
		unsigned int bytes;

		switch(sourceSurface->getInternalFormat())
		{
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		case FORMAT_ATI1:
			width = (dWidth + 3) / 4;
			height = (dHeight + 3) / 4;
			bytes = width * 8;   // 64 bit per 4x4 block
			break;
	//	case FORMAT_DXT2:
		case FORMAT_DXT3:
	//	case FORMAT_DXT4:
		case FORMAT_DXT5:
		case FORMAT_ATI2:
			width = (dWidth + 3) / 4;
			height = (dHeight + 3) / 4;
			bytes = width * 16;   // 128 bit per 4x4 block
			break;
		#endif
		default:
			width = dWidth;
			height = dHeight;
			bytes = width * Image::bytes(sourceSurface->getInternalFormat());
		}

		int sourcePitch = sourceSurface->getPitch();
		int destinationPitch = destinationSurface->getPitch();

		#if S3TC_SUPPORT
		if(sourceSurface->getInternalFormat() == FORMAT_ATI1 || sourceSurface->getInternalFormat() == FORMAT_ATI2)
		{
			// Make the pitch correspond to 4 rows
			sourcePitch *= 4;
			destinationPitch *= 4;
		}
		#endif

		for(unsigned int y = 0; y < height; y++)
		{
			memcpy(destinationBuffer, sourceBuffer, bytes);
			
			sourceBuffer += sourcePitch;
			destinationBuffer += destinationPitch;
		}

		sourceSurface->unlock();
		destinationSurface->unlock();
		
		return true;
	}

	bool Device::bindResources()
	{
		if(!bindViewport())
		{
			return false;   // Zero-area target region
		}

		bindShaderConstants();

		return true;
	}

	void Device::bindShaderConstants()
	{
		if(pixelShaderDirty)
		{
			if(pixelShader)
			{
				if(pixelShaderConstantsFDirty)
				{
					Renderer::setPixelShaderConstantF(0, pixelShaderConstantF[0], pixelShaderConstantsFDirty);
				}

				Renderer::setPixelShader(pixelShader);   // Loads shader constants set with DEF
				pixelShaderConstantsFDirty = pixelShader->dirtyConstantsF;   // Shader DEF'ed constants are dirty
			}
			else
			{
				setPixelShader(0);
			}

			pixelShaderDirty = false;
		}

		if(vertexShaderDirty)
		{
			if(vertexShader)
			{
				if(vertexShaderConstantsFDirty)
				{
					Renderer::setVertexShaderConstantF(0, vertexShaderConstantF[0], vertexShaderConstantsFDirty);
				}
		
				Renderer::setVertexShader(vertexShader);   // Loads shader constants set with DEF
				vertexShaderConstantsFDirty = vertexShader->dirtyConstantsF;   // Shader DEF'ed constants are dirty
			}
			else
			{
				setVertexShader(0);
			}

			vertexShaderDirty = false;
		}
	}
	
	bool Device::bindViewport()
	{
		if(viewport.width <= 0 || viewport.height <= 0)
		{
			return false;
		}

		if(scissorEnable)
		{
			if(scissorRect.x0 >= scissorRect.x1 || scissorRect.y0 >= scissorRect.y1)
			{
				return false;
			}

			sw::Rect scissor;
			scissor.x0 = scissorRect.x0;
			scissor.x1 = scissorRect.x1;
			scissor.y0 = scissorRect.y0;
			scissor.y1 = scissorRect.y1;
			
			setScissor(scissor);
		}
		else
		{
			sw::Rect scissor;
			scissor.x0 = max(viewport.x0, 0);
			scissor.x1 = min(viewport.x0 + viewport.width, renderTarget->getExternalWidth());
			scissor.y0 = max(viewport.y0, 0);
			scissor.y1 = min(viewport.y0 + viewport.height, renderTarget->getExternalHeight());
			
			setScissor(scissor);
		}

		sw::Viewport view;
		view.x0 = (float)viewport.x0;
		view.y0 = (float)viewport.y0;
		view.width = (float)viewport.width;
		view.height = (float)viewport.height;
		view.minZ = viewport.minZ;
		view.maxZ = viewport.maxZ;
		
		Renderer::setViewport(view);

		return true;
	}

	bool Device::validRectangle(const sw::Rect *rect, Image *surface)
	{
		if(!rect)
		{
			return true;
		}

		if(rect->x1 <= rect->x0 || rect->y1 <= rect->y0)
		{
			return false;
		}

		if(rect->x0 < 0 || rect->y0 < 0)
		{
			return false;
		}

		if(rect->x1 > (int)surface->getWidth() || rect->y1 > (int)surface->getHeight())
		{
			return false;
		}

		return true;
	}

	void Device::finish()
	{
		synchronize();
	}
}

extern "C"
{
	gl::Device *createDevice()
	{
		sw::Context *context = new sw::Context();

		if(context)
		{
			return new gl::Device(context);
		}

		return 0;
	}
}