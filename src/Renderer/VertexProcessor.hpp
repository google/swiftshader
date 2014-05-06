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

#ifndef sw_VertexProcessor_hpp
#define sw_VertexProcessor_hpp

#include "Matrix.hpp"
#include "Context.hpp"
#include "LRUCache.hpp"

namespace sw
{
	class Viewport;
	class Routine;
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
		unsigned int count;
		VertexCache vertexCache;
	};

	class VertexProcessor
	{
	public:
		struct States
		{
			unsigned int computeHash();

			uint64_t shaderHash;

			unsigned int fixedFunction						: 1;
			unsigned int shaderContainsTexldl				: 1;
			unsigned int positionRegister					: 4;
			unsigned int pointSizeRegister					: 4;   // 0xF signifies no vertex point size
				
			unsigned int vertexBlendMatrixCount				: 3;
			unsigned int indexedVertexBlendEnable			: 1;
			unsigned int vertexNormalActive					: 1;
			unsigned int normalizeNormals					: 1;
			unsigned int vertexLightingActive				: 1;
			unsigned int diffuseActive						: 1;
			unsigned int specularActive						: 1;
			unsigned int vertexSpecularActive				: 1;
			unsigned int vertexLightActive					: 8;
			unsigned int vertexDiffuseMaterialSourceActive	: BITS(Context::MATERIAL_LAST);
			unsigned int vertexSpecularMaterialSourceActive	: BITS(Context::MATERIAL_LAST);
			unsigned int vertexAmbientMaterialSourceActive	: BITS(Context::MATERIAL_LAST);
			unsigned int vertexEmissiveMaterialSourceActive	: BITS(Context::MATERIAL_LAST);
			unsigned int fogActive							: 1;
			unsigned int vertexFogMode						: BITS(Context::FOG_LAST);
			unsigned int rangeFogActive						: 1;
			unsigned int localViewerActive					: 1;
			unsigned int pointSizeActive					: 1;
			unsigned int pointScaleActive					: 1;

			unsigned int preTransformed						: 1;
			unsigned int postTransform						: 1;
			unsigned int superSampling						: 1;
			unsigned int multiSampling						: 1;

			struct TextureState
			{
				unsigned char texGenActive : BITS(Context::TEXGEN_LAST);
				unsigned char textureTransformCountActive : 3;
				unsigned char texCoordIndexActive : 3;
			};

			TextureState textureState[8];

			Sampler::State samplerState[4];

			struct Input
			{
				operator bool() const   // Returns true if stream contains data
				{
					return count != 0;
				}

				unsigned int type       : BITS(STREAMTYPE_LAST);
				unsigned int count      : 3;
				unsigned int normalized : 1;
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

			Input input[16];
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

		typedef void (__cdecl *RoutinePointer)(Vertex *output, unsigned int *batch, VertexTask *vertexTask, DrawData *draw);

		VertexProcessor(Context *context);

		virtual ~VertexProcessor();

		virtual void setInputStream(int index, const Stream &stream);

		virtual void setInputPositionStream(const Stream &stream);
		virtual void setInputBlendWeightStream(const Stream &stream);
		virtual void setInputBlendIndicesStream(const Stream &stream);
		virtual void setInputNormalStream(const Stream &stream);
		virtual void setInputPSizeStream(const Stream &stream);
		virtual void setInputTexCoordStream(const Stream &stream, int index);
		virtual void setInputPositiontStream(const Stream &stream);
		virtual void setInputColorStream(const Stream &stream, int index);

		virtual void resetInputStreams(bool preTransformed);

		virtual void setFloatConstant(unsigned int index, const float value[4]);
		virtual void setIntegerConstant(unsigned int index, const int integer[4]);
		virtual void setBooleanConstant(unsigned int index, int boolean);

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

		virtual void setFogEnable(bool fogEnable);
		virtual void setVertexFogMode(Context::FogMode fogMode);
		virtual void setRangeFogEnable(bool enable);

		virtual void setColorVertexEnable(bool colorVertexEnable);
		virtual void setDiffuseMaterialSource(Context::MaterialSource diffuseMaterialSource);
		virtual void setSpecularMaterialSource(Context::MaterialSource specularMaterialSource);
		virtual void setAmbientMaterialSource(Context::MaterialSource ambientMaterialSource);
		virtual void setEmissiveMaterialSource(Context::MaterialSource emissiveMaterialSource);

		virtual void setMaterialEmission(const Color<float> &emission);
		virtual void setMaterialAmbient(const Color<float> &materialAmbient);
		virtual void setMaterialDiffuse(const Color<float> &diffuseColor);
		virtual void setMaterialSpecular(const Color<float> &specularColor);
		virtual void setMaterialShininess(float specularPower);

		virtual void setIndexedVertexBlendEnable(bool indexedVertexBlendEnable);
		virtual void setVertexBlendMatrixCount(unsigned int vertexBlendMatrixCount);

		virtual void setTextureWrap(unsigned int stage, int mask);
		virtual void setTexGen(unsigned int stage, Context::TexGen texGen);
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
		virtual void setMaxAnisotropy(unsigned int stage, unsigned int maxAnisotropy);

		virtual void setPointSize(float pointSize);
		virtual void setPointSizeMin(float pointSizeMin);
		virtual void setPointSizeMax(float pointSizeMax);
		virtual void setPointScaleA(float pointScaleA);
		virtual void setPointScaleB(float pointScaleB);
		virtual void setPointScaleC(float pointScaleC);

	protected:
		const Matrix &getModelTransform(int i);
		const Matrix &getViewTransform();

		const State update();
		Routine *routine(const State &state);

		bool isFixedFunction();
		void setRoutineCacheSize(int cacheSize);

		// Shader constants
		float4 c[256 + 1];   // One extra for indices out of range, c[256] = {0, 0, 0, 0}
		int4 i[16];
		bool b[16];

		PointSprite point;
		FixedFunction ff;

	private:
		void updateTransform();

		void setTransform(const Matrix &M, int i);
		void setCameraTransform(const Matrix &M, int i);
		void setNormalTransform(const Matrix &M, int i);

		Context *const context;

		LRUCache<State, Routine> *routineCache;

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
