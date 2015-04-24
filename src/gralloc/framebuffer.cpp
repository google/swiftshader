/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/mman.h>

#include <dlfcn.h>

#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include <sys/system_properties.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/sockets.h>
#include <private/android_filesystem_config.h>

#if HAVE_ANDROID_OS
#include <linux/fb.h>
#endif

#include "gralloc_priv.h"
#include "gr.h"

#include <GceFrameBufferConfig.h>
#include "remoter_framework_pkt.h"

/*****************************************************************************/

// numbers of buffers for page flipping
#define NUM_BUFFERS 2

enum {
    PAGE_FLIP = 0x00000001,
    LOCKED = 0x00000002
};

struct fb_context_t {
    framebuffer_device_t  device;
};

/*****************************************************************************/

static int fb_setSwapInterval(struct framebuffer_device_t* dev,
            int interval)
{
    fb_context_t* ctx = (fb_context_t*)dev;
    if (interval < dev->minSwapInterval || interval > dev->maxSwapInterval)
        return -EINVAL;
    // FIXME: implement fb_setSwapInterval
    return 0;
}

static int fb_setUpdateRect(struct framebuffer_device_t* dev,
        int l, int t, int w, int h)
{
    if (((w|h) <= 0) || ((l|t)<0))
        return -EINVAL;
        
    fb_context_t* ctx = (fb_context_t*)dev;
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);
    if (m->remoter_socket != -1) {
        struct remoter_request_packet pkt;
        remoter_request_packet_init(&pkt, REMOTER_OP_FB_UPDATE_RECT, 0);
        pkt.params.fb_update_rect_params.left = l;
        pkt.params.fb_update_rect_params.top = t;
        pkt.params.fb_update_rect_params.width = w;
        pkt.params.fb_update_rect_params.height = h;
        if (write(m->remoter_socket, &pkt, sizeof(pkt)) < 0) {
            ALOGE("Remoter socket write failed (%s)", strerror(errno));
        }
    } else
        ALOGW("Remoter socket not connected!");
    return 0;
}

static int fb_post(struct framebuffer_device_t* dev, buffer_handle_t buffer)
{
    if (private_handle_t::validate(buffer) < 0)
        return -EINVAL;

    fb_context_t* ctx = (fb_context_t*)dev;

    private_handle_t const* hnd = reinterpret_cast<private_handle_t const*>(buffer);
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);

    if (hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) {
        const size_t offset = uintptr_t(hnd->base) - uintptr_t(m->framebuffer->base);
        m->info.yoffset = offset / m->finfo.line_length;
        if (m->remoter_socket != -1) {
            struct remoter_request_packet pkt;
            remoter_request_packet_init(&pkt, REMOTER_OP_FB_POST, 0);
            pkt.params.fb_post_params.y_offset  = m->info.yoffset;
            if (write(m->remoter_socket, &pkt, sizeof(pkt)) < 0) {
                ALOGE("Remoter socket write failed (%s)", strerror(errno));
            }
        } else
            ALOGW("Remoter socket not connected!");
        m->currentBuffer = buffer;
    } else {
        // If we can't do the page_flip, just copy the buffer to the front 
        // FIXME: use copybit HAL instead of memcpy
        
        void* fb_vaddr;
        void* buffer_vaddr;
        
        m->base.lock(&m->base, m->framebuffer, 
                GRALLOC_USAGE_SW_WRITE_RARELY, 
                0, 0, m->info.xres, m->info.yres,
                &fb_vaddr);

        m->base.lock(&m->base, buffer, 
                GRALLOC_USAGE_SW_READ_RARELY, 
                0, 0, m->info.xres, m->info.yres,
                &buffer_vaddr);

        memcpy(fb_vaddr, buffer_vaddr, m->finfo.line_length * m->info.yres);
        
        m->base.unlock(&m->base, buffer); 
        m->base.unlock(&m->base, m->framebuffer); 
    }
    
    return 0;
}

int wait_for_remoter() {
    char prop_value[PROP_VALUE_MAX];
    const char* prop_name = "ro.remoter_ready";
    int retries = (60 * 5);

    ALOGI("wait_for_remoter():");
    while(retries--) {
        if (property_get(prop_name, prop_value, NULL) <= 0) {
            sleep(1);
            continue;
        } else {
            ALOGI("wait_for_remoter(): Remoter ready");
            break;
        }
    }
    if (!retries) {
        ALOGE("Timed out waiting for remoter");
        return -1;
    }
    return 0;
}

