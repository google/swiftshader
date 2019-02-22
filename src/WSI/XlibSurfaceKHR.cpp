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

#include "XlibSurfaceKHR.hpp"

namespace vk {

XlibSurfaceKHR::XlibSurfaceKHR(const VkXlibSurfaceCreateInfoKHR *pCreateInfo, void *mem) :
		pDisplay(pCreateInfo->dpy),
		window(pCreateInfo->window) {

}

void XlibSurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator) {

}

size_t XlibSurfaceKHR::ComputeRequiredAllocationSize(const VkXlibSurfaceCreateInfoKHR *pCreateInfo) {
	return 0;
}

void XlibSurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const {
	SurfaceKHR::getSurfaceCapabilities(pSurfaceCapabilities);

	XWindowAttributes attr;
	libX11->XGetWindowAttributes(pDisplay, window, &attr);
	VkExtent2D extent = {static_cast<uint32_t>(attr.width), static_cast<uint32_t>(attr.height)};

	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
}

}