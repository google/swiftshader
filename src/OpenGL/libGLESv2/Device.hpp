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

#ifndef gl_Device_hpp
#define gl_Device_hpp

#include "Renderer/Renderer.hpp"

namespace egl
{
	class Image;
}

namespace es2
{
	class Texture;

	struct Viewport
	{
		int x0;
		int y0;
		unsigned int width;
		unsigned int height;
		float minZ;
		float maxZ;
	};

	class Device : public sw::Renderer
	{
	public:
		explicit Device(sw::Context *context);

		virtual ~Device();

		virtual void clearColor(float red, float green, float blue, float alpha, unsigned int rgbaMask);
		virtual void clearDepth(float z);
		virtual void clearStencil(unsigned int stencil, unsigned int mask);
		virtual egl::Image *createDepthStencilSurface(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard);
		virtual egl::Image *createRenderTarget(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool lockable);
		virtual void drawIndexedPrimitive(sw::DrawType type, unsigned int indexOffset, unsigned int primitiveCount);
		virtual void drawPrimitive(sw::DrawType type, unsigned int primiveCount);
		virtual void setPixelShader(sw::PixelShader *shader);
		virtual void setPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		virtual void setScissorEnable(bool enable);
		virtual void setRenderTarget(int index, egl::Image *renderTarget);
		virtual void setDepthBuffer(egl::Image *depthBuffer);
		virtual void setStencilBuffer(egl::Image *stencilBuffer);
		virtual void setScissorRect(const sw::Rect &rect);
		virtual void setVertexShader(sw::VertexShader *shader);
		virtual void setVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		virtual void setViewport(const Viewport &viewport);

		virtual bool stretchRect(sw::Surface *sourceSurface, const sw::SliceRect *sourceRect, sw::Surface *destSurface, const sw::SliceRect *destRect, bool filter);
		virtual bool stretchCube(sw::Surface *sourceSurface, sw::Surface *destSurface);
		virtual void finish();

	private:
		sw::Context *const context;

		bool bindResources();
		void bindShaderConstants();
		bool bindViewport();   // Also adjusts for scissoring

		bool validRectangle(const sw::Rect *rect, sw::Surface *surface);

		void copyBuffer(sw::byte *sourceBuffer, sw::byte *destBuffer, unsigned int width, unsigned int height, unsigned int sourcePitch, unsigned int destPitch, unsigned int bytes, bool flipX, bool flipY);

		Viewport viewport;
		sw::Rect scissorRect;
		bool scissorEnable;

		sw::PixelShader *pixelShader;
		sw::VertexShader *vertexShader;

		bool pixelShaderDirty;
		unsigned int pixelShaderConstantsFDirty;
		bool vertexShaderDirty;
		unsigned int vertexShaderConstantsFDirty;

		float pixelShaderConstantF[FRAGMENT_UNIFORM_VECTORS][4];
		float vertexShaderConstantF[VERTEX_UNIFORM_VECTORS][4];

		egl::Image *renderTarget[RENDERTARGETS];
		egl::Image *depthBuffer;
		egl::Image *stencilBuffer;
	};
}

#endif   // gl_Device_hpp