/*****************************************************************************/
int mapUserspaceFrameBufferLocked(struct private_module_t* module)
{
    if (module->framebuffer) {
        return 0;
    }

    if (wait_for_remoter()) {
        ALOGE("Remoter not available");
        return -ENOENT;
    }

    int fd;
    if ((fd = open(GceFrameBufferConfig::kFrameBufferPath, O_RDWR, 0)) < 0) {
      ALOGE("Failed to open '%s' (%s)",
            GceFrameBufferConfig::kFrameBufferPath, strerror(errno));
      return -errno;
    }

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo info;
    memset(&info, 0, sizeof(info));

    const GceFrameBufferConfig* config = GceFrameBufferConfig::getInstance();

    info.xres = config->x_res();
    info.yres = config->y_res();
    info.bits_per_pixel = config->bits_per_pixel();

    info.red.offset = GceFrameBufferConfig::kRedShift;
    info.red.length = GceFrameBufferConfig::kRedBits;
    info.green.offset = GceFrameBufferConfig::kGreenShift;
    info.green.length = GceFrameBufferConfig::kGreenBits;
    info.blue.offset = GceFrameBufferConfig::kBlueShift;
    info.blue.length = GceFrameBufferConfig::kBlueBits;
    info.xres_virtual = info.xres;
    info.yres_virtual = config->y_res_virtual();
    info.xoffset = 0;
    info.yoffset = 0;
    info.pixclock = 0;
    info.vmode = FB_VMODE_NONINTERLACED;
    info.width  = ((info.xres * 25.4f) / config->dpi() + 0.5f);
    info.height = ((info.yres * 25.4f) / config->dpi() + 0.5f);

    memset(&finfo, 0, sizeof(finfo));
    strcpy(finfo.id, "Userspace FB");
    finfo.type = FB_TYPE_PACKED_PIXELS;
    finfo.line_length = info.xres * (info.bits_per_pixel / 8);
    finfo.accel = FB_ACCEL_NONE;

    module->flags = PAGE_FLIP;
    module->info = info;
    module->finfo = finfo;
    module->xdpi = (info.xres * 25.4f) / info.width;
    module->ydpi = (info.yres * 25.4f) / info.height;
    module->fps = (60 * 1000) / 1000.0f;

    /*
     * map the framebuffer
     */

    size_t fbSize = roundUpToPageSize(finfo.line_length * info.yres_virtual);
    module->framebuffer = new private_handle_t(
        dup(fd), fbSize,
        config->hal_format(), config->x_res(), config->y_res(), 0);
    module->numBuffers = info.yres_virtual / info.yres;
    module->bufferMask = 0;

    void* vaddr = mmap(0, fbSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (vaddr == MAP_FAILED) {
        ALOGE("Error mapping the framebuffer (%s)", strerror(errno));
        return -errno;
    }
    module->framebuffer->base = vaddr;
    memset(vaddr, 0, fbSize);

    // Connect to the remoter so we can notify it of posted buffers and updated
    // regions.
    if ((module->remoter_socket = socket_local_client(
        "remoter", ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM)) < 0) {
        ALOGE("Error connecting to remoter (%s)", strerror(errno));
        return -errno;
    }
    
    return 0;
}

static int mapUserspaceFrameBuffer(struct private_module_t* module)
{
    pthread_mutex_lock(&module->lock);
    int err = mapUserspaceFrameBufferLocked(module);
    pthread_mutex_unlock(&module->lock);
    return err;
}


/*****************************************************************************/

static int fb_close(struct hw_device_t *dev)
{
    fb_context_t* ctx = (fb_context_t*)dev;
    if (ctx) {
        free(ctx);
    }
    return 0;
}

int fb_device_open(hw_module_t const* module, const char* name,
        hw_device_t** device)
{
    int status = -EINVAL;
    if (!strcmp(name, GRALLOC_HARDWARE_FB0)) {
        char prop_value[PATH_MAX];
        /* initialize our state here */
        fb_context_t *dev = (fb_context_t*)malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module = const_cast<hw_module_t*>(module);
        dev->device.common.close = fb_close;
        dev->device.setSwapInterval = fb_setSwapInterval;
        dev->device.post            = fb_post;
        dev->device.setUpdateRect   = fb_setUpdateRect;

        private_module_t* m = (private_module_t*)module;

        status = mapUserspaceFrameBuffer(m);

        if (status >= 0) {
            int stride = m->finfo.line_length / (m->info.bits_per_pixel >> 3);
            int format =
                GceFrameBufferConfig::getInstance()->hal_format();
            const_cast<uint32_t&>(dev->device.flags) = 0;
            const_cast<uint32_t&>(dev->device.width) = m->info.xres;
            const_cast<uint32_t&>(dev->device.height) = m->info.yres;
            const_cast<int&>(dev->device.stride) = stride;
            const_cast<int&>(dev->device.format) = format;
            const_cast<float&>(dev->device.xdpi) = m->xdpi;
            const_cast<float&>(dev->device.ydpi) = m->ydpi;
            const_cast<float&>(dev->device.fps) = m->fps;
            const_cast<int&>(dev->device.minSwapInterval) = 1;
            const_cast<int&>(dev->device.maxSwapInterval) = 1;
            *device = &dev->device.common;
        }
    }
    return status;
}
