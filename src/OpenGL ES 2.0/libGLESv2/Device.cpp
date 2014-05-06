// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
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

bool localShaderConstants = true;

namespace gl
{
	using namespace sw;

	Device::Device(Context *context) : Renderer(context), context(context)
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
		setPointSizeMin(1.0f);
		setPointSpriteEnable(false);
		setPointSizeMax(64.0f);
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

		int x = viewport.x;
		int y = viewport.y;
		int width = viewport.width;
		int height = viewport.height;

		// Clamp against scissor rectangle
		if(scissorEnable)
		{
			if(x < scissorRect.left) x = scissorRect.left;
			if(y < scissorRect.top) y = scissorRect.top;
			if(width > scissorRect.right - scissorRect.left) width = scissorRect.right - scissorRect.left;
			if(height > scissorRect.bottom - scissorRect.top) height = scissorRect.bottom - scissorRect.top;
		}

		if(renderTarget)
		{
			renderTarget->clearColorBuffer(color, rgbaMask, x, y, width, height);
		}
	}

	void Device::clearDepth(float z)
	{
		TRACE("float z = %f", z);

		if(z > 1) z = 1;
		if(z < 0) z = 0;

		int x = viewport.x;
		int y = viewport.y;
		int width = viewport.width;
		int height = viewport.height;

		// Clamp against scissor rectangle
		if(scissorEnable)
		{
			if(x < scissorRect.left) x = scissorRect.left;
			if(y < scissorRect.top) y = scissorRect.top;
			if(width > scissorRect.right - scissorRect.left) width = scissorRect.right - scissorRect.left;
			if(height > scissorRect.bottom - scissorRect.top) height = scissorRect.bottom - scissorRect.top;
		}

		if(depthStencil)
		{
			depthStencil->clearDepthBuffer(z, x, y, width, height);
		}
	}

	void Device::clearStencil(unsigned int stencil, unsigned int mask)
	{
		TRACE("unsigned long stencil = %d", stencil);

		int x = viewport.x;
		int y = viewport.y;
		int width = viewport.width;
		int height = viewport.height;

		// Clamp against scissor rectangle
		if(scissorEnable)
		{
			if(x < scissorRect.left) x = scissorRect.left;
			if(y < scissorRect.top) y = scissorRect.top;
			if(width > scissorRect.right - scissorRect.left) width = scissorRect.right - scissorRect.left;
			if(height > scissorRect.bottom - scissorRect.top) height = scissorRect.bottom - scissorRect.top;
		}

		if(depthStencil)
		{
			depthStencil->clearStencilBuffer(stencil, mask, x, y, width, height);
		}
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

	bool Device::getRenderTargetData(Image *renderTarget, Image *destSurface)
	{
		TRACE("Image *renderTarget = 0x%0.8p, Image *destSurface = 0x%0.8p", renderTarget, destSurface);

		if(!renderTarget || !destSurface)
		{
			ERR("Invalid parameters");
			return false;
		}
		
		if(renderTarget->getWidth()  != destSurface->getWidth() ||
		   renderTarget->getHeight() != destSurface->getHeight() ||
		   renderTarget->getInternalFormat() != destSurface->getInternalFormat())
		{
			ERR("Invalid parameters");
			return false;
		}

		static void (__cdecl *blitFunction)(void *dst, void *src);
		static Routine *blitRoutine;
		static BlitState blitState = {0};

		BlitState update;
		update.width = renderTarget->getInternalWidth();
		update.height = renderTarget->getInternalHeight();
		update.depth = 32;
		update.stride = destSurface->getInternalPitchB();
		update.HDR = false;
		update.cursorHeight = 0;
		update.cursorWidth = 0;
		
		if(memcmp(&blitState, &update, sizeof(BlitState)) != 0)
		{
			blitState = update;
			delete blitRoutine;

			blitRoutine = FrameBuffer::copyRoutine(blitState);
			blitFunction = (void(__cdecl*)(void*, void*))blitRoutine->getEntry();
		}

		void *dst = destSurface->lockInternal(0, 0, 0, LOCK_WRITEONLY, PUBLIC);
		void *src = renderTarget->lockInternal(0, 0, 0, LOCK_WRITEONLY, PUBLIC);

		blitFunction(dst, src);

		destSurface->unlockInternal();
		renderTarget->unlockInternal();

		return true;
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

		if(renderTarget)
		{
			// Reset viewport to size of current render target
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = renderTarget->getWidth();
			viewport.height = renderTarget->getHeight();
			viewport.minZ = 0;
			viewport.maxZ = 1;

			// Reset scissor rectangle to size of current render target
			scissorRect.left = 0;
			scissorRect.top = 0;
			scissorRect.right = renderTarget->getWidth();
			scissorRect.bottom = renderTarget->getHeight();
		}

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
			sRect.top = 0;
			sRect.left = 0;
			sRect.bottom = sHeight;
			sRect.right = sWidth;
		}

		if(destRect)
		{
			dRect = *destRect;
		}
		else
		{
			dRect.top = 0;
			dRect.left = 0;
			dRect.bottom = dHeight;
			dRect.right = dWidth;
		}

		bool scaling = (sRect.right - sRect.left != dRect.right - dRect.left) || (sRect.bottom - sRect.top != dRect.bottom - dRect.top);
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
			unsigned char *sourceBytes = (unsigned char*)source->lockInternal(sRect.left, sRect.top, 0, LOCK_READONLY, PUBLIC);
			unsigned char *destBytes = (unsigned char*)dest->lockInternal(dRect.left, dRect.top, 0, LOCK_READWRITE, PUBLIC);
			unsigned int sourcePitch = source->getInternalPitchB();
			unsigned int destPitch = dest->getInternalPitchB();

			unsigned int width = dRect.right - dRect.left;
			unsigned int height = dRect.bottom - dRect.top;
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
			sRect.left = sourceRect->left;
			sRect.top = sourceRect->top;
			sRect.right = sourceRect->right;
			sRect.bottom = sourceRect->bottom;
		}
		else
		{
			sRect.left = 0;
			sRect.top = 0;
			sRect.right = sourceSurface->getWidth();
			sRect.bottom = sourceSurface->getHeight();
		}

		if(destPoint)
		{
			dRect.left = destPoint->x;
			dRect.top = destPoint->y;
			dRect.right = destPoint->x + sRect.right - sRect.left;
			dRect.bottom = destPoint->y + sRect.bottom - sRect.top;
		}
		else
		{
			dRect.left = 0;
			dRect.top = 0;
			dRect.right = sRect.right - sRect.left;
			dRect.bottom = sRect.bottom - sRect.top;
		}

		if(!validRectangle(&sRect, sourceSurface) || !validRectangle(&dRect, destinationSurface))
		{
			ERR("Invalid parameters");
			return false;
		}

		int sWidth = sRect.right - sRect.left;
		int sHeight = sRect.bottom - sRect.top;

		int dWidth = dRect.right - dRect.left;
		int dHeight = dRect.bottom - dRect.top;

		if(sourceSurface->getMultiSampleDepth() > 1 ||
		   destinationSurface->getMultiSampleDepth() > 1 ||
		   sourceSurface->getInternalFormat() != destinationSurface->getInternalFormat())
		{
			ERR("Invalid parameters");
			return false;
		}
		
		unsigned char *sourceBuffer = (unsigned char*)sourceSurface->lock(sRect.left, sRect.top, LOCK_READONLY);
		unsigned char *destinationBuffer = (unsigned char*)destinationSurface->lock(dRect.left, dRect.top, LOCK_WRITEONLY);

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
			Rect scissor = scissorRect;

			long viewportLeft = viewport.x;
			long viewportRight = viewport.x + viewport.width;
			long viewportTop = viewport.y;
			long viewportBottom = viewport.y + viewport.height;

			// Intersection of scissor rectangle and viewport
			if(viewportLeft > scissor.left) scissor.left = viewportLeft;
			if(viewportTop > scissor.top) scissor.top = viewportTop;
			if(viewportRight < scissor.right) scissor.right = viewportRight;
			if(viewportBottom < scissor.bottom) scissor.bottom = viewportBottom;

			if(scissor.left == scissor.right ||
			   scissor.top == scissor.bottom)
			{
				return false;
			}

			// Dimensions of scissor rectangle relative to viewport
			float relativeLeft = (float)(scissor.left - viewportLeft) / viewport.width;
			float relativeRight = (float)(scissor.right - viewportLeft) / viewport.width;
			float relativeTop = (float)(scissor.top - viewportTop) / viewport.height;
			float relativeBottom = (float)(scissor.bottom - viewportTop) / viewport.height;

			// Transformation of clip space coordinates
			float sX = 1.0f / (relativeRight - relativeLeft);   // Scale
			float tX = sX * ((0.5f - relativeLeft) - (relativeRight - 0.5f));   // Translate
			float sY = 1.0f / (relativeBottom - relativeTop);   // Scale
			float tY = sY * ((0.5f - relativeTop) - (relativeBottom - 0.5f));   // Translate

			// Set the new viewport
			sw::Viewport view;

			view.setLeft((float)scissor.left);
			view.setTop((float)scissor.top);
			view.setWidth((float)(scissor.right - scissor.left));
			view.setHeight((float)(scissor.bottom - scissor.top));

			view.setNear(viewport.minZ);
			view.setFar(viewport.maxZ);

			Renderer::setViewport(view);
			setPostTransformEnable(true);
			setPosScale(sX, sY);
			setPosOffset(tX, -tY);
		}
		else
		{
			// Set viewport
			sw::Viewport view;

			view.setLeft((float)viewport.x);
			view.setTop((float)viewport.y);
			view.setWidth((float)viewport.width);
			view.setHeight((float)viewport.height);

			view.setNear(viewport.minZ);
			view.setFar(viewport.maxZ);

			Renderer::setViewport(view);
			setPostTransformEnable(false);
		}

		return true;
	}

	bool Device::validRectangle(const sw::Rect *rect, Image *surface)
	{
		if(!rect)
		{
			return true;
		}

		if(rect->right <= rect->left || rect->bottom <= rect->top)
		{
			return false;
		}

		if(rect->left < 0 || rect->top < 0)
		{
			return false;
		}

		if(rect->right > (int)surface->getWidth() || rect->bottom > (int)surface->getHeight())
		{
			return false;
		}

		return true;
	}

	void Device::finish()
	{
		if(renderTarget)
		{
			renderTarget->lock(0, 0, sw::LOCK_READWRITE);
			renderTarget->unlock();
		}
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