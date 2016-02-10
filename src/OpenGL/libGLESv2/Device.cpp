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

#include "Device.hpp"

#include "common/Image.hpp"
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

namespace es2
{
	using namespace sw;

	Device::Device(Context *context) : Renderer(context, OpenGL, true), context(context)
	{
		depthStencil = nullptr;
		for(int i = 0; i < RENDERTARGETS; ++i) { renderTarget[i] = nullptr; }

		setDepthBufferEnable(true);
		setFillMode(FILL_SOLID);
		setShadingMode(SHADING_GOURAUD);
		setDepthWriteEnable(true);
		setAlphaTestEnable(false);
		setSourceBlendFactor(BLEND_ONE);
		setDestBlendFactor(BLEND_ZERO);
		setCullMode(CULL_COUNTERCLOCKWISE);
		setDepthCompare(DEPTH_LESSEQUAL);
		setAlphaReference(0.0f);
		setAlphaCompare(ALPHA_ALWAYS);
		setAlphaBlendEnable(false);
		setFogEnable(false);
		setSpecularEnable(false);
		setFogColor(0);
		setPixelFogMode(FOG_NONE);
		setFogStart(0.0f);
		setFogEnd(1.0f);
		setFogDensity(1.0f);
		setRangeFogEnable(false);
		setStencilEnable(false);
		setStencilFailOperation(OPERATION_KEEP);
		setStencilZFailOperation(OPERATION_KEEP);
		setStencilPassOperation(OPERATION_KEEP);
		setStencilCompare(STENCIL_ALWAYS);
		setStencilReference(0);
		setStencilMask(0xFFFFFFFF);
		setStencilWriteMask(0xFFFFFFFF);
		setVertexFogMode(FOG_NONE);
		setClipFlags(0);
		setPointSize(1.0f);
		setPointSizeMin(0.125f);
        setPointSizeMax(8192.0f);
		setColorWriteMask(0, 0x0000000F);
		setBlendOperation(BLENDOP_ADD);
		scissorEnable = false;
		setSlopeDepthBias(0.0f);
		setTwoSidedStencil(false);
		setStencilFailOperationCCW(OPERATION_KEEP);
		setStencilZFailOperationCCW(OPERATION_KEEP);
		setStencilPassOperationCCW(OPERATION_KEEP);
		setStencilCompareCCW(STENCIL_ALWAYS);
		setColorWriteMask(1, 0x0000000F);
		setColorWriteMask(2, 0x0000000F);
		setColorWriteMask(3, 0x0000000F);
		setBlendConstant(0xFFFFFFFF);
		setWriteSRGB(false);
		setDepthBias(0.0f);
		setSeparateAlphaBlendEnable(false);
		setSourceBlendFactorAlpha(BLEND_ONE);
		setDestBlendFactorAlpha(BLEND_ZERO);
		setBlendOperationAlpha(BLENDOP_ADD);
		setPointSpriteEnable(true);
		setColorLogicOpEnabled(false);
		setLogicalOperation(LOGICALOP_COPY);

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

		pixelShader = nullptr;
		vertexShader = nullptr;

		pixelShaderDirty = true;
		pixelShaderConstantsFDirty = 0;
		vertexShaderDirty = true;
		vertexShaderConstantsFDirty = 0;

		for(int i = 0; i < FRAGMENT_UNIFORM_VECTORS; i++)
		{
			float zero[4] = {0, 0, 0, 0};

			setPixelShaderConstantF(i, zero, 1);
		}

		for(int i = 0; i < VERTEX_UNIFORM_VECTORS; i++)
		{
			float zero[4] = {0, 0, 0, 0};

			setVertexShaderConstantF(i, zero, 1);
		}
	}

	Device::~Device()
	{
		if(depthStencil)
		{
			depthStencil->release();
			depthStencil = nullptr;
		}

		for(int i = 0; i < RENDERTARGETS; ++i)
		{
			if(renderTarget[i])
			{
				renderTarget[i]->release();
				renderTarget[i] = nullptr;
			}
		}

		delete context;
	}

	void Device::getScissoredRegion(egl::Image *sourceSurface, int &x0, int &y0, int& width, int& height) const
	{
		x0 = 0;
		y0 = 0;
		width = sourceSurface->getWidth();
		height = sourceSurface->getHeight();

		if(scissorEnable)   // Clamp against scissor rectangle
		{
			if(x0 < scissorRect.x0) x0 = scissorRect.x0;
			if(y0 < scissorRect.y0) y0 = scissorRect.y0;
			if(width > scissorRect.x1 - scissorRect.x0) width = scissorRect.x1 - scissorRect.x0;
			if(height > scissorRect.y1 - scissorRect.y0) height = scissorRect.y1 - scissorRect.y0;
		}
	}

