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

#ifndef sw_Context_hpp
#define sw_Context_hpp

#include "Sampler.hpp"
#include "Stream.hpp"
#include "Point.hpp"
#include "Vertex.hpp"
#include "System/Types.hpp"

namespace sw
{
	class Sampler;
	class Surface;
	class PixelShader;
	class VertexShader;
	struct Triangle;
	struct Primitive;
	struct Vertex;
	class Resource;

	enum In   // Default input stream semantic
	{
		Position = 0,
		BlendWeight = 1,
		BlendIndices = 2,
		Normal = 3,
		PointSize = 4,
		Color0 = 5,
		Color1 = 6,
		TexCoord0 = 7,
		TexCoord1 = 8,
		TexCoord2 = 9,
		TexCoord3 = 10,
		TexCoord4 = 11,
		TexCoord5 = 12,
		TexCoord6 = 13,
		TexCoord7 = 14,
		PositionT = 15
	};

	enum DrawType ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		// These types must stay ordered by vertices per primitive. Also, if these basic types
		// are modified, verify the value assigned to task->verticesPerPrimitive in Renderer.cpp
		DRAW_POINTLIST     = 0x00,
		DRAW_LINELIST      = 0x01,
		DRAW_LINESTRIP     = 0x02,
		DRAW_TRIANGLELIST  = 0x03,
		DRAW_TRIANGLESTRIP = 0x04,
		DRAW_TRIANGLEFAN   = 0x05,

		DRAW_NONINDEXED = 0x00,
		DRAW_INDEXED16  = 0x20,
		DRAW_INDEXED32  = 0x30,

		DRAW_INDEXEDPOINTLIST16 = DRAW_POINTLIST | DRAW_INDEXED16,
		DRAW_INDEXEDLINELIST16  = DRAW_LINELIST  | DRAW_INDEXED16,
		DRAW_INDEXEDLINESTRIP16 = DRAW_LINESTRIP | DRAW_INDEXED16,
		DRAW_INDEXEDTRIANGLELIST16  = DRAW_TRIANGLELIST  | DRAW_INDEXED16,
		DRAW_INDEXEDTRIANGLESTRIP16 = DRAW_TRIANGLESTRIP | DRAW_INDEXED16,
		DRAW_INDEXEDTRIANGLEFAN16   = DRAW_TRIANGLEFAN   | DRAW_INDEXED16,

		DRAW_INDEXEDPOINTLIST32 = DRAW_POINTLIST | DRAW_INDEXED32,
		DRAW_INDEXEDLINELIST32  = DRAW_LINELIST  | DRAW_INDEXED32,
		DRAW_INDEXEDLINESTRIP32 = DRAW_LINESTRIP | DRAW_INDEXED32,
		DRAW_INDEXEDTRIANGLELIST32  = DRAW_TRIANGLELIST  | DRAW_INDEXED32,
		DRAW_INDEXEDTRIANGLESTRIP32 = DRAW_TRIANGLESTRIP | DRAW_INDEXED32,
		DRAW_INDEXEDTRIANGLEFAN32   = DRAW_TRIANGLEFAN   | DRAW_INDEXED32,

