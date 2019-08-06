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

#include "Renderer.hpp"

#include "Clipper.hpp"
#include "Primitive.hpp"
#include "Polygon.hpp"
#include "Reactor/Reactor.hpp"
#include "Pipeline/Constants.hpp"
#include "System/CPUID.hpp"
#include "System/Memory.hpp"
#include "System/Half.hpp"
#include "System/Math.hpp"
#include "System/Timer.hpp"
#include "Vulkan/VkConfig.h"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkDevice.hpp"
#include "Vulkan/VkFence.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Vulkan/VkQueryPool.hpp"
#include "Pipeline/SpirvShader.hpp"
#include "Vertex.hpp"

#undef max

#ifndef NDEBUG
unsigned int minPrimitives = 1;
unsigned int maxPrimitives = 1 << 21;
#endif

namespace sw
{
	static const int batchSize = 128;
	std::atomic<int> threadCount(1);
	std::atomic<int> Renderer::unitCount(1);
	std::atomic<int> Renderer::clusterCount(1);

	template<typename T>
	inline bool setBatchIndices(unsigned int batch[128][3], VkPrimitiveTopology topology, T indices, unsigned int start, unsigned int triangleCount)
	{
		switch(topology)
		{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		{
			auto index = start;
			for(unsigned int i = 0; i < triangleCount; i++)
			{
				batch[i][0] = indices[index];
				batch[i][1] = indices[index];
				batch[i][2] = indices[index];

				index += 1;
			}
			break;
		}
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		{
			auto index = 2 * start;
			for(unsigned int i = 0; i < triangleCount; i++)
			{
				batch[i][0] = indices[index + 0];
				batch[i][1] = indices[index + 1];
				batch[i][2] = indices[index + 1];

				index += 2;
			}
			break;
		}
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		{
			auto index = start;
			for(unsigned int i = 0; i < triangleCount; i++)
			{
				batch[i][0] = indices[index + 0];
				batch[i][1] = indices[index + 1];
				batch[i][2] = indices[index + 1];

				index += 1;
			}
			break;
		}
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		{
			auto index = 3 * start;
			for(unsigned int i = 0; i < triangleCount; i++)
			{
				batch[i][0] = indices[index + 0];
				batch[i][1] = indices[index + 1];
				batch[i][2] = indices[index + 2];

				index += 3;
			}
			break;
		}
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		{
			auto index = start;
			for(unsigned int i = 0; i < triangleCount; i++)
			{
				batch[i][0] = indices[index + 0];
				batch[i][1] = indices[index + ((start + i) & 1) + 1];
				batch[i][2] = indices[index + (~(start + i) & 1) + 1];

				index += 1;
			}
			break;
		}
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		{
			auto index = start + 1;
			for(unsigned int i = 0; i < triangleCount; i++)
			{
				batch[i][0] = indices[index + 0];
				batch[i][1] = indices[index + 1];
				batch[i][2] = indices[0];

				index += 1;
			}
			break;
		}
		default:
			ASSERT(false);
			return false;
		}

		return true;
	}

	struct Parameters
	{
		Renderer *renderer;
		int threadIndex;
	};

	DrawCall::DrawCall()
	{
		occlusionQuery = nullptr;

		references = -1;

		events = nullptr;

		data = (DrawData*)allocate(sizeof(DrawData));
		data->constants = &constants;
	}

	DrawCall::~DrawCall()
	{
		deallocate(data);
	}

	Renderer::Renderer(vk::Device* device) : device(device)
	{
		for(int i = 0; i < 16; i++)
		{
			vertexTask[i] = nullptr;

			worker[i] = nullptr;
			resume[i] = nullptr;
			suspend[i] = nullptr;
		}

		threadsAwake = 0;
		resumeApp = new Event();

		currentDraw = 0;
		nextDraw = 0;

		qHead = 0;
		qSize = 0;

		for(int i = 0; i < 16; i++)
		{
			triangleBatch[i] = nullptr;
			primitiveBatch[i] = nullptr;
		}

		for(int draw = 0; draw < DRAW_COUNT; draw++)
		{
			drawCall[draw] = new DrawCall();
			drawList[draw] = drawCall[draw];
		}

		for(int unit = 0; unit < 16; unit++)
		{
			primitiveProgress[unit].init();
		}

		for(int cluster = 0; cluster < 16; cluster++)
		{
			pixelProgress[cluster].init();
		}

		updateConfiguration(true);
	}

