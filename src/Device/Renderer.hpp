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

#include "Blitter.hpp"
#include "PixelProcessor.hpp"
#include "Plane.hpp"
#include "Primitive.hpp"
#include "SetupProcessor.hpp"
#include "VertexProcessor.hpp"
#include "Device/Config.hpp"
#include "Vulkan/VkDescriptorSet.hpp"

#include "marl/finally.h"
#include "marl/pool.h"
#include "marl/ticket.h"

#include <atomic>
#include <list>
#include <mutex>
#include <thread>

namespace vk {

class DescriptorSet;
class Device;
class Query;

}  // namespace vk

namespace sw {

struct DrawCall;
class PixelShader;
class VertexShader;
struct Task;
class TaskEvents;
class Resource;
struct Constants;

static constexpr int MaxBatchSize = 128;
static constexpr int MaxBatchCount = 16;
static constexpr int MaxClusterCount = 16;
static constexpr int MaxDrawCount = 16;

using TriangleBatch = std::array<Triangle, MaxBatchSize>;
using PrimitiveBatch = std::array<Primitive, MaxBatchSize>;

struct DrawData
{
	const Constants *constants;

	vk::DescriptorSet::Bindings descriptorSets = {};
	vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};

	const void *input[MAX_INTERFACE_COMPONENTS / 4];
	unsigned int robustnessSize[MAX_INTERFACE_COMPONENTS / 4];
	unsigned int stride[MAX_INTERFACE_COMPONENTS / 4];
	const void *indices;

	int instanceID;
	int baseVertex;
	float lineWidth;
	int viewID;

	PixelProcessor::Stencil stencil[2];  // clockwise, counterclockwise
	PixelProcessor::Factor factor;
	unsigned int occlusion[MaxClusterCount];  // Number of pixels passing depth test

	float4 WxF;
	float4 HxF;
	float4 X0xF;
	float4 Y0xF;
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

struct DrawCall
{
	struct BatchData
	{
		using Pool = marl::BoundedPool<BatchData, MaxBatchCount, marl::PoolPolicy::Preserve>;

		TriangleBatch triangles;
		PrimitiveBatch primitives;
		VertexTask vertexTask;
		unsigned int id;
		unsigned int firstPrimitive;
		unsigned int numPrimitives;
		int numVisible;
		marl::Ticket clusterTickets[MaxClusterCount];
	};

	using Pool = marl::BoundedPool<DrawCall, MaxDrawCount, marl::PoolPolicy::Preserve>;
	using SetupFunction = int (*)(Triangle *triangles, Primitive *primitives, const DrawCall *drawCall, int count);

	DrawCall();
	~DrawCall();

	static void run(const marl::Loan<DrawCall> &draw, marl::Ticket::Queue *tickets, marl::Ticket::Queue clusterQueues[MaxClusterCount]);
	static void processVertices(DrawCall *draw, BatchData *batch);
	static void processPrimitives(DrawCall *draw, BatchData *batch);
	static void processPixels(const marl::Loan<DrawCall> &draw, const marl::Loan<BatchData> &batch, const std::shared_ptr<marl::Finally> &finally);
	void setup();
	void teardown();

	int id;

	BatchData::Pool *batchDataPool;
	unsigned int numPrimitives;
	unsigned int numPrimitivesPerBatch;
	unsigned int numBatches;

	VkPrimitiveTopology topology;
	VkProvokingVertexModeEXT provokingVertexMode;
	VkIndexType indexType;
	VkLineRasterizationModeEXT lineRasterizationMode;

	VertexProcessor::RoutineType vertexRoutine;
	SetupProcessor::RoutineType setupRoutine;
	PixelProcessor::RoutineType pixelRoutine;

	SetupFunction setupPrimitives;
	SetupProcessor::State setupState;

	vk::ImageView *renderTarget[RENDERTARGETS];
	vk::ImageView *depthBuffer;
	vk::ImageView *stencilBuffer;
	TaskEvents *events;

	vk::Query *occlusionQuery;

	DrawData *data;

	static void processPrimitiveVertices(
	    unsigned int triangleIndicesOut[MaxBatchSize + 1][3],
	    const void *primitiveIndices,
	    VkIndexType indexType,
	    unsigned int start,
	    unsigned int triangleCount,
	    VkPrimitiveTopology topology,
	    VkProvokingVertexModeEXT provokingVertexMode);

	static int setupSolidTriangles(Triangle *triangles, Primitive *primitives, const DrawCall *drawCall, int count);
	static int setupWireframeTriangles(Triangle *triangles, Primitive *primitives, const DrawCall *drawCall, int count);
	static int setupPointTriangles(Triangle *triangles, Primitive *primitives, const DrawCall *drawCall, int count);
	static int setupLines(Triangle *triangles, Primitive *primitives, const DrawCall *drawCall, int count);
	static int setupPoints(Triangle *triangles, Primitive *primitives, const DrawCall *drawCall, int count);

	static bool setupLine(Primitive &primitive, Triangle &triangle, const DrawCall &draw);
	static bool setupPoint(Primitive &primitive, Triangle &triangle, const DrawCall &draw);
};

class alignas(16) Renderer : public VertexProcessor, public PixelProcessor, public SetupProcessor
{
public:
	Renderer(vk::Device *device);

	virtual ~Renderer();

	void *operator new(size_t size);
	void operator delete(void *mem);

	bool hasOcclusionQuery() const { return occlusionQuery != nullptr; }

	void draw(const sw::Context *context, VkIndexType indexType, unsigned int count, int baseVertex,
	          TaskEvents *events, int instanceID, int viewID, void *indexBuffer, const VkExtent3D &framebufferExtent,
	          PushConstantStorage const &pushConstants, bool update = true);

	// Viewport & Clipper
	void setViewport(const VkViewport &viewport);
	void setScissor(const VkRect2D &scissor);

	void addQuery(vk::Query *query);
	void removeQuery(vk::Query *query);

	void advanceInstanceAttributes(Stream *inputs);

	void synchronize();

private:
	VkViewport viewport;
	VkRect2D scissor;

	DrawCall::Pool drawCallPool;
	DrawCall::BatchData::Pool batchDataPool;

	std::atomic<int> nextDrawID = { 0 };

	vk::Query *occlusionQuery = nullptr;
	marl::Ticket::Queue drawTickets;
	marl::Ticket::Queue clusterQueues[MaxClusterCount];

	VertexProcessor::State vertexState;
	SetupProcessor::State setupState;
	PixelProcessor::State pixelState;

	VertexProcessor::RoutineType vertexRoutine;
	SetupProcessor::RoutineType setupRoutine;
	PixelProcessor::RoutineType pixelRoutine;

	vk::Device *device;
};

}  // namespace sw

#endif  // sw_Renderer_hpp
