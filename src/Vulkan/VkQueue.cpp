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
#include "VkStringify.hpp"
#include "VkTimelineSemaphore.hpp"
#include "Device/Renderer.hpp"
#include "WSI/VkSwapchainKHR.hpp"

#include "marl/defer.h"
#include "marl/scheduler.h"
#include "marl/thread.h"
#include "marl/trace.h"

#include <cstring>

namespace vk {

Queue::SubmitInfo *Queue::DeepCopySubmitInfo(uint32_t submitCount, const VkSubmitInfo *pSubmits)
{
	size_t submitSize = sizeof(SubmitInfo) * submitCount;
	size_t totalSize = submitSize;
	for(uint32_t i = 0; i < submitCount; i++)
	{
		totalSize += pSubmits[i].waitSemaphoreCount * sizeof(VkSemaphore);
		totalSize += pSubmits[i].waitSemaphoreCount * sizeof(VkPipelineStageFlags);
		totalSize += pSubmits[i].signalSemaphoreCount * sizeof(VkSemaphore);
		totalSize += pSubmits[i].commandBufferCount * sizeof(VkCommandBuffer);

		for(const auto *extension = reinterpret_cast<const VkBaseInStructure *>(pSubmits[i].pNext);
		    extension != nullptr; extension = reinterpret_cast<const VkBaseInStructure *>(extension->pNext))
		{
			switch(extension->sType)
			{
			case VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO:
				{
					const auto *tlsSubmitInfo = reinterpret_cast<const VkTimelineSemaphoreSubmitInfo *>(extension);
					totalSize += tlsSubmitInfo->waitSemaphoreValueCount * sizeof(uint64_t);
					totalSize += tlsSubmitInfo->signalSemaphoreValueCount * sizeof(uint64_t);
				}
				break;
			case VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO:
				// SwiftShader doesn't use device group submit info because it only supports a single physical device.
				// However, this extension is core in Vulkan 1.1, so we must treat it as a valid structure type.
				break;
			case VK_STRUCTURE_TYPE_MAX_ENUM:
				// dEQP tests that this value is ignored.
				break;
			default:
				UNSUPPORTED("submitInfo[%d]->pNext sType: %s", i, vk::Stringify(extension->sType).c_str());
				break;
			}
		}
	}

	uint8_t *mem = static_cast<uint8_t *>(
	    vk::allocateHostMemory(totalSize, vk::REQUIRED_MEMORY_ALIGNMENT, vk::NULL_ALLOCATION_CALLBACKS, vk::Fence::GetAllocationScope()));

	auto submits = new(mem) SubmitInfo[submitCount];
	mem += submitSize;

	for(uint32_t i = 0; i < submitCount; i++)
	{
		submits[i].commandBufferCount = pSubmits[i].commandBufferCount;
		submits[i].signalSemaphoreCount = pSubmits[i].signalSemaphoreCount;
		submits[i].waitSemaphoreCount = pSubmits[i].waitSemaphoreCount;

		submits[i].pWaitSemaphores = nullptr;
		submits[i].pWaitDstStageMask = nullptr;
		submits[i].pSignalSemaphores = nullptr;
		submits[i].pCommandBuffers = nullptr;

		if(pSubmits[i].waitSemaphoreCount > 0)
		{
			size_t size = pSubmits[i].waitSemaphoreCount * sizeof(VkSemaphore);
			submits[i].pWaitSemaphores = reinterpret_cast<const VkSemaphore *>(mem);
			memcpy(mem, pSubmits[i].pWaitSemaphores, size);
			mem += size;

			size = pSubmits[i].waitSemaphoreCount * sizeof(VkPipelineStageFlags);
			submits[i].pWaitDstStageMask = reinterpret_cast<const VkPipelineStageFlags *>(mem);
			memcpy(mem, pSubmits[i].pWaitDstStageMask, size);
			mem += size;
		}

		if(pSubmits[i].signalSemaphoreCount > 0)
		{
			size_t size = pSubmits[i].signalSemaphoreCount * sizeof(VkSemaphore);
			submits[i].pSignalSemaphores = reinterpret_cast<const VkSemaphore *>(mem);
			memcpy(mem, pSubmits[i].pSignalSemaphores, size);
			mem += size;
		}

		if(pSubmits[i].commandBufferCount > 0)
		{
			size_t size = pSubmits[i].commandBufferCount * sizeof(VkCommandBuffer);
			submits[i].pCommandBuffers = reinterpret_cast<const VkCommandBuffer *>(mem);
			memcpy(mem, pSubmits[i].pCommandBuffers, size);
			mem += size;
		}

		submits[i].waitSemaphoreValueCount = 0;
		submits[i].pWaitSemaphoreValues = nullptr;
		submits[i].signalSemaphoreValueCount = 0;
		submits[i].pSignalSemaphoreValues = nullptr;

		for(const auto *extension = reinterpret_cast<const VkBaseInStructure *>(pSubmits[i].pNext);
		    extension != nullptr; extension = reinterpret_cast<const VkBaseInStructure *>(extension->pNext))
		{
			switch(extension->sType)
			{
			case VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO:
				{
					const VkTimelineSemaphoreSubmitInfo *tlsSubmitInfo = reinterpret_cast<const VkTimelineSemaphoreSubmitInfo *>(extension);

					if(tlsSubmitInfo->waitSemaphoreValueCount > 0)
					{
						submits[i].waitSemaphoreValueCount = tlsSubmitInfo->waitSemaphoreValueCount;
						size_t size = tlsSubmitInfo->waitSemaphoreValueCount * sizeof(uint64_t);
						submits[i].pWaitSemaphoreValues = reinterpret_cast<uint64_t *>(mem);
						memcpy(mem, tlsSubmitInfo->pWaitSemaphoreValues, size);
						mem += size;
					}

					if(tlsSubmitInfo->signalSemaphoreValueCount > 0)
					{
						submits[i].signalSemaphoreValueCount = tlsSubmitInfo->signalSemaphoreValueCount;
						size_t size = tlsSubmitInfo->signalSemaphoreValueCount * sizeof(uint64_t);
						submits[i].pSignalSemaphoreValues = reinterpret_cast<uint64_t *>(mem);
						memcpy(mem, tlsSubmitInfo->pSignalSemaphoreValues, size);
						mem += size;
					}
				}
				break;
			case VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO:
				// SwiftShader doesn't use device group submit info because it only supports a single physical device.
				// However, this extension is core in Vulkan 1.1, so we must treat it as a valid structure type.
				break;
			case VK_STRUCTURE_TYPE_MAX_ENUM:
				// dEQP tests that this value is ignored.
				break;
			default:
				UNSUPPORTED("submitInfo[%d]->pNext sType: %s", i, vk::Stringify(extension->sType).c_str());
				break;
			}
		}
	}

	return submits;
}

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
	if(fence)
	{
		task.events = fence->getCountedEvent();
		task.events->add();
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
		SubmitInfo &submitInfo = task.pSubmits[i];
		for(uint32_t j = 0; j < submitInfo.waitSemaphoreCount; j++)
		{
			if(auto *sem = DynamicCast<TimelineSemaphore>(submitInfo.pWaitSemaphores[j]))
			{
				ASSERT(j < submitInfo.waitSemaphoreValueCount);
				sem->wait(submitInfo.pWaitSemaphoreValues[j]);
			}
			else if(auto *sem = DynamicCast<BinarySemaphore>(submitInfo.pWaitSemaphores[j]))
			{
				sem->wait(submitInfo.pWaitDstStageMask[j]);
			}
			else
			{
				UNSUPPORTED("Unknown semaphore type");
			}
		}

		{
			CommandBuffer::ExecutionState executionState;
			executionState.renderer = renderer.get();
			executionState.events = task.events.get();
			for(uint32_t j = 0; j < submitInfo.commandBufferCount; j++)
			{
				Cast(submitInfo.pCommandBuffers[j])->submit(executionState);
			}
		}

		for(uint32_t j = 0; j < submitInfo.signalSemaphoreCount; j++)
		{
			if(auto *sem = DynamicCast<TimelineSemaphore>(submitInfo.pSignalSemaphores[j]))
			{
				ASSERT(j < submitInfo.signalSemaphoreValueCount);
				sem->signal(submitInfo.pSignalSemaphoreValues[j]);
			}
			else if(auto *sem = DynamicCast<BinarySemaphore>(submitInfo.pSignalSemaphores[j]))
			{
				sem->signal();
			}
			else
			{
				UNSUPPORTED("Unknown semaphore type");
			}
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
		task.events->done();
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
	auto event = std::make_shared<sw::CountedEvent>();
	event->add();  // done() is called at the end of submitQueue()

	Task task;
	task.events = event;
	pending.put(task);

	event->wait();

	garbageCollect();

	return VK_SUCCESS;
}

void Queue::garbageCollect()
{
	while(true)
	{
		auto v = toDelete.tryTake();
		if(!v.second) { break; }
		vk::freeHostMemory(v.first, NULL_ALLOCATION_CALLBACKS);
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
		auto *semaphore = vk::DynamicCast<BinarySemaphore>(presentInfo->pWaitSemaphores[i]);
		semaphore->wait();
	}

	VkResult commandResult = VK_SUCCESS;

	for(uint32_t i = 0; i < presentInfo->swapchainCount; i++)
	{
		auto *swapchain = vk::Cast(presentInfo->pSwapchains[i]);
		VkResult perSwapchainResult = swapchain->present(presentInfo->pImageIndices[i]);

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

void Queue::beginDebugUtilsLabel(const VkDebugUtilsLabelEXT *pLabelInfo)
{
	// Optional debug label region
}

void Queue::endDebugUtilsLabel()
{
	// Close debug label region opened with beginDebugUtilsLabel()
}

void Queue::insertDebugUtilsLabel(const VkDebugUtilsLabelEXT *pLabelInfo)
{
	// Optional single debug label
}

}  // namespace vk