	Renderer::~Renderer()
	{
		sync.wait();
		terminateThreads();

		delete resumeApp;
		resumeApp = nullptr;

		for(int draw = 0; draw < DRAW_COUNT; draw++)
		{
			delete drawCall[draw];
			drawCall[draw] = nullptr;
		}
	}

	// This object has to be mem aligned
	void* Renderer::operator new(size_t size)
	{
		ASSERT(size == sizeof(Renderer)); // This operator can't be called from a derived class
		return sw::allocate(sizeof(Renderer), 16);
	}

	void Renderer::operator delete(void * mem)
	{
		sw::deallocate(mem);
	}

	void Renderer::draw(const sw::Context* context, VkIndexType indexType, unsigned int count, int baseVertex, TaskEvents *events, bool update)
	{
		if(count == 0) { return; }

		#ifndef NDEBUG
		{
			unsigned int minPrimitives = 1;
			unsigned int maxPrimitives = 1 << 21;
			if(count < minPrimitives || count > maxPrimitives)
			{
				return;
			}
		}
		#endif

		updateConfiguration();

		int ms = context->sampleCount;

		if(!context->multiSampleMask)
		{
			return;
		}

		sync.add();

		if(update)
		{
			vertexState = VertexProcessor::update(context);
			setupState = SetupProcessor::update(context);
			pixelState = PixelProcessor::update(context);

			vertexRoutine = VertexProcessor::routine(vertexState, context->pipelineLayout, context->vertexShader, context->descriptorSets);
			setupRoutine = SetupProcessor::routine(setupState);
			pixelRoutine = PixelProcessor::routine(pixelState, context->pipelineLayout, context->pixelShader, context->descriptorSets);
		}

		int batch = batchSize / ms;

		int (Renderer::*setupPrimitives)(int batch, int count);

		if(context->isDrawTriangle())
		{
			setupPrimitives = &Renderer::setupTriangles;
		}
		else if(context->isDrawLine())
		{
			setupPrimitives = &Renderer::setupLines;
		}
		else   // Point draw
		{
			setupPrimitives = &Renderer::setupPoints;
		}

		DrawCall *draw = nullptr;

		do
		{
			for(int i = 0; i < DRAW_COUNT; i++)
			{
				if(drawCall[i]->references == -1)
				{
					draw = drawCall[i];
					drawList[nextDraw & DRAW_COUNT_BITS] = draw;

					break;
				}
			}

			if(!draw)
			{
				resumeApp->wait();
			}
		}
		while(!draw);

		DrawData *data = draw->data;

		if (occlusionQuery)
		{
			occlusionQuery->start();
		}
		draw->occlusionQuery = occlusionQuery;

		draw->topology = context->topology;
		draw->indexType = indexType;
		draw->batchSize = batch;

		draw->vertexRoutine = vertexRoutine;
		draw->setupRoutine = setupRoutine;
		draw->pixelRoutine = pixelRoutine;
		draw->vertexPointer = (VertexProcessor::RoutinePointer)vertexRoutine->getEntry();
		draw->setupPointer = (SetupProcessor::RoutinePointer)setupRoutine->getEntry();
		draw->pixelPointer = (PixelProcessor::RoutinePointer)pixelRoutine->getEntry();
		draw->setupPrimitives = setupPrimitives;
		draw->setupState = setupState;

		data->descriptorSets = context->descriptorSets;
		data->descriptorDynamicOffsets = context->descriptorDynamicOffsets;

		if(events)
		{
			events->start();
		}

		ASSERT(!draw->events);
		draw->events = events;

		for(int i = 0; i < MAX_INTERFACE_COMPONENTS/4; i++)
		{
			data->input[i] = context->input[i].buffer;
			data->stride[i] = context->input[i].vertexStride;
		}

		data->indices = context->indexBuffer;

		if(context->vertexShader->hasBuiltinInput(spv::BuiltInInstanceIndex))
		{
			data->instanceID = context->instanceID;
		}

		data->baseVertex = baseVertex;

		if(pixelState.stencilActive)
		{
			data->stencil[0].set(context->frontStencil.reference, context->frontStencil.compareMask, context->frontStencil.writeMask);
			data->stencil[1].set(context->backStencil.reference, context->backStencil.compareMask, context->backStencil.writeMask);
		}

		data->lineWidth = context->lineWidth;

		data->factor = factor;

		if(pixelState.alphaToCoverage)
		{
			if(ms == 4)
			{
				data->a2c0 = replicate(0.2f);
				data->a2c1 = replicate(0.4f);
				data->a2c2 = replicate(0.6f);
				data->a2c3 = replicate(0.8f);
			}
			else if(ms == 2)
			{
				data->a2c0 = replicate(0.25f);
				data->a2c1 = replicate(0.75f);
			}
			else ASSERT(false);
		}

		if(pixelState.occlusionEnabled)
		{
			for(int cluster = 0; cluster < clusterCount; cluster++)
			{
				data->occlusion[cluster] = 0;
			}
		}

		// Viewport
		{
			float W = 0.5f * viewport.width;
			float H = 0.5f * viewport.height;
			float X0 = viewport.x + W;
			float Y0 = viewport.y + H;
			float N = viewport.minDepth;
			float F = viewport.maxDepth;
			float Z = F - N;

			if(context->isDrawTriangle())
			{
				N += context->depthBias;
			}

			data->Wx16 = replicate(W * 16);
			data->Hx16 = replicate(H * 16);
			data->X0x16 = replicate(X0 * 16 - 8);
			data->Y0x16 = replicate(Y0 * 16 - 8);
			data->halfPixelX = replicate(0.5f / W);
			data->halfPixelY = replicate(0.5f / H);
			data->viewportHeight = abs(viewport.height);
			data->slopeDepthBias = context->slopeDepthBias;
			data->depthRange = Z;
			data->depthNear = N;
		}

		// Target
		{
			for(int index = 0; index < RENDERTARGETS; index++)
			{
				draw->renderTarget[index] = context->renderTarget[index];

				if(draw->renderTarget[index])
				{
					data->colorBuffer[index] = (unsigned int*)context->renderTarget[index]->getOffsetPointer({0, 0, 0}, VK_IMAGE_ASPECT_COLOR_BIT, 0, 0);
					data->colorPitchB[index] = context->renderTarget[index]->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
					data->colorSliceB[index] = context->renderTarget[index]->slicePitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
				}
			}

			draw->depthBuffer = context->depthBuffer;
			draw->stencilBuffer = context->stencilBuffer;

			if(draw->depthBuffer)
			{
				data->depthBuffer = (float*)context->depthBuffer->getOffsetPointer({0, 0, 0}, VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0);
				data->depthPitchB = context->depthBuffer->rowPitchBytes(VK_IMAGE_ASPECT_DEPTH_BIT, 0);
				data->depthSliceB = context->depthBuffer->slicePitchBytes(VK_IMAGE_ASPECT_DEPTH_BIT, 0);
			}

			if(draw->stencilBuffer)
			{
				data->stencilBuffer = (unsigned char*)context->stencilBuffer->getOffsetPointer({0, 0, 0}, VK_IMAGE_ASPECT_STENCIL_BIT, 0, 0);
				data->stencilPitchB = context->stencilBuffer->rowPitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
				data->stencilSliceB = context->stencilBuffer->slicePitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
			}
		}

		// Scissor
		{
			data->scissorX0 = scissor.offset.x;
			data->scissorX1 = scissor.offset.x + scissor.extent.width;
			data->scissorY0 = scissor.offset.y;
			data->scissorY1 = scissor.offset.y + scissor.extent.height;
		}

		// Push constants
		{
			data->pushConstants = context->pushConstants;
		}

		draw->primitive = 0;
		draw->count = count;

		draw->references = (count + batch - 1) / batch;

		schedulerMutex.lock();
		++nextDraw; // Atomic
		schedulerMutex.unlock();

		#ifndef NDEBUG
		if(threadCount == 1)   // Use main thread for draw execution
		{
			threadsAwake = 1;
			task[0].type = Task::RESUME;

			taskLoop(0);
		}
		else
		#endif
		{
			if(!threadsAwake)
			{
				suspend[0]->wait();

				threadsAwake = 1;
				task[0].type = Task::RESUME;

				resume[0]->signal();
			}
		}
	}