	void Device::clearColor(float red, float green, float blue, float alpha, unsigned int rgbaMask)
	{
		if(!rgbaMask)
		{
			return;
		}

		float rgba[4];
		rgba[0] = red;
		rgba[1] = green;
		rgba[2] = blue;
		rgba[3] = alpha;

		for(int i = 0; i < RENDERTARGETS; ++i)
		{
			if(renderTarget[i])
			{
				int x0(0), y0(0), width(0), height(0);
				getScissoredRegion(renderTarget[i], x0, y0, width, height);

				sw::SliceRect sliceRect;
				if(renderTarget[i]->getClearRect(x0, y0, width, height, sliceRect))
				{
					clear(rgba, FORMAT_A32B32G32R32F, renderTarget[i], sliceRect, rgbaMask);
				}
			}
		}
	}

	void Device::clearDepth(float z)
	{
		if(!depthStencil)
		{
			return;
		}

		if(z > 1) z = 1;
		if(z < 0) z = 0;

		int x0(0), y0(0), width(0), height(0);
		getScissoredRegion(depthStencil, x0, y0, width, height);

		depthStencil->clearDepthBuffer(z, x0, y0, width, height);
	}

	void Device::clearStencil(unsigned int stencil, unsigned int mask)
	{
		if(!depthStencil)
		{
			return;
		}

		int x0(0), y0(0), width(0), height(0);
		getScissoredRegion(depthStencil, x0, y0, width, height);

		depthStencil->clearStencilBuffer(stencil, mask, x0, y0, width, height);
	}

