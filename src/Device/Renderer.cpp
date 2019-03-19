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
#include "Device/SwiftConfig.hpp"
#include "Reactor/Reactor.hpp"
#include "Pipeline/Constants.hpp"
#include "System/MutexLock.hpp"
#include "System/CPUID.hpp"
#include "System/Memory.hpp"
#include "System/Resource.hpp"
#include "System/Half.hpp"
#include "System/Math.hpp"
#include "System/Timer.hpp"
#include "Vulkan/VkConfig.h"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Pipeline/SpirvShader.hpp"
#include "Vertex.hpp"

#undef max

bool disableServer = true;

#ifndef NDEBUG
unsigned int minPrimitives = 1;
unsigned int maxPrimitives = 1 << 21;
#endif

namespace sw
{
	extern bool booleanFaceRegister;
	extern bool fullPixelPositionRegister;
	extern bool leadingVertexFirst;         // Flat shading uses first vertex, else last
	extern bool secondaryColor;             // Specular lighting is applied after texturing
	extern bool colorsDefaultToZero;

	extern bool forceWindowed;
	extern bool postBlendSRGB;
	extern bool exactColorRounding;
	extern TransparencyAntialiasing transparencyAntialiasing;
	extern bool forceClearRegisters;

	extern bool precacheVertex;
	extern bool precacheSetup;
	extern bool precachePixel;

	static const int batchSize = 128;
	AtomicInt threadCount(1);
	AtomicInt Renderer::unitCount(1);
	AtomicInt Renderer::clusterCount(1);

	TranscendentalPrecision logPrecision = ACCURATE;
	TranscendentalPrecision expPrecision = ACCURATE;
	TranscendentalPrecision rcpPrecision = ACCURATE;
	TranscendentalPrecision rsqPrecision = ACCURATE;
	bool perspectiveCorrection = true;

	static void setGlobalRenderingSettings(Conventions conventions, bool exactColorRounding)
	{
		static bool initialized = false;

		if(!initialized)
		{
			sw::booleanFaceRegister = conventions.booleanFaceRegister;
			sw::fullPixelPositionRegister = conventions.fullPixelPositionRegister;
			sw::leadingVertexFirst = conventions.leadingVertexFirst;
			sw::secondaryColor = conventions.secondaryColor;
			sw::colorsDefaultToZero = conventions.colorsDefaultToZero;
			sw::exactColorRounding = exactColorRounding;
			initialized = true;
		}
	}

	struct Parameters
	{
		Renderer *renderer;
		int threadIndex;
	};

	DrawCall::DrawCall()
	{
		queries = 0;

		references = -1;

		data = (DrawData*)allocate(sizeof(DrawData));
		data->constants = &constants;
	}

	DrawCall::~DrawCall()
	{
		delete queries;

		deallocate(data);
	}

	Renderer::Renderer(Context *context, Conventions conventions, bool exactColorRounding) : VertexProcessor(context), PixelProcessor(context), SetupProcessor(context), context(context), viewport()
	{
		setGlobalRenderingSettings(conventions, exactColorRounding);

		setRenderTarget(0, nullptr);
		clipper = new Clipper;
		blitter = new Blitter;

		#if PERF_HUD
			resetTimers();
		#endif

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

		clipFlags = 0;

		swiftConfig = new SwiftConfig(disableServer);
		updateConfiguration(true);

		sync = new Resource(0);
	}