	void Renderer::threadFunction(void *parameters)
	{
		Renderer *renderer = static_cast<Parameters*>(parameters)->renderer;
		int threadIndex = static_cast<Parameters*>(parameters)->threadIndex;

		CPUID::setFlushToZero(true);
		CPUID::setDenormalsAreZero(true);

		renderer->threadLoop(threadIndex);
	}

	void Renderer::threadLoop(int threadIndex)
	{
		while(!exitThreads)
		{
			taskLoop(threadIndex);

			suspend[threadIndex]->signal();
			resume[threadIndex]->wait();
		}
	}

	void Renderer::taskLoop(int threadIndex)
	{
		while(task[threadIndex].type != Task::SUSPEND)
		{
			scheduleTask(threadIndex);
			executeTask(threadIndex);
		}
	}

	void Renderer::findAvailableTasks()
	{
		// Find pixel tasks
		for(int cluster = 0; cluster < clusterCount; cluster++)
		{
			if(!pixelProgress[cluster].executing)
			{
				for(int unit = 0; unit < unitCount; unit++)
				{
					if(primitiveProgress[unit].references > 0)   // Contains processed primitives
					{
						if(pixelProgress[cluster].drawCall == primitiveProgress[unit].drawCall)
						{
							if(pixelProgress[cluster].processedPrimitives == primitiveProgress[unit].firstPrimitive)   // Previous primitives have been rendered
							{
								Task &task = taskQueue[qHead];
								task.type = Task::PIXELS;
								task.primitiveUnit = unit;
								task.pixelCluster = cluster;

								pixelProgress[cluster].executing = true;

								// Commit to the task queue
								qHead = (qHead + 1) & TASK_COUNT_BITS;
								++qSize; // Atomic

								break;
							}
						}
					}
				}
			}
		}

		// Find primitive tasks
		if(currentDraw == nextDraw)
		{
			return;   // No more primitives to process
		}

		for(int unit = 0; unit < unitCount; unit++)
		{
			DrawCall *draw = drawList[currentDraw & DRAW_COUNT_BITS];

			int primitive = draw->primitive;
			int count = draw->count;

			if(primitive >= count)
			{
				++currentDraw; // Atomic

				if(currentDraw == nextDraw)
				{
					return;   // No more primitives to process
				}

				draw = drawList[currentDraw & DRAW_COUNT_BITS];
			}

			if(!primitiveProgress[unit].references)   // Task not already being executed and not still in use by a pixel unit
			{
				primitive = draw->primitive;
				count = draw->count;
				int batch = draw->batchSize;

				primitiveProgress[unit].drawCall = currentDraw.load();
				primitiveProgress[unit].firstPrimitive = primitive;
				primitiveProgress[unit].primitiveCount = count - primitive >= batch ? batch : count - primitive;

				draw->primitive += batch;

				Task &task = taskQueue[qHead];
				task.type = Task::PRIMITIVES;
				task.primitiveUnit = unit;

				primitiveProgress[unit].references = -1;

				// Commit to the task queue
				qHead = (qHead + 1) & TASK_COUNT_BITS;
				++qSize; // Atomic
			}
		}
	}

