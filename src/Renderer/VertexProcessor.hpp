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

#ifndef sw_VertexProcessor_hpp
#define sw_VertexProcessor_hpp

#include "Matrix.hpp"
#include "Context.hpp"
#include "RoutineCache.hpp"

namespace sw
{
	struct DrawData;

	struct VertexCache   // FIXME: Variable size
	{
		void clear();

		Vertex vertex[16][4];
		unsigned int tag[16];

		int drawCall;
	};

	struct VertexTask
	{
		unsigned int vertexStart;
		unsigned int vertexCount;
		unsigned int verticesPerPrimitive;
		VertexCache vertexCache;
	};

	class VertexProcessor
	{
	public:
		struct States
		{
			unsigned int computeHash();

			uint64_t shaderID;

			bool fixedFunction             : 1;
			bool textureSampling           : 1;
			unsigned int positionRegister  : 4;
			unsigned int pointSizeRegister : 4;   // 0xF signifies no vertex point size

			unsigned int vertexBlendMatrixCount               : 3;
			bool indexedVertexBlendEnable                     : 1;
			bool vertexNormalActive                           : 1;
			bool normalizeNormals                             : 1;
			bool vertexLightingActive                         : 1;
			bool diffuseActive                                : 1;
			bool specularActive                               : 1;
			bool vertexSpecularActive                         : 1;
			unsigned int vertexLightActive                    : 8;
			MaterialSource vertexDiffuseMaterialSourceActive  : BITS(MATERIAL_LAST);
			MaterialSource vertexSpecularMaterialSourceActive : BITS(MATERIAL_LAST);
			MaterialSource vertexAmbientMaterialSourceActive  : BITS(MATERIAL_LAST);
			MaterialSource vertexEmissiveMaterialSourceActive : BITS(MATERIAL_LAST);
			bool fogActive                                    : 1;
			FogMode vertexFogMode                             : BITS(FOG_LAST);
			bool rangeFogActive                               : 1;
			bool localViewerActive                            : 1;
			bool pointSizeActive                              : 1;
			bool pointScaleActive                             : 1;
			bool transformFeedbackQueryEnabled                : 1;
			uint64_t transformFeedbackEnabled                 : 64;

			bool preTransformed : 1;
			bool superSampling  : 1;
			bool multiSampling  : 1;

			struct TextureState
			{
				TexGen texGenActive                       : BITS(TEXGEN_LAST);
				unsigned char textureTransformCountActive : 3;
				unsigned char texCoordIndexActive         : 3;
			};

			TextureState textureState[8];

			Sampler::State samplerState[VERTEX_TEXTURE_IMAGE_UNITS];

			struct Input
			{
				operator bool() const   // Returns true if stream contains data
				{
					return count != 0;
				}

				StreamType type    : BITS(STREAMTYPE_LAST);
				unsigned int count : 3;
				bool normalized    : 1;
			};

			struct Output
			{
				union
				{
					unsigned char write : 4;

					struct
					{
						unsigned char xWrite : 1;
						unsigned char yWrite : 1;
						unsigned char zWrite : 1;
						unsigned char wWrite : 1;
					};
				};

				union
				{
					unsigned char clamp : 4;

					struct
					{
						unsigned char xClamp : 1;
						unsigned char yClamp : 1;
						unsigned char zClamp : 1;
						unsigned char wClamp : 1;
					};
				};
			};

			Input input[VERTEX_ATTRIBUTES];
			Output output[12];
		};

		struct State : States
		{
			State();

			bool operator==(const State &state) const;

			unsigned int hash;
		};

		struct FixedFunction
		{
			float4 transformT[12][4];
			float4 cameraTransformT[12][4];
			float4 normalTransformT[12][4];
			float4 textureTransform[8][4];