	Renderer::~Renderer()
	{
		sync->lock(EXCLUSIVE);
		sync->destruct();
		terminateThreads();
		sync->unlock();

		delete clipper;
		clipper = nullptr;

		delete blitter;
		blitter = nullptr;

		delete resumeApp;
		resumeApp = nullptr;

		for(int draw = 0; draw < DRAW_COUNT; draw++)
		{
			delete drawCall[draw];
			drawCall[draw] = nullptr;
		}

		delete swiftConfig;
		swiftConfig = nullptr;
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

	void Renderer::draw(DrawType drawType, unsigned int count, bool update)
	{
		#ifndef NDEBUG
			if(count < minPrimitives || count > maxPrimitives)
			{
				return;
			}
		#endif

		context->drawType = drawType;

		updateConfiguration();

		int ms = context->sampleCount;
		unsigned int oldMultiSampleMask = context->multiSampleMask;
		context->multiSampleMask = context->sampleMask & ((unsigned)0xFFFFFFFF >> (32 - ms));

		if(!context->multiSampleMask)
		{
			return;
		}

		sync->lock(sw::PRIVATE);

		if(update || oldMultiSampleMask != context->multiSampleMask)
		{
			vertexState = VertexProcessor::update(drawType);
			setupState = SetupProcessor::update();
			pixelState = PixelProcessor::update();

			vertexRoutine = VertexProcessor::routine(vertexState);
			setupRoutine = SetupProcessor::routine(setupState);
			pixelRoutine = PixelProcessor::routine(pixelState);
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

		if(queries.size() != 0)
		{
			draw->queries = new std::list<Query*>();
			for(auto &query : queries)
			{
				++query->reference; // Atomic
				draw->queries->push_back(query);
			}
		}

		draw->drawType = drawType;
		draw->batchSize = batch;

		vertexRoutine->bind();
		setupRoutine->bind();
		pixelRoutine->bind();

		draw->vertexRoutine = vertexRoutine;
		draw->setupRoutine = setupRoutine;
		draw->pixelRoutine = pixelRoutine;
		draw->vertexPointer = (VertexProcessor::RoutinePointer)vertexRoutine->getEntry();
		draw->setupPointer = (SetupProcessor::RoutinePointer)setupRoutine->getEntry();
		draw->pixelPointer = (PixelProcessor::RoutinePointer)pixelRoutine->getEntry();
		draw->setupPrimitives = setupPrimitives;
		draw->setupState = setupState;

		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			data->input[i] = context->input[i].buffer;
			data->stride[i] = context->input[i].stride;
		}

		if(context->indexBuffer)
		{
			data->indices = context->indexBuffer;
		}

		if(context->vertexShader->hasBuiltinInput(spv::BuiltInInstanceId))
		{
			data->instanceID = context->instanceID;
		}

		if(pixelState.stencilActive)
		{
			data->stencil[0].set(context->frontStencil.reference, context->frontStencil.compareMask, context->frontStencil.writeMask);
			data->stencil[1].set(context->backStencil.reference, context->backStencil.compareMask, context->backStencil.writeMask);
		}

		data->lineWidth = context->lineWidth;

		data->factor = factor;

		if(pixelState.transparencyAntialiasing == TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			float ref = context->alphaReference * (1.0f / 255.0f);
			float margin = sw::min(ref, 1.0f - ref);

			if(ms == 4)
			{
				data->a2c0 = replicate(ref - margin * 0.6f);
				data->a2c1 = replicate(ref - margin * 0.2f);
				data->a2c2 = replicate(ref + margin * 0.2f);
				data->a2c3 = replicate(ref + margin * 0.6f);
			}
			else if(ms == 2)
			{
				data->a2c0 = replicate(ref - margin * 0.3f);
				data->a2c1 = replicate(ref + margin * 0.3f);
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

		#if PERF_PROFILE
			for(int cluster = 0; cluster < clusterCount; cluster++)
			{
				for(int i = 0; i < PERF_TIMERS; i++)
				{
					data->cycles[i][cluster] = 0;
				}
			}
		#endif

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
					VkOffset3D offset = { 0, 0, static_cast<int32_t>(context->renderTargetLayer[index]) };
					data->colorBuffer[index] = (unsigned int*)context->renderTarget[index]->getOffsetPointer(offset, VK_IMAGE_ASPECT_COLOR_BIT);
					data->colorPitchB[index] = context->renderTarget[index]->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT);
					data->colorSliceB[index] = context->renderTarget[index]->slicePitchBytes(VK_IMAGE_ASPECT_COLOR_BIT);
				}
			}

			draw->depthBuffer = context->depthBuffer;
			draw->stencilBuffer = context->stencilBuffer;

			if(draw->depthBuffer)
			{
				VkOffset3D offset = { 0, 0, static_cast<int32_t>(context->depthBufferLayer) };
				data->depthBuffer = (float*)context->depthBuffer->getOffsetPointer(offset, VK_IMAGE_ASPECT_DEPTH_BIT);
				data->depthPitchB = context->depthBuffer->rowPitchBytes(VK_IMAGE_ASPECT_DEPTH_BIT);
				data->depthSliceB = context->depthBuffer->slicePitchBytes(VK_IMAGE_ASPECT_DEPTH_BIT);
			}

			if(draw->stencilBuffer)
			{
				VkOffset3D offset = { 0, 0, static_cast<int32_t>(context->stencilBufferLayer) };
				data->stencilBuffer = (unsigned char*)context->stencilBuffer->getOffsetPointer(offset, VK_IMAGE_ASPECT_STENCIL_BIT);
				data->stencilPitchB = context->stencilBuffer->rowPitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT);
				data->stencilSliceB = context->stencilBuffer->slicePitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT);
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