	void Renderer::scheduleTask(int threadIndex)
	{
		schedulerMutex.lock();

		int curThreadsAwake = threadsAwake;

		if((int)qSize < threadCount - curThreadsAwake + 1)
		{
			findAvailableTasks();
		}

		if(qSize != 0)
		{
			task[threadIndex] = taskQueue[(qHead - qSize) & TASK_COUNT_BITS];
			--qSize; // Atomic

			if(curThreadsAwake != threadCount)
			{
				int wakeup = qSize - curThreadsAwake + 1;

				for(int i = 0; i < threadCount && wakeup > 0; i++)
				{
					if(task[i].type == Task::SUSPEND)
					{
						suspend[i]->wait();
						task[i].type = Task::RESUME;
						resume[i]->signal();

						++threadsAwake; // Atomic
						wakeup--;
					}
				}
			}
		}
		else
		{
			task[threadIndex].type = Task::SUSPEND;

			--threadsAwake; // Atomic
		}

		schedulerMutex.unlock();
	}

	void Renderer::executeTask(int threadIndex)
	{
		switch(task[threadIndex].type.load())
		{
		case Task::PRIMITIVES:
			{
				int unit = task[threadIndex].primitiveUnit;

				int input = primitiveProgress[unit].firstPrimitive;
				int count = primitiveProgress[unit].primitiveCount;
				DrawCall *draw = drawList[primitiveProgress[unit].drawCall & DRAW_COUNT_BITS];
				int (Renderer::*setupPrimitives)(int batch, int count) = draw->setupPrimitives;

				processPrimitiveVertices(unit, input, count, draw->count, threadIndex);

				int visible = 0;

				if(!draw->setupState.rasterizerDiscard)
				{
					visible = (this->*setupPrimitives)(unit, count);
				}

				primitiveProgress[unit].visible = visible;
				primitiveProgress[unit].references = clusterCount.load();
			}
			break;
		case Task::PIXELS:
			{
				int unit = task[threadIndex].primitiveUnit;
				int visible = primitiveProgress[unit].visible;

				if(visible > 0)
				{
					int cluster = task[threadIndex].pixelCluster;
					Primitive *primitive = primitiveBatch[unit];
					DrawCall *draw = drawList[pixelProgress[cluster].drawCall & DRAW_COUNT_BITS];
					DrawData *data = draw->data;
					PixelProcessor::RoutinePointer pixelRoutine = draw->pixelPointer;

					pixelRoutine(primitive, visible, cluster, data);
				}

				finishRendering(task[threadIndex]);
			}
			break;
		case Task::RESUME:
			break;
		case Task::SUSPEND:
			break;
		default:
			ASSERT(false);
		}
	}