	egl::Image *Device::createDepthStencilSurface(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard)
	{
		if(height > OUTLINE_RESOLUTION)
		{
			ERR("Invalid parameters: %dx%d", width, height);
			return nullptr;
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
		case FORMAT_DF24S8:
		case FORMAT_DF16S8:
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

	egl::Image *Device::createRenderTarget(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool lockable)
	{
		if(height > OUTLINE_RESOLUTION)
		{
			ERR("Invalid parameters: %dx%d", width, height);
			return nullptr;
		}

		egl::Image *surface = new egl::Image(width, height, format, multiSampleDepth, lockable);

		if(!surface)
		{
			ERR("Out of memory");
			return nullptr;
		}

		return surface;
	}

	void Device::drawIndexedPrimitive(sw::DrawType type, unsigned int indexOffset, unsigned int primitiveCount)
	{
		if(!bindResources() || !primitiveCount)
		{
			return;
		}

		draw(type, indexOffset, primitiveCount);
	}

	void Device::drawPrimitive(sw::DrawType type, unsigned int primitiveCount)
	{
		if(!bindResources() || !primitiveCount)
		{
			return;
		}

		setIndexBuffer(nullptr);

		draw(type, 0, primitiveCount);
	}

	void Device::setDepthStencilSurface(egl::Image *depthStencil)
	{
		if(this->depthStencil == depthStencil)
		{
			return;
		}

		if(depthStencil)
		{
			depthStencil->addRef();
		}

		if(this->depthStencil)
		{
			this->depthStencil->release();
		}

		this->depthStencil = depthStencil;

		setDepthStencil(depthStencil);
	}

	void Device::setPixelShader(PixelShader *pixelShader)
	{
		this->pixelShader = pixelShader;
		pixelShaderDirty = true;
	}

	void Device::setPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count)
	{
		for(unsigned int i = 0; i < count && startRegister + i < FRAGMENT_UNIFORM_VECTORS; i++)
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

	void Device::setRenderTarget(int index, egl::Image *renderTarget)
	{
		if(renderTarget)
		{
			renderTarget->addRef();
		}

		if(this->renderTarget[index])
		{
			this->renderTarget[index]->release();
		}

		this->renderTarget[index] = renderTarget;

		Renderer::setRenderTarget(index, renderTarget);
	}

	void Device::setScissorRect(const sw::Rect &rect)
	{
		scissorRect = rect;
	}

	void Device::setVertexShader(VertexShader *vertexShader)
	{
		this->vertexShader = vertexShader;
		vertexShaderDirty = true;
	}

	void Device::setVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count)
	{
		for(unsigned int i = 0; i < count && startRegister + i < VERTEX_UNIFORM_VECTORS; i++)
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
		this->viewport = viewport;
	}

	void Device::copyBuffer(sw::byte *sourceBuffer, sw::byte *destBuffer, unsigned int width, unsigned int height, unsigned int sourcePitch, unsigned int destPitch, unsigned int bytes, bool flipX, bool flipY)
	{
		unsigned int widthB = width * bytes;
		unsigned int widthMaxB = widthB - 1;

		if(flipX)
		{
			if(flipY)
			{
				sourceBuffer += (height - 1) * sourcePitch;
				for(unsigned int y = 0; y < height; ++y, sourceBuffer -= sourcePitch, destBuffer += destPitch)
				{
					for(unsigned int x = 0; x < widthB; ++x)
					{
						destBuffer[x] = sourceBuffer[widthMaxB - x];
					}
				}
			}
			else
			{
				for(unsigned int y = 0; y < height; ++y, sourceBuffer += sourcePitch, destBuffer += destPitch)
				{
					for(unsigned int x = 0; x < widthB; ++x)
					{
						destBuffer[x] = sourceBuffer[widthMaxB - x];
					}
				}
			}
		}
		else
		{
			if(flipY)
			{
				sourceBuffer += (height - 1) * sourcePitch;
				for(unsigned int y = 0; y < height; ++y, sourceBuffer -= sourcePitch, destBuffer += destPitch)
				{
					memcpy(destBuffer, sourceBuffer, widthB);
				}
			}
			else
			{
				for(unsigned int y = 0; y < height; ++y, sourceBuffer += sourcePitch, destBuffer += destPitch)
				{
					memcpy(destBuffer, sourceBuffer, widthB);
				}
			}
		}
	}

	bool Device::stretchRect(sw::Surface *source, const sw::SliceRect *sourceRect, sw::Surface *dest, const sw::SliceRect *destRect, bool filter)
	{
		if(!source || !dest)
		{
			ERR("Invalid parameters");
			return false;
		}

		int sWidth = source->getWidth();
		int sHeight = source->getHeight();
		int dWidth = dest->getWidth();
		int dHeight = dest->getHeight();

		bool flipX = false;
		bool flipY = false;
		if(sourceRect && destRect)
		{
			flipX = (sourceRect->x0 < sourceRect->x1) ^ (destRect->x0 < destRect->x1);
			flipY = (sourceRect->y0 < sourceRect->y1) ^ (destRect->y0 < destRect->y1);
		}
		else if(sourceRect)
		{
			flipX = (sourceRect->x0 > sourceRect->x1);
			flipY = (sourceRect->y0 > sourceRect->y1);
		}
		else if(destRect)
		{
			flipX = (destRect->x0 > destRect->x1);
			flipY = (destRect->y0 > destRect->y1);
		}

		SliceRect sRect;
		SliceRect dRect;

		if(sourceRect)
		{
			sRect = *sourceRect;

			if(sRect.x0 > sRect.x1)
			{
				swap(sRect.x0, sRect.x1);
			}

			if(sRect.y0 > sRect.y1)
			{
				swap(sRect.y0, sRect.y1);
			}
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

			if(dRect.x0 > dRect.x1)
			{
				swap(dRect.x0, dRect.x1);
			}

			if(dRect.y0 > dRect.y1)
			{
				swap(dRect.y0, dRect.y1);
			}
		}
		else
		{
			dRect.y0 = 0;
			dRect.x0 = 0;
			dRect.y1 = dHeight;
			dRect.x1 = dWidth;
		}

		if(!validRectangle(&sRect, source) || !validRectangle(&dRect, dest))
		{
			ERR("Invalid parameters");
			return false;
		}

		bool scaling = (sRect.x1 - sRect.x0 != dRect.x1 - dRect.x0) || (sRect.y1 - sRect.y0 != dRect.y1 - dRect.y0);
		bool equalFormats = source->getInternalFormat() == dest->getInternalFormat();
		bool depthStencil = egl::Image::isDepth(source->getInternalFormat()) || egl::Image::isStencil(source->getInternalFormat());
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
				sw::byte *sourceBuffer = (sw::byte*)source->lockInternal(0, 0, sourceRect->slice, LOCK_READONLY, PUBLIC);
				sw::byte *destBuffer = (sw::byte*)dest->lockInternal(0, 0, destRect->slice, LOCK_DISCARD, PUBLIC);

				copyBuffer(sourceBuffer, destBuffer, source->getWidth(), source->getHeight(), source->getInternalPitchB(), dest->getInternalPitchB(), egl::Image::bytes(source->getInternalFormat()), flipX, flipY);

				source->unlockInternal();
				dest->unlockInternal();
			}

			if(source->hasStencil())
			{
				sw::byte *sourceBuffer = (sw::byte*)source->lockStencil(0, PUBLIC);
				sw::byte *destBuffer = (sw::byte*)dest->lockStencil(0, PUBLIC);

				copyBuffer(sourceBuffer, destBuffer, source->getWidth(), source->getHeight(), source->getInternalPitchB(), dest->getInternalPitchB(), egl::Image::bytes(source->getInternalFormat()), flipX, flipY);

				source->unlockStencil();
				dest->unlockStencil();
			}
		}
		else if(!scaling && equalFormats)
		{
			unsigned char *sourceBytes = (unsigned char*)source->lockInternal(sRect.x0, sRect.y0, sourceRect->slice, LOCK_READONLY, PUBLIC);
			unsigned char *destBytes = (unsigned char*)dest->lockInternal(dRect.x0, dRect.y0, destRect->slice, LOCK_READWRITE, PUBLIC);
			unsigned int sourcePitch = source->getInternalPitchB();
			unsigned int destPitch = dest->getInternalPitchB();

			unsigned int width = dRect.x1 - dRect.x0;
			unsigned int height = dRect.y1 - dRect.y0;

			copyBuffer(sourceBytes, destBytes, width, height, sourcePitch, destPitch, egl::Image::bytes(source->getInternalFormat()), flipX, flipY);

			if(alpha0xFF)
			{
				for(unsigned int y = 0; y < height; ++y, destBytes += destPitch)
				{
					for(unsigned int x = 0; x < width; ++x)
					{
						destBytes[4 * x + 3] = 0xFF;
					}
				}
			}

			source->unlockInternal();
			dest->unlockInternal();
		}
		else
		{
			if(flipX)
			{
				swap(dRect.x0, dRect.x1);
			}
			if(flipY)
			{
				swap(dRect.y0, dRect.y1);
			}
			blit(source, sRect, dest, dRect, scaling && filter);
		}

