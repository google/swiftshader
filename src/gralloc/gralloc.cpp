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

#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <utils/String8.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include <api_level_fixes.h>

#include "GceFrameBufferConfig.h"
#include "gralloc_priv.h"
#include "gr.h"

/*****************************************************************************/

struct gralloc_context_t {
  alloc_device_t  device;
  /* our private data here */
};

static int gralloc_alloc_buffer(
    alloc_device_t* dev, int format, int w, int h,
    buffer_handle_t* pHandle, int* pStrideInPixels);

/*****************************************************************************/

static int gralloc_device_open(
    const hw_module_t* module, const char* name, hw_device_t** device);


/*****************************************************************************/

static struct hw_module_methods_t gralloc_module_methods = {
  .open = gralloc_device_open
};

struct private_module_t HAL_MODULE_INFO_SYM = {
  .base = {
    .common = {
      .tag = HARDWARE_MODULE_TAG,
      .version_major = 1,
      .version_minor = 0,
#ifdef GCE_CONVERTING_GRALLOC
      .id = "converting_gralloc",
#else
      .id = GRALLOC_HARDWARE_MODULE_ID,
#endif
      .name = "GCE X86 Graphics Memory Allocator Module",
      .author = "The Android Open Source Project",
      .methods = &gralloc_module_methods
    },
    .registerBuffer = gralloc_register_buffer,
    .unregisterBuffer = gralloc_unregister_buffer,
#ifdef GCE_CONVERTING_GRALLOC
    .lock = gralloc_converting_lock,
    .unlock = gralloc_converting_unlock,
#else
    .lock = gralloc_lock,
    .unlock = gralloc_unlock,
#endif
    },
  .framebuffer = 0,
  .remoter_socket = -1,
  .flags = 0,
  .numBuffers = 0,
  .bufferMask = 0,
  .lock = PTHREAD_MUTEX_INITIALIZER,
  .currentBuffer = 0,
};

/*****************************************************************************/

static int gralloc_alloc_framebuffer_locked(
    alloc_device_t* dev,
    buffer_handle_t* pHandle, int* pStrideInPixels) {
  static const GceFrameBufferConfig* config =
      GceFrameBufferConfig::getInstance();

  private_module_t* m = reinterpret_cast<private_module_t*>(
      dev->common.module);
  // allocate the framebuffer
  if (m->framebuffer == NULL) {
    // The framebuffer is mapped once and forever.
    int err = mapUserspaceFrameBufferLocked(m);
    if (err < 0) {
      ALOGE("Failed to map framebuffer (%d)", errno);
      return err;
    }
  }

  const uint32_t bufferMask = m->bufferMask;
  const uint32_t numBuffers = m->numBuffers;
  const size_t bufferSize = m->finfo.line_length * m->info.yres;
  if (numBuffers == 1) {
    // If we have only one buffer, we never use page-flipping. Instead,
    // we return a regular buffer which will be memcpy'ed to the main
    // screen when post is called.
    return gralloc_alloc_buffer(
        dev, config->hal_format(), config->x_res(), config->y_res(),
        pHandle, pStrideInPixels);
  }

  if (bufferMask >= ((1LU<<numBuffers)-1)) {
    // We ran out of buffers.
    return -ENOMEM;
  }

  // create a "fake" handles for it
  intptr_t vaddr = intptr_t(m->framebuffer->base);
  private_handle_t* hnd = new private_handle_t(
      dup(m->framebuffer->fd), bufferSize,
      config->hal_format(), config->x_res(), config->y_res(),
      private_handle_t::PRIV_FLAGS_FRAMEBUFFER);

  // find a free slot
  for (uint32_t i=0 ; i<numBuffers ; i++) {
    if ((bufferMask & (1LU << i)) == 0) {
      m->bufferMask |= (1LU << i);
      break;
    }
    vaddr += bufferSize;
  }

  hnd->base = reinterpret_cast<void*>(vaddr);
  hnd->frame_offset = vaddr - intptr_t(m->framebuffer->base);
  *pHandle = hnd;
  *pStrideInPixels = config->line_length() / (config->bits_per_pixel() / 8);

  return 0;
}

static int gralloc_alloc_framebuffer(
    alloc_device_t* dev,
    buffer_handle_t* pHandle, int* pStrideInPixels) {
  private_module_t* m = reinterpret_cast<private_module_t*>(
      dev->common.module);
  pthread_mutex_lock(&m->lock);
  int err = gralloc_alloc_framebuffer_locked(dev, pHandle, pStrideInPixels);
  pthread_mutex_unlock(&m->lock);
  return err;
}