	void Renderer::synchronize()
	{
		sync.wait();
		device->updateSamplingRoutineConstCache();
	}

	void Renderer::finishRendering(Task &pixelTask)
	{
		int unit = pixelTask.primitiveUnit;
		int cluster = pixelTask.pixelCluster;

		DrawCall &draw = *drawList[primitiveProgress[unit].drawCall & DRAW_COUNT_BITS];
		DrawData &data = *draw.data;
		int primitive = primitiveProgress[unit].firstPrimitive;
		int count = primitiveProgress[unit].primitiveCount;
		int processedPrimitives = primitive + count;

		pixelProgress[cluster].processedPrimitives = processedPrimitives;

		if(pixelProgress[cluster].processedPrimitives >= draw.count)
		{
			++pixelProgress[cluster].drawCall; // Atomic
			pixelProgress[cluster].processedPrimitives = 0;
		}

		int ref = --primitiveProgress[unit].references; // Atomic

		if(ref == 0)
		{
			ref = --draw.references; // Atomic

			if(ref == 0)
			{
				if (draw.occlusionQuery)
				{
					for(int cluster = 0; cluster < clusterCount; cluster++)
					{
						draw.occlusionQuery->add(data.occlusion[cluster]);
					}
					draw.occlusionQuery->finish();
				}

				draw.vertexRoutine.reset();
				draw.setupRoutine.reset();
				draw.pixelRoutine.reset();

				if(draw.events)
				{
					draw.events->finish();
					draw.events = nullptr;
				}

				sync.done();

				draw.references = -1;
				resumeApp->signal();
			}
		}

		pixelProgress[cluster].executing = false;
	}

	void Renderer::processPrimitiveVertices(int unit, unsigned int start, unsigned int triangleCount, unsigned int loop, int thread)
	{
		Triangle *triangle = triangleBatch[unit];
		int primitiveDrawCall = primitiveProgress[unit].drawCall;
		DrawCall *draw = drawList[primitiveDrawCall & DRAW_COUNT_BITS];
		DrawData *data = draw->data;
		VertexTask *task = vertexTask[thread];

		const void *indices = data->indices;
		VertexProcessor::RoutinePointer vertexRoutine = draw->vertexPointer;

		if(task->vertexCache.drawCall != primitiveDrawCall)
		{
			task->vertexCache.clear();
			task->vertexCache.drawCall = primitiveDrawCall;
		}

		unsigned int batch[128 + 1][3];  // One extra for SIMD width overrun. TODO: Adjust to dynamic batch size.
		VkPrimitiveTopology topology = static_cast<VkPrimitiveTopology>(static_cast<int>(draw->topology));

		if(!indices)
		{
			struct LinearIndex
			{
				unsigned int operator[](unsigned int i) { return i; }
			};

			if(!setBatchIndices(batch, topology, LinearIndex(), start, triangleCount))
			{
				return;
			}
		}
		else
		{
			switch(draw->indexType.load())
			{
			case VK_INDEX_TYPE_UINT16:
				if(!setBatchIndices(batch, topology, static_cast<const uint16_t*>(indices), start, triangleCount))
				{
					return;
				}
				break;
			case VK_INDEX_TYPE_UINT32:
				if(!setBatchIndices(batch, topology, static_cast<const uint32_t*>(indices), start, triangleCount))
				{
					return;
				}
				break;
			break;
			default:
				ASSERT(false);
				return;
			}
		}

		// Repeat the last index to allow for SIMD width overrun.
		batch[triangleCount][0] = batch[triangleCount - 1][2];
		batch[triangleCount][1] = batch[triangleCount - 1][2];
		batch[triangleCount][2] = batch[triangleCount - 1][2];

		task->primitiveStart = start;
		task->vertexCount = triangleCount * 3;
		vertexRoutine(&triangle->v0, (unsigned int*)&batch, task, data);
	}

