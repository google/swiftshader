// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#include "VkQueue.hpp"
#include "VkCommandBuffer.hpp"
#include "VkFence.hpp"
#include "VkSemaphore.hpp"
#include "Device/Renderer.hpp"
#include "WSI/VkSwapchainKHR.hpp"

#include "marl/defer.h"
#include "marl/scheduler.h"
#include "marl/thread.h"
#include "marl/trace.h"

#include <cstring>

namespace {

VkSubmitInfo *DeepCopySubmitInfo(uint32_t submitCount, const VkSubmitInfo *pSubmits)
{
	size_t submitSize = sizeof(VkSubmitInfo) * submitCount;
	size_t totalSize = submitSize;
	for(uint32_t i = 0; i < submitCount; i++)
	{
		totalSize += pSubmits[i].waitSemaphoreCount * sizeof(VkSemaphore);
		totalSize += pSubmits[i].waitSemaphoreCount * sizeof(VkPipelineStageFlags);
		totalSize += pSubmits[i].signalSemaphoreCount * sizeof(VkSemaphore);
		totalSize += pSubmits[i].commandBufferCount * sizeof(VkCommandBuffer);
	}

	uint8_t *mem = static_cast<uint8_t *>(
	    vk::allocate(totalSize, vk::REQUIRED_MEMORY_ALIGNMENT, vk::DEVICE_MEMORY, vk::Fence::GetAllocationScope()));

	auto submits = new(mem) VkSubmitInfo[submitCount];
	memcpy(mem, pSubmits, submitSize);
	mem += submitSize;

	for(uint32_t i = 0; i < submitCount; i++)
	{
		size_t size = pSubmits[i].waitSemaphoreCount * sizeof(VkSemaphore);
		submits[i].pWaitSemaphores = reinterpret_cast<const VkSemaphore *>(mem);
		memcpy(mem, pSubmits[i].pWaitSemaphores, size);
		mem += size;

		size = pSubmits[i].waitSemaphoreCount * sizeof(VkPipelineStageFlags);
		submits[i].pWaitDstStageMask = reinterpret_cast<const VkPipelineStageFlags *>(mem);
		memcpy(mem, pSubmits[i].pWaitDstStageMask, size);
		mem += size;

		size = pSubmits[i].signalSemaphoreCount * sizeof(VkSemaphore);
		submits[i].pSignalSemaphores = reinterpret_cast<const VkSemaphore *>(mem);
		memcpy(mem, pSubmits[i].pSignalSemaphores, size);
		mem += size;

		size = pSubmits[i].commandBufferCount * sizeof(VkCommandBuffer);
		submits[i].pCommandBuffers = reinterpret_cast<const VkCommandBuffer *>(mem);
		memcpy(mem, pSubmits[i].pCommandBuffers, size);
		mem += size;
	}

	return submits;
}

}  // anonymous namespace

namespace vk {

Queue::Queue(Device *device, marl::Scheduler *scheduler)
    : device(device)
{
	queueThread = std::thread(&Queue::taskLoop, this, scheduler);
}

Queue::~Queue()
{
	Task task;
	task.type = Task::KILL_THREAD;
	pending.put(task);

	queueThread.join();
	ASSERT_MSG(pending.count() == 0, "queue has work after worker thread shutdown");

	garbageCollect();
}

VkResult Queue::submit(uint32_t submitCount, const VkSubmitInfo *pSubmits, Fence *fence)
{
	garbageCollect();

	Task task;
	task.submitCount = submitCount;
	task.pSubmits = DeepCopySubmitInfo(submitCount, pSubmits);
	task.events = fence;

	if(task.events)
	{
		task.events->start();
	}

	pending.put(task);

	return VK_SUCCESS;
}

void Queue::submitQueue(const Task &task)
{
	if(renderer == nullptr)
	{
		renderer.reset(new sw::Renderer(device));
	}

	for(uint32_t i = 0; i < task.submitCount; i++)
	{
		auto &submitInfo = task.pSubmits[i];
		for(uint32_t j = 0; j < submitInfo.waitSemaphoreCount; j++)
		{
			vk::Cast(submitInfo.pWaitSemaphores[j])->wait(submitInfo.pWaitDstStageMask[j]);
		}

		{
			CommandBuffer::ExecutionState executionState;
			executionState.renderer = renderer.get();
			executionState.events = task.events;
			for(uint32_t j = 0; j < submitInfo.commandBufferCount; j++)
			{
				vk::Cast(submitInfo.pCommandBuffers[j])->submit(executionState);
			}
		}

		for(uint32_t j = 0; j < submitInfo.signalSemaphoreCount; j++)
		{
			vk::Cast(submitInfo.pSignalSemaphores[j])->signal();
		}
	}

	if(task.pSubmits)
	{
		toDelete.put(task.pSubmits);
	}

	if(task.events)
	{
		// TODO: fix renderer signaling so that work submitted separately from (but before) a fence
		// is guaranteed complete by the time the fence signals.
		renderer->synchronize();
		task.events->finish();
	}
}

void Queue::taskLoop(marl::Scheduler *scheduler)
{
	marl::Thread::setName("Queue<%p>", this);
	scheduler->bind();
	defer(scheduler->unbind());

	while(true)
	{
		Task task = pending.take();

		switch(task.type)
		{
			case Task::KILL_THREAD:
				ASSERT_MSG(pending.count() == 0, "queue has remaining work!");
				return;
			case Task::SUBMIT_QUEUE:
				submitQueue(task);
				break;
			default:
				UNREACHABLE("task.type %d", static_cast<int>(task.type));
				break;
		}
	}
}

VkResult Queue::waitIdle()
{
	// Wait for task queue to flush.
	sw::WaitGroup wg;
	wg.add();

	Task task;
	task.events = &wg;
	pending.put(task);

	wg.wait();

	garbageCollect();

	return VK_SUCCESS;
}

void Queue::garbageCollect()
{
	while(true)
	{
		auto v = toDelete.tryTake();
		if(!v.second) { break; }
		vk::deallocate(v.first, DEVICE_MEMORY);
	}
}

#ifndef __ANDROID__
VkResult Queue::present(const VkPresentInfoKHR *presentInfo)
{
	// This is a hack to deal with screen tearing for now.
	// Need to correctly implement threading using VkSemaphore
	// to get rid of it. b/132458423
	waitIdle();

	for(uint32_t i = 0; i < presentInfo->waitSemaphoreCount; i++)
	{
		vk::Cast(presentInfo->pWaitSemaphores[i])->wait();
	}

	VkResult commandResult = VK_SUCCESS;

	for(uint32_t i = 0; i < presentInfo->swapchainCount; i++)
	{
		VkResult perSwapchainResult = vk::Cast(presentInfo->pSwapchains[i])->present(presentInfo->pImageIndices[i]);

		if(presentInfo->pResults)
		{
			presentInfo->pResults[i] = perSwapchainResult;
		}

		// Keep track of the worst result code. VK_SUBOPTIMAL_KHR is a success code so it should
		// not override failure codes, but should not get replaced by a VK_SUCCESS result itself.
		if(perSwapchainResult != VK_SUCCESS)
		{
			if(commandResult == VK_SUCCESS || commandResult == VK_SUBOPTIMAL_KHR)
			{
				commandResult = perSwapchainResult;
			}
		}
	}

	return commandResult;
}
#endif

}  // namespace vk