			float4 lightPosition[8];
			float4 lightAmbient[8];
			float4 lightSpecular[8];
			float4 lightDiffuse[8];
			float4 attenuationConstant[8];
			float4 attenuationLinear[8];
			float4 attenuationQuadratic[8];
			float lightRange[8];
			float4 materialDiffuse;
			float4 materialSpecular;
			float materialShininess;
			float4 globalAmbient;
			float4 materialEmission;
			float4 materialAmbient;
		};

		struct PointSprite
		{
			float4 pointSize;
			float pointSizeMin;
			float pointSizeMax;
			float pointScaleA;
			float pointScaleB;
			float pointScaleC;
		};

		typedef void (*RoutinePointer)(Vertex *output, unsigned int *batch, VertexTask *vertexTask, DrawData *draw);

		VertexProcessor(Context *context);

		virtual ~VertexProcessor();

		virtual void setInputStream(int index, const Stream &stream);
		virtual void resetInputStreams(bool preTransformed);

		virtual void setFloatConstant(unsigned int index, const float value[4]);
		virtual void setIntegerConstant(unsigned int index, const int integer[4]);
		virtual void setBooleanConstant(unsigned int index, int boolean);

		virtual void setUniformBuffer(int index, sw::Resource* uniformBuffer, int offset);
		virtual void lockUniformBuffers(byte** u, sw::Resource* uniformBuffers[]);

		virtual void setTransformFeedbackBuffer(int index, sw::Resource* transformFeedbackBuffer, int offset, unsigned int reg, unsigned int row, unsigned int col, size_t stride);
		virtual void lockTransformFeedbackBuffers(byte** t, unsigned int* v, unsigned int* r, unsigned int* c, unsigned int* s, sw::Resource* transformFeedbackBuffers[]);

		// Transformations
		virtual void setModelMatrix(const Matrix &M, int i = 0);
		virtual void setViewMatrix(const Matrix &V);
		virtual void setBaseMatrix(const Matrix &B);
		virtual void setProjectionMatrix(const Matrix &P);

		// Lighting
		virtual void setLightingEnable(bool lightingEnable);
		virtual void setLightEnable(unsigned int light, bool lightEnable);
		virtual void setSpecularEnable(bool specularEnable);

		virtual void setGlobalAmbient(const Color<float> &globalAmbient);
		virtual void setLightPosition(unsigned int light, const Point &lightPosition);
		virtual void setLightViewPosition(unsigned int light, const Point &lightPosition);
		virtual void setLightDiffuse(unsigned int light, const Color<float> &lightDiffuse);
		virtual void setLightSpecular(unsigned int light, const Color<float> &lightSpecular);
		virtual void setLightAmbient(unsigned int light, const Color<float> &lightAmbient);
		virtual void setLightAttenuation(unsigned int light, float constant, float linear, float quadratic);
		virtual void setLightRange(unsigned int light, float lightRange);

		virtual void setInstanceID(int instanceID);

		virtual void setFogEnable(bool fogEnable);
		virtual void setVertexFogMode(FogMode fogMode);
		virtual void setRangeFogEnable(bool enable);

		virtual void setColorVertexEnable(bool colorVertexEnable);
		virtual void setDiffuseMaterialSource(MaterialSource diffuseMaterialSource);
		virtual void setSpecularMaterialSource(MaterialSource specularMaterialSource);
		virtual void setAmbientMaterialSource(MaterialSource ambientMaterialSource);
		virtual void setEmissiveMaterialSource(MaterialSource emissiveMaterialSource);

		virtual void setMaterialEmission(const Color<float> &emission);
		virtual void setMaterialAmbient(const Color<float> &materialAmbient);
		virtual void setMaterialDiffuse(const Color<float> &diffuseColor);
		virtual void setMaterialSpecular(const Color<float> &specularColor);
		virtual void setMaterialShininess(float specularPower);

		virtual void setIndexedVertexBlendEnable(bool indexedVertexBlendEnable);
		virtual void setVertexBlendMatrixCount(unsigned int vertexBlendMatrixCount);