	int Renderer::setupTriangles(int unit, int count)
	{
		Triangle *triangle = triangleBatch[unit];
		Primitive *primitive = primitiveBatch[unit];

		DrawCall &draw = *drawList[primitiveProgress[unit].drawCall & DRAW_COUNT_BITS];
		SetupProcessor::State &state = draw.setupState;
		const SetupProcessor::RoutinePointer &setupRoutine = draw.setupPointer;

		int ms = state.multiSample;
		const DrawData *data = draw.data;
		int visible = 0;

		for(int i = 0; i < count; i++, triangle++)
		{
			Vertex &v0 = triangle->v0;
			Vertex &v1 = triangle->v1;
			Vertex &v2 = triangle->v2;

			if((v0.clipFlags & v1.clipFlags & v2.clipFlags) == Clipper::CLIP_FINITE)
			{
				Polygon polygon(&v0.position, &v1.position, &v2.position);

				int clipFlagsOr = v0.clipFlags | v1.clipFlags | v2.clipFlags;

				if(clipFlagsOr != Clipper::CLIP_FINITE)
				{
					if(!Clipper::Clip(polygon, clipFlagsOr, draw))
					{
						continue;
					}
				}

				if(setupRoutine(primitive, triangle, &polygon, data))
				{
					primitive += ms;
					visible++;
				}
			}
		}

		return visible;
	}

	int Renderer::setupLines(int unit, int count)
	{
		Triangle *triangle = triangleBatch[unit];
		Primitive *primitive = primitiveBatch[unit];
		int visible = 0;

		DrawCall &draw = *drawList[primitiveProgress[unit].drawCall & DRAW_COUNT_BITS];
		SetupProcessor::State &state = draw.setupState;

		int ms = state.multiSample;

		for(int i = 0; i < count; i++)
		{
			if(setupLine(*primitive, *triangle, draw))
			{
				primitive += ms;
				visible++;
			}

			triangle++;
		}

		return visible;
	}

	int Renderer::setupPoints(int unit, int count)
	{
		Triangle *triangle = triangleBatch[unit];
		Primitive *primitive = primitiveBatch[unit];
		int visible = 0;

		DrawCall &draw = *drawList[primitiveProgress[unit].drawCall & DRAW_COUNT_BITS];
		SetupProcessor::State &state = draw.setupState;

		int ms = state.multiSample;

		for(int i = 0; i < count; i++)
		{
			if(setupPoint(*primitive, *triangle, draw))
			{
				primitive += ms;
				visible++;
			}

			triangle++;
		}

		return visible;
	}

