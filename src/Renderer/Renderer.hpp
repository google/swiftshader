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

#ifndef sw_Renderer_hpp
#define sw_Renderer_hpp

#include "VertexProcessor.hpp"
#include "PixelProcessor.hpp"
#include "SetupProcessor.hpp"
#include "Plane.hpp"
#include "Blitter.hpp"
#include "Common/MutexLock.hpp"
#include "Common/Thread.hpp"
#include "Main/Config.hpp"

#include <list>

namespace sw
{
	class Clipper;
	class PixelShader;
	class VertexShader;
	class SwiftConfig;
	struct Task;
	class Resource;
	class Renderer;
	struct Constants;

	extern int batchSize;
	extern int threadCount;
	extern int unitCount;
	extern int clusterCount;

	enum TranscendentalPrecision
	{
		APPROXIMATE,
		PARTIAL,	// 2^-10
		ACCURATE,
		WHQL,		// 2^-21
		IEEE		// 2^-23
	};

	extern TranscendentalPrecision logPrecision;
	extern TranscendentalPrecision expPrecision;
	extern TranscendentalPrecision rcpPrecision;
	extern TranscendentalPrecision rsqPrecision;
	extern bool perspectiveCorrection;

	struct Conventions
	{
		bool halfIntegerCoordinates;
		bool symmetricNormalizedDepth;
		bool booleanFaceRegister;
		bool fullPixelPositionRegister;
		bool leadingVertexFirst;
		bool secondaryColor;
	};

	static const Conventions OpenGL =
	{
		true,    // halfIntegerCoordinates
		true,    // symmetricNormalizedDepth
		true,    // booleanFaceRegister
		true,    // fullPixelPositionRegister
		false,   // leadingVertexFirst
		false    // secondaryColor
	};

	static const Conventions Direct3D =
	{
		false,   // halfIntegerCoordinates
		false,   // symmetricNormalizedDepth
		false,   // booleanFaceRegister
		false,   // fullPixelPositionRegister
		true,    // leadingVertexFirst
		true,    // secondardyColor
	};

	struct Query
	{
		Query()
		{
			building = false;
			reference = 0;
			data = 0;
		}

		void begin()
		{
			building = true;
			data = 0;
		}

		void end()
		{
			building = false;
		}

		bool building;
		volatile int reference;
		volatile unsigned int data;
	};

	struct DrawData
	{
		const Constants *constants;

		const void *input[VERTEX_ATTRIBUTES];
		unsigned int stride[VERTEX_ATTRIBUTES];
		Texture mipmap[TOTAL_IMAGE_UNITS];
		const void *indices;

		struct VS
		{
			float4 c[VERTEX_UNIFORM_VECTORS + 1];   // One extra for indices out of range, c[VERTEX_UNIFORM_VECTORS] = {0, 0, 0, 0}
			int4 i[16];
			bool b[16];
		};

		struct PS
		{
			word4 cW[8][4];
			float4 c[FRAGMENT_UNIFORM_VECTORS];
			int4 i[16];
			bool b[16];
		};

		union
		{
			VS vs;
			VertexProcessor::FixedFunction ff;
		};

		PS ps;

		int instanceID;

		VertexProcessor::PointSprite point;
		float lineWidth;

		PixelProcessor::Stencil stencil[2];   // clockwise, counterclockwise
		PixelProcessor::Stencil stencilCCW;
		PixelProcessor::Fog fog;
		PixelProcessor::Factor factor;
		unsigned int occlusion[16];   // Number of pixels passing depth test

		#if PERF_PROFILE
			int64_t cycles[PERF_TIMERS][16];
		#endif

		TextureStage::Uniforms textureStage[8];

		float4 Wx16;
		float4 Hx16;
		float4 X0x16;
		float4 Y0x16;
		float4 XXXX;
		float4 YYYY;
		float4 halfPixelX;
		float4 halfPixelY;
		float viewportHeight;
		float slopeDepthBias;
		float depthRange;
		float depthNear;
		Plane clipPlane[6];

		unsigned int *colorBuffer[RENDERTARGETS];
		int colorPitchB[RENDERTARGETS];
		int colorSliceB[RENDERTARGETS];
		float *depthBuffer;
		int depthPitchB;
		int depthSliceB;
		unsigned char *stencilBuffer;
		int stencilPitchB;
		int stencilSliceB;

		int scissorX0;
		int scissorX1;
		int scissorY0;
		int scissorY1;

		float4 a2c0;
		float4 a2c1;
		float4 a2c2;
		float4 a2c3;
	};

	struct DrawCall
	{
		DrawCall();

		~DrawCall();