static int gralloc_alloc_buffer(
    alloc_device_t* dev, int format, int w, int h,
    buffer_handle_t* pHandle, int* pStrideInPixels) {
  int err = 0;
  int fd = -1;
  static int sequence = 0;

  int bytes_per_pixel = formatToBytesPerPixel(format);
  int bytes_per_line = GceFrameBufferConfig::align(bytes_per_pixel * w);
  int size = GceFrameBufferConfig::align(sizeof(gralloc_buffer_control_t));
  int primary_offset = size;
  size = roundUpToPageSize(size + bytes_per_line * h);
  size += PAGE_SIZE;
  int secondary_offset = 0;
  if (bytes_per_pixel != 4) {
    secondary_offset = size;
    size += roundUpToPageSize(GceFrameBufferConfig::align(4 * w) * h);
    size += PAGE_SIZE;
  }

  fd = ashmem_create_region(
      android::String8::format(
          "gralloc-%d.%d", getpid(), sequence++).string(),
      size);
  if (fd < 0) {
    ALOGE("couldn't create ashmem (%s)", strerror(-errno));
    err = -errno;
  }

  if (err == 0) {
    private_handle_t* hnd = new private_handle_t(fd, size, format, w, h, 0);
    hnd->primary_offset = primary_offset;
    hnd->secondary_offset = secondary_offset;
    gralloc_module_t* module = reinterpret_cast<gralloc_module_t*>(
        dev->common.module);
    err = mapBuffer(module, hnd);
    if (err == 0) {
      reinterpret_cast<gralloc_buffer_control_t*>(hnd->base)->last_locked =
          gralloc_buffer_control_t::PRIMARY;
      *pHandle = hnd;
      *pStrideInPixels = bytes_per_line / bytes_per_pixel;
    }
  }

  ALOGE_IF(err, "gralloc failed err=%s", strerror(-err));

  return err;
}

/*****************************************************************************/

static int gralloc_alloc(
    alloc_device_t* dev, int w, int h, int format, int usage,
    buffer_handle_t* pHandle, int* pStrideInPixels) {
  if (!pHandle || !pStrideInPixels)
    return -EINVAL;

  int err;
  if (usage & GRALLOC_USAGE_HW_FB) {
    err = gralloc_alloc_framebuffer(dev, pHandle, pStrideInPixels);
  } else {
    err = gralloc_alloc_buffer(dev, format, w, h, pHandle, pStrideInPixels);
  }

  if (err < 0) {
    return err;
  }
  return 0;
}

static int gralloc_free(alloc_device_t* dev,
        buffer_handle_t handle) {
  if (private_handle_t::validate(handle) < 0)
    return -EINVAL;

  private_handle_t const* hnd = reinterpret_cast<private_handle_t const*>(handle);
  if (hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) {
    // free this buffer
    private_module_t* m = reinterpret_cast<private_module_t*>(
        dev->common.module);
    const size_t bufferSize = m->finfo.line_length * m->info.yres;
    int index = (uintptr_t(hnd->base) - uintptr_t(m->framebuffer->base)) / bufferSize;
    m->bufferMask &= ~(1 << index);
  } else {
    gralloc_module_t* module = reinterpret_cast<gralloc_module_t*>(
        dev->common.module);
    terminateBuffer(module, const_cast<private_handle_t*>(hnd));
  }

  close(hnd->fd);
  delete hnd;
  return 0;
}

/*****************************************************************************/

static int gralloc_close(struct hw_device_t *dev) {
  gralloc_context_t* ctx = reinterpret_cast<gralloc_context_t*>(dev);
  if (ctx) {
    /* TODO: keep a list of all buffer_handle_t created, and free them
     * all here.
     */
    free(ctx);
  }
  return 0;
}

int gralloc_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device) {
  int status = -EINVAL;
  if (!strcmp(name, GRALLOC_HARDWARE_GPU0)) {
    gralloc_context_t *dev;
    dev = (gralloc_context_t*)malloc(sizeof(*dev));

    /* initialize our state here */
    memset(dev, 0, sizeof(*dev));

    /* initialize the procs */
    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version = 0;
    dev->device.common.module = const_cast<hw_module_t*>(module);
    dev->device.common.close = gralloc_close;

    dev->device.alloc   = gralloc_alloc;
    dev->device.free    = gralloc_free;

    *device = &dev->device.common;
    status = 0;
  } else {
    status = fb_device_open(module, name, device);
  }
  return status;
}