	bool Renderer::setupLine(Primitive &primitive, Triangle &triangle, const DrawCall &draw)
	{
		const SetupProcessor::RoutinePointer &setupRoutine = draw.setupPointer;
		const SetupProcessor::State &state = draw.setupState;
		const DrawData &data = *draw.data;

		float lineWidth = data.lineWidth;

		Vertex &v0 = triangle.v0;
		Vertex &v1 = triangle.v1;

		const float4 &P0 = v0.position;
		const float4 &P1 = v1.position;

		if(P0.w <= 0 && P1.w <= 0)
		{
			return false;
		}

		const float W = data.Wx16[0] * (1.0f / 16.0f);
		const float H = data.Hx16[0] * (1.0f / 16.0f);

		float dx = W * (P1.x / P1.w - P0.x / P0.w);
		float dy = H * (P1.y / P1.w - P0.y / P0.w);

		if(dx == 0 && dy == 0)
		{
			return false;
		}

		if(state.multiSample > 1)   // Rectangle
		{
			float4 P[4];
			int C[4];

			P[0] = P0;
			P[1] = P1;
			P[2] = P1;
			P[3] = P0;

			float scale = lineWidth * 0.5f / sqrt(dx*dx + dy*dy);

			dx *= scale;
			dy *= scale;

			float dx0h = dx * P0.w / H;
			float dy0w = dy * P0.w / W;

			float dx1h = dx * P1.w / H;
			float dy1w = dy * P1.w / W;

			P[0].x += -dy0w;
			P[0].y += +dx0h;
			C[0] = Clipper::ComputeClipFlags(P[0]);

			P[1].x += -dy1w;
			P[1].y += +dx1h;
			C[1] = Clipper::ComputeClipFlags(P[1]);

			P[2].x += +dy1w;
			P[2].y += -dx1h;
			C[2] = Clipper::ComputeClipFlags(P[2]);

			P[3].x += +dy0w;
			P[3].y += -dx0h;
			C[3] = Clipper::ComputeClipFlags(P[3]);

			if((C[0] & C[1] & C[2] & C[3]) == Clipper::CLIP_FINITE)
			{
				Polygon polygon(P, 4);

				int clipFlagsOr = C[0] | C[1] | C[2] | C[3];

				if(clipFlagsOr != Clipper::CLIP_FINITE)
				{
					if(!Clipper::Clip(polygon, clipFlagsOr, draw))
					{
						return false;
					}
				}

				return setupRoutine(&primitive, &triangle, &polygon, &data);
			}
		}
		else   // Diamond test convention
		{
			float4 P[8];
			int C[8];

			P[0] = P0;
			P[1] = P0;
			P[2] = P0;
			P[3] = P0;
			P[4] = P1;
			P[5] = P1;
			P[6] = P1;
			P[7] = P1;

			float dx0 = lineWidth * 0.5f * P0.w / W;
			float dy0 = lineWidth * 0.5f * P0.w / H;

			float dx1 = lineWidth * 0.5f * P1.w / W;
			float dy1 = lineWidth * 0.5f * P1.w / H;

			P[0].x += -dx0;
			C[0] = Clipper::ComputeClipFlags(P[0]);

			P[1].y += +dy0;
			C[1] = Clipper::ComputeClipFlags(P[1]);

			P[2].x += +dx0;
			C[2] = Clipper::ComputeClipFlags(P[2]);

			P[3].y += -dy0;
			C[3] = Clipper::ComputeClipFlags(P[3]);

			P[4].x += -dx1;
			C[4] = Clipper::ComputeClipFlags(P[4]);

			P[5].y += +dy1;
			C[5] = Clipper::ComputeClipFlags(P[5]);

			P[6].x += +dx1;
			C[6] = Clipper::ComputeClipFlags(P[6]);

			P[7].y += -dy1;
			C[7] = Clipper::ComputeClipFlags(P[7]);

			if((C[0] & C[1] & C[2] & C[3] & C[4] & C[5] & C[6] & C[7]) == Clipper::CLIP_FINITE)
			{
				float4 L[6];

				if(dx > -dy)
				{
					if(dx > dy)   // Right
					{
						L[0] = P[0];
						L[1] = P[1];
						L[2] = P[5];
						L[3] = P[6];
						L[4] = P[7];
						L[5] = P[3];
					}
					else   // Down
					{
						L[0] = P[0];
						L[1] = P[4];
						L[2] = P[5];
						L[3] = P[6];
						L[4] = P[2];
						L[5] = P[3];
					}
				}
				else
				{
					if(dx > dy)   // Up
					{
						L[0] = P[0];
						L[1] = P[1];
						L[2] = P[2];
						L[3] = P[6];
						L[4] = P[7];
						L[5] = P[4];
					}
					else   // Left
					{
						L[0] = P[1];
						L[1] = P[2];
						L[2] = P[3];
						L[3] = P[7];
						L[4] = P[4];
						L[5] = P[5];
					}
				}

				Polygon polygon(L, 6);

				int clipFlagsOr = C[0] | C[1] | C[2] | C[3] | C[4] | C[5] | C[6] | C[7];

				if(clipFlagsOr != Clipper::CLIP_FINITE)
				{
					if(!Clipper::Clip(polygon, clipFlagsOr, draw))
					{
						return false;
					}
				}

				return setupRoutine(&primitive, &triangle, &polygon, &data);
			}
		}

		return false;
	}

	bool Renderer::setupPoint(Primitive &primitive, Triangle &triangle, const DrawCall &draw)
	{
		const SetupProcessor::RoutinePointer &setupRoutine = draw.setupPointer;
		const DrawData &data = *draw.data;

		Vertex &v = triangle.v0;

		float pSize = v.pointSize;

		pSize = clamp(pSize, 1.0f, static_cast<float>(vk::MAX_POINT_SIZE));

		float4 P[4];
		int C[4];

		P[0] = v.position;
		P[1] = v.position;
		P[2] = v.position;
		P[3] = v.position;

		const float X = pSize * P[0].w * data.halfPixelX[0];
		const float Y = pSize * P[0].w * data.halfPixelY[0];

		P[0].x -= X;
		P[0].y += Y;
		C[0] = Clipper::ComputeClipFlags(P[0]);

		P[1].x += X;
		P[1].y += Y;
		C[1] = Clipper::ComputeClipFlags(P[1]);

		P[2].x += X;
		P[2].y -= Y;
		C[2] = Clipper::ComputeClipFlags(P[2]);

		P[3].x -= X;
		P[3].y -= Y;
		C[3] = Clipper::ComputeClipFlags(P[3]);

		Polygon polygon(P, 4);

		if((C[0] & C[1] & C[2] & C[3]) == Clipper::CLIP_FINITE)
		{
			int clipFlagsOr = C[0] | C[1] | C[2] | C[3];

			if(clipFlagsOr != Clipper::CLIP_FINITE)
			{
				if(!Clipper::Clip(polygon, clipFlagsOr, draw))
				{
					return false;
				}
			}

			triangle.v1 = triangle.v0;
			triangle.v2 = triangle.v0;

			triangle.v1.projected.x += iround(16 * 0.5f * pSize);
			triangle.v2.projected.y -= iround(16 * 0.5f * pSize) * (data.Hx16[0] > 0.0f ? 1 : -1);   // Both Direct3D and OpenGL expect (0, 0) in the top-left corner
			return setupRoutine(&primitive, &triangle, &polygon, &data);
		}

		return false;
	}

