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
	class Image;

	enum PrimitiveType
	{
		DRAW_POINTLIST,
		DRAW_LINELIST,
		DRAW_LINESTRIP,
		DRAW_LINELOOP,
		DRAW_TRIANGLELIST,
		DRAW_TRIANGLESTRIP,
		DRAW_TRIANGLEFAN
	};

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

		virtual void clearColor(unsigned int color, unsigned int rgbaMask);
		virtual void clearDepth(float z);
		virtual void clearStencil(unsigned int stencil, unsigned int mask);
		virtual Image *createDepthStencilSurface(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard);
		virtual Image *createRenderTarget(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool lockable);
		virtual void drawIndexedPrimitive(PrimitiveType type, unsigned int indexOffset, unsigned int primitiveCount, int indexSize);
		virtual void drawPrimitive(PrimitiveType primitiveType, unsigned int primiveCount);
		virtual void setDepthStencilSurface(egl::Image *newDepthStencil);
		virtual void setPixelShader(sw::PixelShader *shader);
		virtual void setPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		virtual void setScissorEnable(bool enable);
		virtual void setRenderTarget(egl::Image *renderTarget);
		virtual void setScissorRect(const sw::Rect &rect);
		virtual void setVertexShader(sw::VertexShader *shader);
		virtual void setVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		virtual void setViewport(const Viewport &viewport);

		virtual bool stretchRect(egl::Image *sourceSurface, const sw::SliceRect *sourceRect, egl::Image *destSurface, const sw::SliceRect *destRect, bool filter);
		virtual void finish();

	private:
		sw::Context *const context;

		bool bindResources();
		void bindShaderConstants();
		bool bindViewport();   // Also adjusts for scissoring

		bool validRectangle(const sw::Rect *rect, egl::Image *surface);

		Viewport viewport;
		sw::Rect scissorRect;
		bool scissorEnable;

		sw::PixelShader *pixelShader;
		sw::VertexShader *vertexShader;

		bool pixelShaderDirty;
		unsigned int pixelShaderConstantsFDirty;
		bool vertexShaderDirty;
		unsigned int vertexShaderConstantsFDirty;

		float pixelShaderConstantF[224][4];
		float vertexShaderConstantF[256][4];

		egl::Image *renderTarget;
		egl::Image *depthStencil;
	};
}

#endif   // gl_Device_hpp