		if(logPrecision < IEEE)
		{
			CPUID::setFlushToZero(true);
			CPUID::setDenormalsAreZero(true);
		}

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
								qSize++;

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

				primitiveProgress[unit].drawCall = currentDraw;
				primitiveProgress[unit].firstPrimitive = primitive;
				primitiveProgress[unit].primitiveCount = count - primitive >= batch ? batch : count - primitive;

				draw->primitive += batch;

				Task &task = taskQueue[qHead];
				task.type = Task::PRIMITIVES;
				task.primitiveUnit = unit;

				primitiveProgress[unit].references = -1;

				// Commit to the task queue
				qHead = (qHead + 1) & TASK_COUNT_BITS;
				qSize++;
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
			qSize--;

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
		#if PERF_HUD
			int64_t startTick = Timer::ticks();
		#endif

		switch(task[threadIndex].type)
		{
		case Task::PRIMITIVES:
			{
				int unit = task[threadIndex].primitiveUnit;

				int input = primitiveProgress[unit].firstPrimitive;
				int count = primitiveProgress[unit].primitiveCount;
				DrawCall *draw = drawList[primitiveProgress[unit].drawCall & DRAW_COUNT_BITS];
				int (Renderer::*setupPrimitives)(int batch, int count) = draw->setupPrimitives;

				processPrimitiveVertices(unit, input, count, draw->count, threadIndex);

				#if PERF_HUD
					int64_t time = Timer::ticks();
					vertexTime[threadIndex] += time - startTick;
					startTick = time;
				#endif

				int visible = 0;

				if(!draw->setupState.rasterizerDiscard)
				{
					visible = (this->*setupPrimitives)(unit, count);
				}

				primitiveProgress[unit].visible = visible;
				primitiveProgress[unit].references = clusterCount;

				#if PERF_HUD
					setupTime[threadIndex] += Timer::ticks() - startTick;
				#endif
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

				#if PERF_HUD
					pixelTime[threadIndex] += Timer::ticks() - startTick;
				#endif
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
		sync->lock(sw::PUBLIC);
		sync->unlock();
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

		int ref = primitiveProgress[unit].references--; // Atomic

		if(ref == 0)
		{
			ref = draw.references--; // Atomic

			if(ref == 0)
			{
				#if PERF_PROFILE
					for(int cluster = 0; cluster < clusterCount; cluster++)
					{
						for(int i = 0; i < PERF_TIMERS; i++)
						{
							profiler.cycles[i] += data.cycles[i][cluster];
						}
					}
				#endif

				if(draw.queries)
				{
					for(auto &query : *(draw.queries))
					{
						switch(query->type)
						{
						case Query::FRAGMENTS_PASSED:
							for(int cluster = 0; cluster < clusterCount; cluster++)
							{
								query->data += data.occlusion[cluster];
							}
							break;
						case Query::TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
							query->data += processedPrimitives;
							break;
						default:
							break;
						}

						--query->reference; // Atomic
					}

					delete draw.queries;
					draw.queries = 0;
				}

				draw.vertexRoutine->unbind();
				draw.setupRoutine->unbind();
				draw.pixelRoutine->unbind();

				sync->unlock();

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

		unsigned int batch[128][3];   // FIXME: Adjust to dynamic batch size

		switch(draw->drawType)
		{
		case DRAW_POINTLIST:
			{
				unsigned int index = start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index;
					batch[i][1] = index;
					batch[i][2] = index;

					index += 1;
				}
			}
			break;
		case DRAW_LINELIST:
			{
				unsigned int index = 2 * start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index + 0;
					batch[i][1] = index + 1;
					batch[i][2] = index + 1;

					index += 2;
				}
			}
			break;
		case DRAW_LINESTRIP:
			{
				unsigned int index = start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index + 0;
					batch[i][1] = index + 1;
					batch[i][2] = index + 1;

					index += 1;
				}
			}
			break;
		case DRAW_TRIANGLELIST:
			{
				unsigned int index = 3 * start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index + 0;
					batch[i][1] = index + 1;
					batch[i][2] = index + 2;

					index += 3;
				}
			}
			break;
		case DRAW_TRIANGLESTRIP:
			{
				unsigned int index = start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					if(leadingVertexFirst)
					{
						batch[i][0] = index + 0;
						batch[i][1] = index + (index & 1) + 1;
						batch[i][2] = index + (~index & 1) + 1;
					}
					else
					{
						batch[i][0] = index + (index & 1);
						batch[i][1] = index + (~index & 1);
						batch[i][2] = index + 2;
					}

					index += 1;
				}
			}
			break;
		case DRAW_TRIANGLEFAN:
			{
				unsigned int index = start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					if(leadingVertexFirst)
					{
						batch[i][0] = index + 1;
						batch[i][1] = index + 2;
						batch[i][2] = 0;
					}
					else
					{
						batch[i][0] = 0;
						batch[i][1] = index + 1;
						batch[i][2] = index + 2;
					}

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDPOINTLIST16:
			{
				const unsigned short *index = (const unsigned short*)indices + start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = *index;
					batch[i][1] = *index;
					batch[i][2] = *index;

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDPOINTLIST32:
			{
				const unsigned int *index = (const unsigned int*)indices + start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = *index;
					batch[i][1] = *index;
					batch[i][2] = *index;

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDLINELIST16:
			{
				const unsigned short *index = (const unsigned short*)indices + 2 * start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[1];
					batch[i][2] = index[1];

					index += 2;
				}
			}
			break;
		case DRAW_INDEXEDLINELIST32:
			{
				const unsigned int *index = (const unsigned int*)indices + 2 * start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[1];
					batch[i][2] = index[1];

					index += 2;
				}
			}
			break;
		case DRAW_INDEXEDLINESTRIP16:
			{
				const unsigned short *index = (const unsigned short*)indices + start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[1];
					batch[i][2] = index[1];

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDLINESTRIP32:
			{
				const unsigned int *index = (const unsigned int*)indices + start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[1];
					batch[i][2] = index[1];

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDTRIANGLELIST16:
			{
				const unsigned short *index = (const unsigned short*)indices + 3 * start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[1];
					batch[i][2] = index[2];

					index += 3;
				}
			}
			break;
		case DRAW_INDEXEDTRIANGLELIST32:
			{
				const unsigned int *index = (const unsigned int*)indices + 3 * start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[1];
					batch[i][2] = index[2];

					index += 3;
				}
			}
			break;
		case DRAW_INDEXEDTRIANGLESTRIP16:
			{
				const unsigned short *index = (const unsigned short*)indices + start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[((start + i) & 1) + 1];
					batch[i][2] = index[(~(start + i) & 1) + 1];

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDTRIANGLESTRIP32:
			{
				const unsigned int *index = (const unsigned int*)indices + start;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[0];
					batch[i][1] = index[((start + i) & 1) + 1];
					batch[i][2] = index[(~(start + i) & 1) + 1];

					index += 1;
				}
			}
			break;
		case DRAW_INDEXEDTRIANGLEFAN16:
			{
				const unsigned short *index = (const unsigned short*)indices;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[start + i + 1];
					batch[i][1] = index[start + i + 2];
					batch[i][2] = index[0];
				}
			}
			break;
		case DRAW_INDEXEDTRIANGLEFAN32:
			{
				const unsigned int *index = (const unsigned int*)indices;

				for(unsigned int i = 0; i < triangleCount; i++)
				{
					batch[i][0] = index[start + i + 1];
					batch[i][1] = index[start + i + 2];
					batch[i][2] = index[0];
				}
			}
			break;
		default:
			ASSERT(false);
			return;
		}

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
				Polygon polygon(&v0.builtins.position, &v1.builtins.position, &v2.builtins.position);

				int clipFlagsOr = v0.clipFlags | v1.clipFlags | v2.clipFlags;

				if(clipFlagsOr != Clipper::CLIP_FINITE)
				{
					if(!clipper->clip(polygon, clipFlagsOr, draw))
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

		const float4 &P0 = v0.builtins.position;
		const float4 &P1 = v1.builtins.position;

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
			C[0] = clipper->computeClipFlags(P[0]);

			P[1].x += -dy1w;
			P[1].y += +dx1h;
			C[1] = clipper->computeClipFlags(P[1]);

			P[2].x += +dy1w;
			P[2].y += -dx1h;
			C[2] = clipper->computeClipFlags(P[2]);

			P[3].x += +dy0w;
			P[3].y += -dx0h;
			C[3] = clipper->computeClipFlags(P[3]);

			if((C[0] & C[1] & C[2] & C[3]) == Clipper::CLIP_FINITE)
			{
				Polygon polygon(P, 4);

				int clipFlagsOr = C[0] | C[1] | C[2] | C[3];

				if(clipFlagsOr != Clipper::CLIP_FINITE)
				{
					if(!clipper->clip(polygon, clipFlagsOr, draw))
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
			C[0] = clipper->computeClipFlags(P[0]);

			P[1].y += +dy0;
			C[1] = clipper->computeClipFlags(P[1]);

			P[2].x += +dx0;
			C[2] = clipper->computeClipFlags(P[2]);

			P[3].y += -dy0;
			C[3] = clipper->computeClipFlags(P[3]);

			P[4].x += -dx1;
			C[4] = clipper->computeClipFlags(P[4]);

			P[5].y += +dy1;
			C[5] = clipper->computeClipFlags(P[5]);

			P[6].x += +dx1;
			C[6] = clipper->computeClipFlags(P[6]);

			P[7].y += -dy1;
			C[7] = clipper->computeClipFlags(P[7]);

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
					if(!clipper->clip(polygon, clipFlagsOr, draw))
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

		float pSize = v.builtins.pointSize;

		pSize = clamp(pSize, 1.0f, static_cast<float>(vk::MAX_POINT_SIZE));

		float4 P[4];
		int C[4];

		P[0] = v.builtins.position;
		P[1] = v.builtins.position;
		P[2] = v.builtins.position;
		P[3] = v.builtins.position;

		const float X = pSize * P[0].w * data.halfPixelX[0];
		const float Y = pSize * P[0].w * data.halfPixelY[0];

		P[0].x -= X;
		P[0].y += Y;
		C[0] = clipper->computeClipFlags(P[0]);

		P[1].x += X;
		P[1].y += Y;
		C[1] = clipper->computeClipFlags(P[1]);

		P[2].x += X;
		P[2].y -= Y;
		C[2] = clipper->computeClipFlags(P[2]);

		P[3].x -= X;
		P[3].y -= Y;
		C[3] = clipper->computeClipFlags(P[3]);

		triangle.v1 = triangle.v0;
		triangle.v2 = triangle.v0;

		triangle.v1.projected.x += iround(16 * 0.5f * pSize);
		triangle.v2.projected.y -= iround(16 * 0.5f * pSize) * (data.Hx16[0] > 0.0f ? 1 : -1);   // Both Direct3D and OpenGL expect (0, 0) in the top-left corner

		Polygon polygon(P, 4);

		if((C[0] & C[1] & C[2] & C[3]) == Clipper::CLIP_FINITE)
		{
			int clipFlagsOr = C[0] | C[1] | C[2] | C[3];

			if(clipFlagsOr != Clipper::CLIP_FINITE)
			{
				if(!clipper->clip(polygon, clipFlagsOr, draw))
				{
					return false;
				}
			}

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
			worker[i] = new Thread(threadFunction, &parameters);

			suspend[i]->wait();
			suspend[i]->signal();
		}
	}

	void Renderer::terminateThreads()
	{
		while(threadsAwake != 0)
		{
			Thread::sleep(1);
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

	void Renderer::setMultiSampleMask(unsigned int mask)
	{
		context->sampleMask = mask;
	}

	void Renderer::setTransparencyAntialiasing(TransparencyAntialiasing transparencyAntialiasing)
	{
		sw::transparencyAntialiasing = transparencyAntialiasing;
	}

	void Renderer::setLineWidth(float width)
	{
		context->lineWidth = width;
	}

	void Renderer::setDepthBias(float bias)
	{
		context->depthBias = bias;
	}

	void Renderer::setSlopeDepthBias(float slopeBias)
	{
		context->slopeDepthBias = slopeBias;
	}

	void Renderer::setRasterizerDiscard(bool rasterizerDiscard)
	{
		context->rasterizerDiscard = rasterizerDiscard;
	}

	void Renderer::setPixelShader(const SpirvShader *shader)
	{
		context->pixelShader = shader;
	}

	void Renderer::setVertexShader(const SpirvShader *shader)
	{
		context->vertexShader = shader;
	}

	void Renderer::addQuery(Query *query)
	{
		queries.push_back(query);
	}

	void Renderer::removeQuery(Query *query)
	{
		queries.remove(query);
	}

	#if PERF_HUD
		int Renderer::getThreadCount()
		{
			return threadCount;
		}

		int64_t Renderer::getVertexTime(int thread)
		{
			return vertexTime[thread];
		}

		int64_t Renderer::getSetupTime(int thread)
		{
			return setupTime[thread];
		}

		int64_t Renderer::getPixelTime(int thread)
		{
			return pixelTime[thread];
		}

		void Renderer::resetTimers()
		{
			for(int thread = 0; thread < threadCount; thread++)
			{
				vertexTime[thread] = 0;
				setupTime[thread] = 0;
				pixelTime[thread] = 0;
			}
		}
	#endif

	void Renderer::setContext(const sw::Context& context)
	{
		*(this->context) = context;
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
		bool newConfiguration = swiftConfig->hasNewConfiguration();

		if(newConfiguration || initialUpdate)
		{
			terminateThreads();

			SwiftConfig::Configuration configuration = {};
			swiftConfig->getConfiguration(configuration);

			precacheVertex = !newConfiguration && configuration.precache;
			precacheSetup = !newConfiguration && configuration.precache;
			precachePixel = !newConfiguration && configuration.precache;

			VertexProcessor::setRoutineCacheSize(configuration.vertexRoutineCacheSize);
			PixelProcessor::setRoutineCacheSize(configuration.pixelRoutineCacheSize);
			SetupProcessor::setRoutineCacheSize(configuration.setupRoutineCacheSize);

			switch(configuration.textureSampleQuality)
			{
			case 0:  Sampler::setFilterQuality(FILTER_POINT);       break;
			case 1:  Sampler::setFilterQuality(FILTER_LINEAR);      break;
			case 2:  Sampler::setFilterQuality(FILTER_ANISOTROPIC); break;
			default: Sampler::setFilterQuality(FILTER_ANISOTROPIC); break;
			}

			switch(configuration.mipmapQuality)
			{
			case 0:  Sampler::setMipmapQuality(MIPMAP_POINT);  break;
			case 1:  Sampler::setMipmapQuality(MIPMAP_LINEAR); break;
			default: Sampler::setMipmapQuality(MIPMAP_LINEAR); break;
			}

			setPerspectiveCorrection(configuration.perspectiveCorrection);

			switch(configuration.transcendentalPrecision)
			{
			case 0:
				logPrecision = APPROXIMATE;
				expPrecision = APPROXIMATE;
				rcpPrecision = APPROXIMATE;
				rsqPrecision = APPROXIMATE;
				break;
			case 1:
				logPrecision = PARTIAL;
				expPrecision = PARTIAL;
				rcpPrecision = PARTIAL;
				rsqPrecision = PARTIAL;
				break;
			case 2:
				logPrecision = ACCURATE;
				expPrecision = ACCURATE;
				rcpPrecision = ACCURATE;
				rsqPrecision = ACCURATE;
				break;
			case 3:
				logPrecision = WHQL;
				expPrecision = WHQL;
				rcpPrecision = WHQL;
				rsqPrecision = WHQL;
				break;
			case 4:
				logPrecision = IEEE;
				expPrecision = IEEE;
				rcpPrecision = IEEE;
				rsqPrecision = IEEE;
				break;
			default:
				logPrecision = ACCURATE;
				expPrecision = ACCURATE;
				rcpPrecision = ACCURATE;
				rsqPrecision = ACCURATE;
				break;
			}

			switch(configuration.transparencyAntialiasing)
			{
			case 0:  transparencyAntialiasing = TRANSPARENCY_NONE;              break;
			case 1:  transparencyAntialiasing = TRANSPARENCY_ALPHA_TO_COVERAGE; break;
			default: transparencyAntialiasing = TRANSPARENCY_NONE;              break;
			}

			switch(configuration.threadCount)
			{
			case -1: threadCount = CPUID::coreCount();        break;
			case 0:  threadCount = CPUID::processAffinity();  break;
			default: threadCount = configuration.threadCount; break;
			}

			CPUID::setEnableSSE4_1(configuration.enableSSE4_1);
			CPUID::setEnableSSSE3(configuration.enableSSSE3);
			CPUID::setEnableSSE3(configuration.enableSSE3);
			CPUID::setEnableSSE2(configuration.enableSSE2);
			CPUID::setEnableSSE(configuration.enableSSE);

			for(int pass = 0; pass < 10; pass++)
			{
				optimization[pass] = configuration.optimization[pass];
			}

			forceWindowed = configuration.forceWindowed;
			postBlendSRGB = configuration.postBlendSRGB;
			exactColorRounding = configuration.exactColorRounding;
			forceClearRegisters = configuration.forceClearRegisters;

		#ifndef NDEBUG
			minPrimitives = configuration.minPrimitives;
			maxPrimitives = configuration.maxPrimitives;
		#endif
		}

		if(!initialUpdate && !worker[0])
		{
			initializeThreads();
		}
	}
}