		DrawType drawType;
		int batchSize;

		Routine *vertexRoutine;
		Routine *setupRoutine;
		Routine *pixelRoutine;

		VertexProcessor::RoutinePointer vertexPointer;
		SetupProcessor::RoutinePointer setupPointer;
		PixelProcessor::RoutinePointer pixelPointer;

		int (*setupPrimitives)(Renderer *renderer, int batch, int count);
		SetupProcessor::State setupState;

		Resource *vertexStream[VERTEX_ATTRIBUTES];
		Resource *indexBuffer;
		Surface *renderTarget[RENDERTARGETS];
		Surface *depthStencil;
		Resource *texture[TOTAL_IMAGE_UNITS];

		int vsDirtyConstF;
		int vsDirtyConstI;
		int vsDirtyConstB;

		int psDirtyConstF;
		int psDirtyConstI;
		int psDirtyConstB;

		std::list<Query*> *queries;

		int clipFlags;

		volatile int primitive;    // Current primitive to enter pipeline
		volatile int count;        // Number of primitives to render
		volatile int references;   // Remaining references to this draw call, 0 when done drawing, -1 when resources unlocked and slot is free

		DrawData *data;
	};

	struct Viewport
	{
		float x0;
		float y0;
		float width;
		float height;
		float minZ;
		float maxZ;
	};

	class Renderer : public VertexProcessor, public PixelProcessor, public SetupProcessor
	{
		struct Task
		{
			enum Type
			{
				PRIMITIVES,
				PIXELS,

				RESUME,
				SUSPEND
			};

			volatile Type type;
			volatile int primitiveUnit;
			volatile int pixelCluster;
		};

		struct PrimitiveProgress
		{
			void init()
			{
				drawCall = 0;
				firstPrimitive = 0;
				primitiveCount = 0;
				visible = 0;
				references = 0;
			}

			volatile int drawCall;
			volatile int firstPrimitive;
			volatile int primitiveCount;
			volatile int visible;
			volatile int references;
		};

		struct PixelProgress
		{
			void init()
			{
				drawCall = 0;
				processedPrimitives = 0;
				executing = false;
			}

			volatile int drawCall;
			volatile int processedPrimitives;
			volatile bool executing;
		};

	public:
		Renderer(Context *context, Conventions conventions, bool exactColorRounding);

		virtual ~Renderer();

		virtual void clear(void* pixel, Format format, Surface *dest, const SliceRect &dRect, unsigned int rgbaMask);
		virtual void blit(Surface *source, const SliceRect &sRect, Surface *dest, const SliceRect &dRect, bool filter);
		virtual void blit3D(Surface *source, Surface *dest);
		virtual void draw(DrawType drawType, unsigned int indexOffset, unsigned int count, bool update = true);

		virtual void setIndexBuffer(Resource *indexBuffer);

		virtual void setMultiSampleMask(unsigned int mask);
		virtual void setTransparencyAntialiasing(TransparencyAntialiasing transparencyAntialiasing);

		virtual void setTextureResource(unsigned int sampler, Resource *resource);
		virtual void setTextureLevel(unsigned int sampler, unsigned int face, unsigned int level, Surface *surface, TextureType type);

		virtual void setTextureFilter(SamplerType type, int sampler, FilterType textureFilter);
		virtual void setMipmapFilter(SamplerType type, int sampler, MipmapType mipmapFilter);
		virtual void setGatherEnable(SamplerType type, int sampler, bool enable);
		virtual void setAddressingModeU(SamplerType type, int sampler, AddressingMode addressingMode);
		virtual void setAddressingModeV(SamplerType type, int sampler, AddressingMode addressingMode);
		virtual void setAddressingModeW(SamplerType type, int sampler, AddressingMode addressingMode);
		virtual void setReadSRGB(SamplerType type, int sampler, bool sRGB);
		virtual void setMipmapLOD(SamplerType type, int sampler, float bias);
		virtual void setBorderColor(SamplerType type, int sampler, const Color<float> &borderColor);
		virtual void setMaxAnisotropy(SamplerType type, int sampler, float maxAnisotropy);
		virtual void setSwizzleR(SamplerType type, int sampler, SwizzleType swizzleR);
		virtual void setSwizzleG(SamplerType type, int sampler, SwizzleType swizzleG);
		virtual void setSwizzleB(SamplerType type, int sampler, SwizzleType swizzleB);
		virtual void setSwizzleA(SamplerType type, int sampler, SwizzleType swizzleA);
		
		virtual void setPointSpriteEnable(bool pointSpriteEnable);
		virtual void setPointScaleEnable(bool pointScaleEnable);
		virtual void setLineWidth(float width);