		DRAW_LAST = DRAW_INDEXEDTRIANGLEFAN32
	};

	enum CullMode ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		CULL_NONE,
		CULL_CLOCKWISE,
		CULL_COUNTERCLOCKWISE,

		CULL_LAST = CULL_COUNTERCLOCKWISE
	};

	enum TransparencyAntialiasing ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		TRANSPARENCY_NONE,
		TRANSPARENCY_ALPHA_TO_COVERAGE,

		TRANSPARENCY_LAST = TRANSPARENCY_ALPHA_TO_COVERAGE
	};

	class Context
	{
	public:
		Context();

		~Context();

		void *operator new(size_t bytes);
		void operator delete(void *pointer, size_t bytes);

		void init();

		bool isDrawPoint() const;
		bool isDrawLine() const;
		bool isDrawTriangle() const;

		bool setDepthBufferEnable(bool depthBufferEnable);

		bool setAlphaBlendEnable(bool alphaBlendEnable);
		bool setSourceBlendFactor(VkBlendFactor sourceBlendFactor);
		bool setDestBlendFactor(VkBlendFactor destBlendFactor);
		bool setBlendOperation(VkBlendOp blendOperation);

		bool setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable);
		bool setSourceBlendFactorAlpha(VkBlendFactor sourceBlendFactorAlpha);
		bool setDestBlendFactorAlpha(VkBlendFactor destBlendFactorAlpha);
		bool setBlendOperationAlpha(VkBlendOp blendOperationAlpha);

		bool setColorWriteMask(int index, int colorWriteMask);
		bool setWriteSRGB(bool sRGB);

		bool setColorLogicOpEnabled(bool colorLogicOpEnabled);
		bool setLogicalOperation(VkLogicOp logicalOperation);

		bool depthWriteActive();
		bool alphaTestActive();
		bool depthBufferActive();
		bool stencilActive();

		bool perspectiveActive();

		bool alphaBlendActive();
		VkBlendFactor sourceBlendFactor();
		VkBlendFactor destBlendFactor();
		VkBlendOp blendOperation();

		VkBlendFactor sourceBlendFactorAlpha();
		VkBlendFactor destBlendFactorAlpha();
		VkBlendOp blendOperationAlpha();

		VkLogicOp colorLogicOp();

		unsigned short pixelShaderModel() const;
		unsigned short vertexShaderModel() const;

		int getMultiSampleCount() const;

		DrawType drawType;

		bool stencilEnable;
		VkCompareOp stencilCompareMode;
		int stencilReference;
		int stencilMask;
		VkStencilOp stencilFailOperation;
		VkStencilOp stencilPassOperation;
		VkStencilOp stencilZFailOperation;
		int stencilWriteMask;

		bool twoSidedStencil;
		VkCompareOp stencilCompareModeCCW;
		int stencilReferenceCCW;
		int stencilMaskCCW;
		VkStencilOp stencilFailOperationCCW;
		VkStencilOp stencilPassOperationCCW;
		VkStencilOp stencilZFailOperationCCW;
		int stencilWriteMaskCCW;

		// Pixel processor states
		VkCompareOp alphaCompareMode;
		bool alphaTestEnable;

		CullMode cullMode;
		bool frontFacingCCW;
		float alphaReference;

		float depthBias;
		float slopeDepthBias;

		Sampler sampler[TOTAL_IMAGE_UNITS];

		VkFormat renderTargetInternalFormat(int index);
		int colorWriteActive();
		int colorWriteActive(int index);
		bool colorUsed();

		Resource *texture[TOTAL_IMAGE_UNITS];
		Stream input[MAX_VERTEX_INPUTS];
		Resource *indexBuffer;

		Surface *renderTarget[RENDERTARGETS];
		unsigned int renderTargetLayer[RENDERTARGETS];
		Surface *depthBuffer;
		unsigned int depthBufferLayer;
		Surface *stencilBuffer;
		unsigned int stencilBufferLayer;

		// Shaders
		const PixelShader *pixelShader;
		const VertexShader *vertexShader;

		// Instancing
		int instanceID;

		bool occlusionEnabled;
		bool transformFeedbackQueryEnabled;
		uint64_t transformFeedbackEnabled;

		// Pixel processor states
		bool rasterizerDiscard;
		bool depthBufferEnable;
		VkCompareOp depthCompareMode;
		bool depthWriteEnable;

		bool alphaBlendEnable;
		VkBlendFactor sourceBlendFactorState;
		VkBlendFactor destBlendFactorState;
		VkBlendOp blendOperationState;

		bool separateAlphaBlendEnable;
		VkBlendFactor sourceBlendFactorStateAlpha;
		VkBlendFactor destBlendFactorStateAlpha;
		VkBlendOp blendOperationStateAlpha;

		float lineWidth;

		int colorWriteMask[RENDERTARGETS];   // RGBA
		bool writeSRGB;
		unsigned int sampleMask;
		unsigned int multiSampleMask;

		bool colorLogicOpEnabled;
		VkLogicOp logicalOperation;
	};
}

#endif   // sw_Context_hpp
