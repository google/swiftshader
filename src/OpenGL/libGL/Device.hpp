// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef gl_Device_hpp
#define gl_Device_hpp

#include "Renderer/Renderer.hpp"

namespace gl
{
	class Image;
	class Texture;

	enum PrimitiveType
	{
		DRAW_POINTLIST,
		DRAW_LINELIST,
		DRAW_LINESTRIP,
		DRAW_LINELOOP,
		DRAW_TRIANGLELIST,
		DRAW_TRIANGLESTRIP,
		DRAW_TRIANGLEFAN,
		DRAW_QUADLIST
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

		virtual void clearColor(float red, float green, float blue, float alpha, unsigned int rgbaMask);
		virtual void clearDepth(float z);
		virtual void clearStencil(unsigned int stencil, unsigned int mask);
		virtual Image *createDepthStencilSurface(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard);
		virtual Image *createRenderTarget(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool lockable);
		virtual void drawIndexedPrimitive(PrimitiveType type, unsigned int indexOffset, unsigned int primitiveCount, int indexSize);
		virtual void drawPrimitive(PrimitiveType primitiveType, unsigned int primiveCount);
		virtual void setDepthStencilSurface(Image *newDepthStencil);
		virtual void setPixelShader(sw::PixelShader *shader);
		virtual void setPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		virtual void setScissorEnable(bool enable);
		virtual void setRenderTarget(int index, Image *renderTarget);
		virtual void setScissorRect(const sw::Rect &rect);
		virtual void setVertexShader(sw::VertexShader *shader);
		virtual void setVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		virtual void setViewport(const Viewport &viewport);

		virtual bool stretchRect(Image *sourceSurface, const sw::SliceRect *sourceRect, Image *destSurface, const sw::SliceRect *destRect, bool filter);
		virtual void finish();

	private:
		sw::Context *const context;

		bool bindResources();
		void bindShaderConstants();
		bool bindViewport();   // Also adjusts for scissoring

		bool validRectangle(const sw::Rect *rect, Image *surface);

		Viewport viewport;
		sw::Rect scissorRect;
		bool scissorEnable;

		sw::PixelShader *pixelShader;
		sw::VertexShader *vertexShader;

		bool pixelShaderDirty;
		unsigned int pixelShaderConstantsFDirty;
		bool vertexShaderDirty;
		unsigned int vertexShaderConstantsFDirty;

		float pixelShaderConstantF[sw::FRAGMENT_UNIFORM_VECTORS][4];
		float vertexShaderConstantF[sw::VERTEX_UNIFORM_VECTORS][4];

		Image *renderTarget;
		Image *depthStencil;
	};
}

#endif   // gl_Device_hpp