		virtual void setTextureWrap(unsigned int stage, int mask);
		virtual void setTexGen(unsigned int stage, TexGen texGen);
		virtual void setLocalViewer(bool localViewer);
		virtual void setNormalizeNormals(bool normalizeNormals);
		virtual void setTextureMatrix(int stage, const Matrix &T);
		virtual void setTextureTransform(int stage, int count, bool project);

		virtual void setTextureFilter(unsigned int sampler, FilterType textureFilter);
		virtual void setMipmapFilter(unsigned int sampler, MipmapType mipmapFilter);
		virtual void setGatherEnable(unsigned int sampler, bool enable);
		virtual void setAddressingModeU(unsigned int sampler, AddressingMode addressingMode);
		virtual void setAddressingModeV(unsigned int sampler, AddressingMode addressingMode);
		virtual void setAddressingModeW(unsigned int sampler, AddressingMode addressingMode);
		virtual void setReadSRGB(unsigned int sampler, bool sRGB);
		virtual void setMipmapLOD(unsigned int sampler, float bias);
		virtual void setBorderColor(unsigned int sampler, const Color<float> &borderColor);
		virtual void setMaxAnisotropy(unsigned int stage, float maxAnisotropy);
		virtual void setSwizzleR(unsigned int sampler, SwizzleType swizzleR);
		virtual void setSwizzleG(unsigned int sampler, SwizzleType swizzleG);
		virtual void setSwizzleB(unsigned int sampler, SwizzleType swizzleB);
		virtual void setSwizzleA(unsigned int sampler, SwizzleType swizzleA);

		virtual void setPointSize(float pointSize);
		virtual void setPointSizeMin(float pointSizeMin);
		virtual void setPointSizeMax(float pointSizeMax);
		virtual void setPointScaleA(float pointScaleA);
		virtual void setPointScaleB(float pointScaleB);
		virtual void setPointScaleC(float pointScaleC);

		virtual void setTransformFeedbackQueryEnabled(bool enable);
		virtual void enableTransformFeedback(uint64_t enable);

	protected:
		const Matrix &getModelTransform(int i);
		const Matrix &getViewTransform();

		const State update();
		Routine *routine(const State &state);

		bool isFixedFunction();
		void setRoutineCacheSize(int cacheSize);

		// Shader constants
		float4 c[VERTEX_UNIFORM_VECTORS + 1];   // One extra for indices out of range, c[VERTEX_UNIFORM_VECTORS] = {0, 0, 0, 0}
		int4 i[16];
		bool b[16];

		PointSprite point;
		FixedFunction ff;

	private:
		struct UniformBufferInfo
		{
			UniformBufferInfo();

			Resource* buffer;
			int offset;
		};
		UniformBufferInfo uniformBufferInfo[MAX_UNIFORM_BUFFER_BINDINGS];

		struct TransformFeedbackInfo
		{
			TransformFeedbackInfo();

			Resource* buffer;
			int offset;
			int reg;
			int row;
			int col;
			size_t stride;
		};
		TransformFeedbackInfo transformFeedbackInfo[MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS];

		void updateTransform();
		void setTransform(const Matrix &M, int i);
		void setCameraTransform(const Matrix &M, int i);
		void setNormalTransform(const Matrix &M, int i);

		Context *const context;

		RoutineCache<State> *routineCache;

	protected:
		Matrix M[12];      // Model/Geometry/World matrix
		Matrix V;          // View/Camera/Eye matrix
		Matrix B;          // Base matrix
		Matrix P;          // Projection matrix
		Matrix PB;         // P * B
		Matrix PBV;        // P * B * V
		Matrix PBVM[12];   // P * B * V * M

		// Update hierarchy
		bool updateMatrix;
		bool updateModelMatrix[12];
		bool updateViewMatrix;
		bool updateBaseMatrix;
		bool updateProjectionMatrix;
		bool updateLighting;
	};
}

#endif   // sw_VertexProcessor_hpp
