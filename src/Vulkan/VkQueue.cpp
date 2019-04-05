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

#include "VkCommandBuffer.hpp"
#include "VkFence.hpp"
#include "VkQueue.hpp"
#include "VkSemaphore.hpp"
#include "Device/Renderer.hpp"
#include "WSI/VkSwapchainKHR.hpp"

namespace vk
{

Queue::Queue(uint32_t pFamilyIndex, float pPriority) : familyIndex(pFamilyIndex), priority(pPriority)
{
	context = new sw::Context();
	renderer = new sw::Renderer(context, sw::OpenGL, true);
}

void Queue::destroy()
{
	delete context;
	delete renderer;
}

void Queue::submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence)
{
	for(uint32_t i = 0; i < submitCount; i++)
	{
		auto& submitInfo = pSubmits[i];
		for(uint32_t j = 0; j < submitInfo.waitSemaphoreCount; j++)
		{
			vk::Cast(submitInfo.pWaitSemaphores[j])->wait(submitInfo.pWaitDstStageMask[j]);
		}

		{
			CommandBuffer::ExecutionState executionState;
			executionState.renderer = renderer;
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

	// FIXME (b/117835459): signal the fence only once the work is completed
	if(fence != VK_NULL_HANDLE)
	{
		vk::Cast(fence)->signal();
	}
}

void Queue::waitIdle()
{
	// equivalent to submitting a fence to a queue and waiting
	// with an infinite timeout for that fence to signal

	// FIXME (b/117835459): implement once we have working fences
}

#ifndef __ANDROID__
void Queue::present(const VkPresentInfoKHR* presentInfo)
{
	for(uint32_t i = 0; i < presentInfo->waitSemaphoreCount; i++)
	{
		vk::Cast(presentInfo->pWaitSemaphores[i])->wait();
	}

	for(uint32_t i = 0; i < presentInfo->swapchainCount; i++)
	{
		vk::Cast(presentInfo->pSwapchains[i])->present(presentInfo->pImageIndices[i]);
	}
}
#endif

} // namespace vk
