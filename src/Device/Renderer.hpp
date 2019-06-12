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

#ifndef sw_Renderer_hpp
#define sw_Renderer_hpp

#include "VertexProcessor.hpp"
#include "PixelProcessor.hpp"
#include "SetupProcessor.hpp"
#include "Plane.hpp"
#include "Blitter.hpp"
#include "Device/Config.hpp"
#include "System/Synchronization.hpp"
#include "Vulkan/VkDescriptorSet.hpp"

#include <atomic>
#include <list>
#include <mutex>
#include <thread>

namespace vk
{
	class DescriptorSet;
	class Query;
}

namespace sw
{
	struct DrawCall;
	class PixelShader;
	class VertexShader;
	struct Task;
	class TaskEvents;
	class Resource;
	struct Constants;

	struct DrawData
	{
		const Constants *constants;

		vk::DescriptorSet::Bindings descriptorSets = {};
		vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};

		const void *input[MAX_VERTEX_INPUTS];
		unsigned int stride[MAX_VERTEX_INPUTS];
		const void *indices;

		int instanceID;
		int baseVertex;
		float lineWidth;

		PixelProcessor::Stencil stencil[2];   // clockwise, counterclockwise
		PixelProcessor::Factor factor;
		unsigned int occlusion[16];   // Number of pixels passing depth test

		float4 Wx16;
		float4 Hx16;
		float4 X0x16;
		float4 Y0x16;
		float4 halfPixelX;
		float4 halfPixelY;
		float viewportHeight;
		float slopeDepthBias;
		float depthRange;
		float depthNear;

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

		PushConstantStorage pushConstants;
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

			void operator=(const Task& task)
			{
				type = task.type.load();
				primitiveUnit = task.primitiveUnit.load();
				pixelCluster = task.pixelCluster.load();
			}

			std::atomic<int> type;
			std::atomic<int> primitiveUnit;
			std::atomic<int> pixelCluster;
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

			std::atomic<int> drawCall;
			std::atomic<int> firstPrimitive;
			std::atomic<int> primitiveCount;
			std::atomic<int> visible;
			std::atomic<int> references;
		};

		struct PixelProgress
		{
			void init()
			{
				drawCall = 0;
				processedPrimitives = 0;
				executing = false;
			}

			std::atomic<int> drawCall;
			std::atomic<int> processedPrimitives;
			std::atomic<int> executing;
		};

	public:
		Renderer();

		virtual ~Renderer();

		void *operator new(size_t size);
		void operator delete(void * mem);

		bool hasQueryOfType(VkQueryType type) const;

		void draw(const sw::Context* context, VkIndexType indexType, unsigned int count, int baseVertex, TaskEvents *events, bool update = true);

		// Viewport & Clipper
		void setViewport(const VkViewport &viewport);
		void setScissor(const VkRect2D &scissor);

		void addQuery(vk::Query *query);
		void removeQuery(vk::Query *query);

		void advanceInstanceAttributes(Stream* inputs);

		void synchronize();

		static int getClusterCount() { return clusterCount; }

	private:
		static void threadFunction(void *parameters);
		void threadLoop(int threadIndex);
		void taskLoop(int threadIndex);
		void findAvailableTasks();
		void scheduleTask(int threadIndex);
		void executeTask(int threadIndex);
		void finishRendering(Task &pixelTask);

		void processPrimitiveVertices(int unit, unsigned int start, unsigned int count, unsigned int loop, int thread);

		int setupTriangles(int batch, int count);
		int setupLines(int batch, int count);
		int setupPoints(int batch, int count);

		bool setupLine(Primitive &primitive, Triangle &triangle, const DrawCall &draw);
		bool setupPoint(Primitive &primitive, Triangle &triangle, const DrawCall &draw);

		void updateConfiguration(bool initialUpdate = false);
		void initializeThreads();
		void terminateThreads();

		VkViewport viewport;
		VkRect2D scissor;

		Triangle *triangleBatch[16];
		Primitive *primitiveBatch[16];

		std::atomic<int> exitThreads;
		std::atomic<int> threadsAwake;
		std::thread *worker[16];
		Event *resume[16];         // Events for resuming threads
		Event *suspend[16];        // Events for suspending threads
		Event *resumeApp;          // Event for resuming the application thread

		PrimitiveProgress primitiveProgress[16];
		PixelProgress pixelProgress[16];
		Task task[16];   // Current tasks for threads

		enum {
			DRAW_COUNT = 16,   // Number of draw calls buffered (must be power of 2)
			DRAW_COUNT_BITS = DRAW_COUNT - 1,
		};
		DrawCall *drawCall[DRAW_COUNT];
		DrawCall *drawList[DRAW_COUNT];

		std::atomic<int> currentDraw;
		std::atomic<int> nextDraw;

		enum {
			TASK_COUNT = 32,   // Size of the task queue (must be power of 2)
			TASK_COUNT_BITS = TASK_COUNT - 1,
		};
		Task taskQueue[TASK_COUNT];
		std::atomic<int> qHead;
		std::atomic<int> qSize;

		static std::atomic<int> unitCount;
		static std::atomic<int> clusterCount;

		std::mutex schedulerMutex;

		VertexTask *vertexTask[16];

		std::list<vk::Query*> queries;
		WaitGroup sync;

		VertexProcessor::State vertexState;
		SetupProcessor::State setupState;
		PixelProcessor::State pixelState;

		Routine *vertexRoutine;
		Routine *setupRoutine;
		Routine *pixelRoutine;
	};

	struct DrawCall
	{
		DrawCall();

		~DrawCall();

		std::atomic<int> topology;
		std::atomic<int> indexType;
		std::atomic<int> batchSize;

		Routine *vertexRoutine;
		Routine *setupRoutine;
		Routine *pixelRoutine;

		VertexProcessor::RoutinePointer vertexPointer;
		SetupProcessor::RoutinePointer setupPointer;
		PixelProcessor::RoutinePointer pixelPointer;

		int (Renderer::*setupPrimitives)(int batch, int count);
		SetupProcessor::State setupState;

		vk::ImageView *renderTarget[RENDERTARGETS];
		vk::ImageView *depthBuffer;
		vk::ImageView *stencilBuffer;
		TaskEvents *events;

		std::list<vk::Query*> *queries;

		std::atomic<int> primitive;    // Current primitive to enter pipeline
		std::atomic<int> count;        // Number of primitives to render
		std::atomic<int> references;   // Remaining references to this draw call, 0 when done drawing, -1 when resources unlocked and slot is free

		DrawData *data;
	};
}

#endif   // sw_Renderer_hpp