	void Renderer::initializeThreads()
	{
		unitCount = ceilPow2(threadCount);
		clusterCount = ceilPow2(threadCount);

		for(int i = 0; i < unitCount; i++)
		{
			triangleBatch[i] = (Triangle*)allocate(batchSize * sizeof(Triangle));
			primitiveBatch[i] = (Primitive*)allocate(batchSize * sizeof(Primitive));
		}

		for(int i = 0; i < threadCount; i++)
		{
			vertexTask[i] = (VertexTask*)allocate(sizeof(VertexTask));
			vertexTask[i]->vertexCache.drawCall = -1;

			task[i].type = Task::SUSPEND;

			resume[i] = new Event();
			suspend[i] = new Event();

			Parameters parameters;
			parameters.threadIndex = i;
			parameters.renderer = this;

			exitThreads = false;
			worker[i] = new std::thread(threadFunction, &parameters);

			suspend[i]->wait();
			suspend[i]->signal();
		}
	}

	void Renderer::terminateThreads()
	{
		while(threadsAwake != 0)
		{
			std::this_thread::yield();
		}

		for(int thread = 0; thread < threadCount; thread++)
		{
			if(worker[thread])
			{
				exitThreads = true;
				resume[thread]->signal();
				worker[thread]->join();

				delete worker[thread];
				worker[thread] = 0;
				delete resume[thread];
				resume[thread] = 0;
				delete suspend[thread];
				suspend[thread] = 0;
			}

			deallocate(vertexTask[thread]);
			vertexTask[thread] = 0;
		}

		for(int i = 0; i < 16; i++)
		{
			deallocate(triangleBatch[i]);
			triangleBatch[i] = 0;

			deallocate(primitiveBatch[i]);
			primitiveBatch[i] = 0;
		}
	}

	void Renderer::addQuery(vk::Query *query)
	{
		ASSERT(query->getType() == VK_QUERY_TYPE_OCCLUSION);
		ASSERT(!occlusionQuery);

		occlusionQuery = query;
	}

	void Renderer::removeQuery(vk::Query *query)
	{
		ASSERT(query->getType() == VK_QUERY_TYPE_OCCLUSION);
		ASSERT(occlusionQuery == query);

		occlusionQuery = nullptr;
	}

	void Renderer::advanceInstanceAttributes(Stream* inputs)
	{
		for(uint32_t i = 0; i < vk::MAX_VERTEX_INPUT_BINDINGS; i++)
		{
			auto &attrib = inputs[i];
			if (attrib.count && attrib.instanceStride)
			{
				// Under the casts: attrib.buffer += attrib.instanceStride
				attrib.buffer = (void const *)((uintptr_t)attrib.buffer + attrib.instanceStride);
			}
		}
	}

	void Renderer::setViewport(const VkViewport &viewport)
	{
		this->viewport = viewport;
	}

	void Renderer::setScissor(const VkRect2D &scissor)
	{
		this->scissor = scissor;
	}

	void Renderer::updateConfiguration(bool initialUpdate)
	{
		if(initialUpdate)
		{
			terminateThreads();

			VertexProcessor::setRoutineCacheSize(1024);
			PixelProcessor::setRoutineCacheSize(1024);
			SetupProcessor::setRoutineCacheSize(1024);

			threadCount = CPUID::processAffinity();

			CPUID::setEnableSSE4_1(true);
			CPUID::setEnableSSSE3(true);
			CPUID::setEnableSSE3(true);
			CPUID::setEnableSSE2(true);
			CPUID::setEnableSSE(true);
		}

		if(!initialUpdate && !worker[0])
		{
			initializeThreads();
		}
	}
}