		return true;
	}

	bool Device::stretchCube(sw::Surface *source, sw::Surface *dest)
	{
		if(!source || !dest || egl::Image::isDepth(source->getInternalFormat()) || egl::Image::isStencil(source->getInternalFormat()))
		{
			ERR("Invalid parameters");
			return false;
		}

		int sWidth  = source->getWidth();
		int sHeight = source->getHeight();
		int sDepth  = source->getDepth();
		int dWidth  = dest->getWidth();
		int dHeight = dest->getHeight();
		int dDepth  = dest->getDepth();

		bool scaling = (sWidth != dWidth) || (sHeight != dHeight) || (sDepth != dDepth);
		bool equalFormats = source->getInternalFormat() == dest->getInternalFormat();
		bool alpha0xFF = false;

		if((source->getInternalFormat() == FORMAT_A8R8G8B8 && dest->getInternalFormat() == FORMAT_X8R8G8B8) ||
		   (source->getInternalFormat() == FORMAT_X8R8G8B8 && dest->getInternalFormat() == FORMAT_A8R8G8B8))
		{
			equalFormats = true;
			alpha0xFF = true;
		}

		if(!scaling && equalFormats)
		{
			unsigned int sourcePitch = source->getInternalPitchB();
			unsigned int destPitch = dest->getInternalPitchB();
			unsigned int bytes = dWidth * egl::Image::bytes(source->getInternalFormat());

			for(int z = 0; z < dDepth; ++z)
			{
				unsigned char *sourceBytes = (unsigned char*)source->lockInternal(0, 0, z, LOCK_READONLY, PUBLIC);
				unsigned char *destBytes = (unsigned char*)dest->lockInternal(0, 0, z, LOCK_READWRITE, PUBLIC);
				for(int y = 0; y < dHeight; ++y)
				{
					memcpy(destBytes, sourceBytes, bytes);

					if(alpha0xFF)
					{
						for(int x = 0; x < dWidth; ++x)
						{
							destBytes[4 * x + 3] = 0xFF;
						}
					}

					sourceBytes += sourcePitch;
					destBytes += destPitch;
				}
			}

			source->unlockInternal();
			dest->unlockInternal();
		}
		else
		{
			blit3D(source, dest);
		}

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
			scissor.x0 = viewport.x0;
			scissor.x1 = viewport.x0 + viewport.width;
			scissor.y0 = viewport.y0;
			scissor.y1 = viewport.y0 + viewport.height;

			for(int i = 0; i < RENDERTARGETS; ++i)
			{
				if(renderTarget[i])
				{
					scissor.x0 = max(scissor.x0, 0);
					scissor.x1 = min(scissor.x1, renderTarget[i]->getWidth());
					scissor.y0 = max(scissor.y0, 0);
					scissor.y1 = min(scissor.y1, renderTarget[i]->getHeight());
				}
			}

			if(depthStencil)
			{
				scissor.x0 = max(scissor.x0, 0);
				scissor.x1 = min(scissor.x1, depthStencil->getWidth());
				scissor.y0 = max(scissor.y0, 0);
				scissor.y1 = min(scissor.y1, depthStencil->getHeight());
			}

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

	bool Device::validRectangle(const sw::Rect *rect, sw::Surface *surface)
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