		virtual void setDepthBias(float bias);
		virtual void setSlopeDepthBias(float slopeBias);

		// Programmable pipelines
		virtual void setPixelShader(const PixelShader *shader);
		virtual void setVertexShader(const VertexShader *shader);

		virtual void setPixelShaderConstantF(int index, const float value[4], int count = 1);
		virtual void setPixelShaderConstantI(int index, const int value[4], int count = 1);
		virtual void setPixelShaderConstantB(int index, const int *boolean, int count = 1);

		virtual void setVertexShaderConstantF(int index, const float value[4], int count = 1);
		virtual void setVertexShaderConstantI(int index, const int value[4], int count = 1);
		virtual void setVertexShaderConstantB(int index, const int *boolean, int count = 1);

		// Viewport & Clipper
		virtual void setViewport(const Viewport &viewport);
		virtual void setScissor(const Rect &scissor);
		virtual void setClipFlags(int flags);
		virtual void setClipPlane(unsigned int index, const float plane[4]);

		// Partial transform
		virtual void setModelMatrix(const Matrix &M, int i = 0);
		virtual void setViewMatrix(const Matrix &V);
		virtual void setBaseMatrix(const Matrix &B);
		virtual void setProjectionMatrix(const Matrix &P);

		virtual void addQuery(Query *query);
		virtual void removeQuery(Query *query);

		void synchronize();

		#if PERF_HUD
			// Performance timers
			int getThreadCount();
			int64_t getVertexTime(int thread);
			int64_t getSetupTime(int thread);
			int64_t getPixelTime(int thread);
			void resetTimers();
		#endif

	private:
		static void threadFunction(void *parameters);
		void threadLoop(int threadIndex);
		void taskLoop(int threadIndex);
		void findAvailableTasks();
		void scheduleTask(int threadIndex);
		void executeTask(int threadIndex);
		void finishRendering(Task &pixelTask);

		void processPrimitiveVertices(int unit, unsigned int start, unsigned int count, unsigned int loop, int thread);

		static int setupSolidTriangles(Renderer *renderer, int batch, int count);
		static int setupWireframeTriangle(Renderer *renderer, int batch, int count);
		static int setupVertexTriangle(Renderer *renderer, int batch, int count);
		static int setupLines(Renderer *renderer, int batch, int count);
		static int setupPoints(Renderer *renderer, int batch, int count);

		static bool setupLine(Renderer *renderer, Primitive &primitive, Triangle &triangle, const DrawCall &draw);
		static bool setupPoint(Renderer *renderer, Primitive &primitive, Triangle &triangle, const DrawCall &draw);

		bool isReadWriteTexture(int sampler);
		void updateClipper();
		void updateConfiguration(bool initialUpdate = false);
		static unsigned int computeClipFlags(const float4 &v, const DrawData &data);
		void initializeThreads();
		void terminateThreads();

		void loadConstants(const VertexShader *vertexShader);
		void loadConstants(const PixelShader *pixelShader);

		Context *context;
		Clipper *clipper;
		Viewport viewport;
		Rect scissor;
		int clipFlags;

		Triangle *triangleBatch[16];
		Primitive *primitiveBatch[16];

		// User-defined clipping planes
		Plane userPlane[MAX_CLIP_PLANES];
		Plane clipPlane[MAX_CLIP_PLANES];   // Tranformed to clip space
		bool updateClipPlanes;

		volatile bool exitThreads;
		volatile int threadsAwake;
		Thread *worker[16];
		Event *resume[16];         // Events for resuming threads
		Event *suspend[16];        // Events for suspending threads
		Event *resumeApp;          // Event for resuming the application thread

		PrimitiveProgress primitiveProgress[16];
		PixelProgress pixelProgress[16];
		Task task[16];   // Current tasks for threads

		enum {DRAW_COUNT = 16};   // Number of draw calls buffered
		DrawCall *drawCall[DRAW_COUNT];
		DrawCall *drawList[DRAW_COUNT];

		volatile int currentDraw;
		volatile int nextDraw;

		Task taskQueue[32];
		unsigned int qHead;
		unsigned int qSize;

		BackoffLock schedulerMutex;

		#if PERF_HUD
			int64_t vertexTime[16];
			int64_t setupTime[16];
			int64_t pixelTime[16];
		#endif

		VertexTask *vertexTask[16];

		SwiftConfig *swiftConfig;

		std::list<Query*> queries;
		Resource *sync;

		VertexProcessor::State vertexState;
		SetupProcessor::State setupState;
		PixelProcessor::State pixelState;
	};
}

#endif   // sw_Renderer_hpp
