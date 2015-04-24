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
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <utils/ashmem.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>
#include <system/graphics.h>

#include <libyuv/convert_argb.h>
#include <libyuv/convert_from.h>

#include "GceFrameBufferConfig.h"

#include "gralloc_priv.h"


/* desktop Linux needs a little help with gettid() */
#if defined(ARCH_X86) && !defined(HAVE_ANDROID_OS)
#define __KERNEL__
# include <linux/unistd.h>
pid_t gettid() { return syscall(__NR_gettid);}
#undef __KERNEL__
#endif

static const char* buffer_name(
    private_handle_t* hnd, char output[ASHMEM_NAME_LEN]) {
  output[0] = '\0';
  if (!hnd) {
    ALOGE("Attempted to log gralloc name hnd=NULL");
    return output;
  }
  if (hnd->fd == -1) {
    ALOGE("Attempted to log gralloc name hnd=%p with fd == -1", hnd);
    return output;
  }
  int rval = ioctl(hnd->fd, ASHMEM_GET_NAME, output);
  if (rval == -1) {
    ALOGE("ASHMEM_GET_NAME failed, hnd=%p fd=%d (%s)", hnd, hnd->fd,
        strerror(errno));
  }
  return output;
}

/*****************************************************************************/

static int gralloc_map(gralloc_module_t const* /*module*/,
        buffer_handle_t handle,
        void** vaddr) {
  char name_buf[ASHMEM_NAME_LEN];
  private_handle_t* hnd = (private_handle_t*)handle;
  if (!(hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER)) {
    size_t size = hnd->total_size;
    void* mappedAddress = mmap(
        0, size, PROT_READ|PROT_WRITE, MAP_SHARED, hnd->fd, 0);
    if (mappedAddress == MAP_FAILED) {
      ALOGE("Could not mmap %s", strerror(errno));
      return -errno;
    }
    // Set up the guard pages. The last page is always a guard
    uintptr_t base = uintptr_t(mappedAddress);
    uintptr_t addr = base + hnd->total_size - PAGE_SIZE;
    if (mprotect((void*)addr, PAGE_SIZE, PROT_NONE) == -1) {
      ALOGE("mprotect base=%p, pg=%p failed (%s)",
            (void*)base, (void*)addr, strerror(errno));
    }
    // If we have a secondary buffer, then the page before it is also
    // a guard page.
    if (hnd->secondary_offset) {
      addr = base + hnd->secondary_offset - PAGE_SIZE;
      if (mprotect((void*)addr, PAGE_SIZE, PROT_NONE) == -1) {
        ALOGE("mprotect base=%p, sec_pg=%p failed (%s)",
              (void*)base, (void*)addr, strerror(errno));
      }
    }
    hnd->base = reinterpret_cast<void*>(
        uintptr_t(mappedAddress) + hnd->frame_offset);
    ALOGI("Mapped %s hnd=%p fd=%d base=%p", buffer_name(hnd, name_buf), hnd,
          hnd->fd, hnd->base);
    ALOGI("... %s format=%d width=%d height=%d", name_buf, hnd->format,
          hnd->x_res, hnd->y_res);
    //ALOGD("gralloc_map() succeeded fd=%d, off=%d, size=%d, vaddr=%p",
    //        hnd->fd, hnd->offset, hnd->size, mappedAddress);
  } else {
    ALOGI("Mapped framebuffer hnd=%p base=%p", hnd, hnd->base);
  }
  *vaddr = (void*)hnd->base;
  return 0;
}

static int gralloc_unmap(gralloc_module_t const* /*module*/,
        buffer_handle_t handle) {
  char name_buf[ASHMEM_NAME_LEN];

  private_handle_t* hnd = (private_handle_t*)handle;
  if (!(hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER)) {
    ALOGI("Unmapped %s hnd=%p fd=%d base=%p", buffer_name(hnd, name_buf), hnd,
          hnd->fd, hnd->base);
    if (munmap(hnd->base, hnd->total_size) < 0) {
      ALOGE("Could not unmap %s", strerror(errno));
    }
  }
  hnd->base = 0;
  return 0;
}

/*****************************************************************************/

int gralloc_register_buffer(gralloc_module_t const* module,
        buffer_handle_t handle) {
  char name_buf[ASHMEM_NAME_LEN];

  if (private_handle_t::validate(handle) < 0)
    return -EINVAL;

  // *** WARNING WARNING WARNING ***
  //
  // If a buffer handle is passed from the process that allocated it to a
  // different process, and then back to the allocator process, we will
  // create a second mapping of the buffer. If the process reads and writes
  // through both mappings, normal memory ordering guarantees may be
  // violated, depending on the processor cache implementation*.
  //
  // If you are deriving a new gralloc implementation from this code, don't
  // do this. A "real" gralloc should provide a single reference-counted
  // mapping for each buffer in a process.
  //
  // In the current system, there is one case that needs a buffer to be
  // registered in the same process that allocated it. The SurfaceFlinger
  // process acts as the IGraphicBufferAlloc Binder provider, so all gralloc
  // allocations happen in its process. After returning the buffer handle to
  // the IGraphicBufferAlloc client, SurfaceFlinger free's its handle to the
  // buffer (unmapping it from the SurfaceFlinger process). If
  // SurfaceFlinger later acts as the producer end of the buffer queue the
  // buffer belongs to, it will get a new handle to the buffer in response
  // to IGraphicBufferProducer::requestBuffer(). Like any buffer handle
  // received through Binder, the SurfaceFlinger process will register it.
  // Since it already freed its original handle, it will only end up with
  // one mapping to the buffer and there will be no problem.
  //
  // Currently SurfaceFlinger only acts as a buffer producer for a remote
  // consumer when taking screenshots and when using virtual displays.
  //
  // Eventually, each application should be allowed to make its own gralloc
  // allocations, solving the problem. Also, this ashmem-based gralloc
  // should go away, replaced with a real ion-based gralloc.
  //
  // * Specifically, associative virtually-indexed caches are likely to have
  //   problems. Most modern L1 caches fit that description.

  private_handle_t* hnd = (private_handle_t*)handle;
   ALOGI("Registered %s hnd=%p fd=%d", buffer_name(hnd, name_buf), hnd,
          hnd->fd);
   ALOGD_IF(hnd->allocating_pid == getpid(),
           "Registering a buffer in the process that created it. "
           "This may cause memory ordering problems.");

  void *vaddr;
  return gralloc_map(module, handle, &vaddr);
}

int gralloc_unregister_buffer(gralloc_module_t const* module,
        buffer_handle_t handle) {
  char name_buf[ASHMEM_NAME_LEN];

  if (private_handle_t::validate(handle) < 0)
    return -EINVAL;

  private_handle_t* hnd = (private_handle_t*)handle;
  ALOGI("Unregistered %s hnd=%p fd=%d", buffer_name(hnd, name_buf), hnd,
        hnd->fd);
  if (hnd->base)
    gralloc_unmap(module, handle);

  return 0;
}

int mapBuffer(gralloc_module_t const* module,
        private_handle_t* hnd) {
  void* vaddr;
  return gralloc_map(module, hnd, &vaddr);
}

int terminateBuffer(gralloc_module_t const* module,
        private_handle_t* hnd) {
  if (hnd->base) {
    // this buffer was mapped, unmap it now
    gralloc_unmap(module, hnd);
  }

  return 0;
}

#ifdef GCE_CONVERTING_GRALLOC
int gralloc_converting_lock(
    gralloc_module_t const* module, buffer_handle_t handle, int usage,
    int l, int t, int w, int h, void** vaddr) {
  char name_buf[ASHMEM_NAME_LEN];

  if (private_handle_t::validate(handle) < 0) {
    return -EINVAL;
  }

  private_handle_t* hnd = (private_handle_t*)handle;
  buffer_name(hnd, name_buf);
  gralloc_buffer_control_t* control = NULL;
  if (hnd->primary_offset) {
    control = reinterpret_cast<gralloc_buffer_control_t*>(hnd->base);
  }
  unsigned char* primary_buffer = reinterpret_cast<unsigned char*>(
      uintptr_t(hnd->base) + hnd->primary_offset);
  unsigned char* secondary_buffer = hnd->secondary_offset ?
      reinterpret_cast<unsigned char*>(
          uintptr_t(hnd->base) + hnd->secondary_offset) : primary_buffer;

  if (hnd->format == HAL_PIXEL_FORMAT_RGB_565) {
    if (control &&
        (control->last_locked == gralloc_buffer_control_t::PRIMARY)) {
      ALOGI("Converting RGB16 to RGB32 %s", name_buf);
      libyuv::RGB565ToARGB(
          primary_buffer,  GceFrameBufferConfig::align(hnd->x_res * 2),
          secondary_buffer, GceFrameBufferConfig::align(hnd->x_res * 4),
          hnd->x_res, hnd->y_res);
      control->last_locked = gralloc_buffer_control_t::SECONDARY;
    } else {
      ALOGE("Can't convert: RGB16, but no control structure %s", name_buf);
    }
    *vaddr = secondary_buffer;
  } else {
    *vaddr = primary_buffer;
  }
  ALOGI("Locking buffer %s hnd=%p secondary=%d at=%p",
        name_buf, hnd, (control ? control->last_locked : 0), *vaddr);
  return 0;
}

int gralloc_converting_unlock(gralloc_module_t const* module,
        buffer_handle_t handle) {
  if (private_handle_t::validate(handle) < 0)
    return -EINVAL;
  return 0;
}
#else
int gralloc_lock(gralloc_module_t const* /*module*/,
        buffer_handle_t handle, int /*usage*/,
        int /*l*/, int /*t*/, int /*w*/, int /*h*/,
        void** vaddr) {
  char name_buf[ASHMEM_NAME_LEN];
  // this is called when a buffer is being locked for software
  // access. in thin implementation we have nothing to do since
  // not synchronization with the h/w is needed.
  // typically this is used to wait for the h/w to finish with
  // this buffer if relevant. the data cache may need to be
  // flushed or invalidated depending on the usage bits and the
  // hardware.

  if (private_handle_t::validate(handle) < 0)
    return -EINVAL;

  private_handle_t* hnd = (private_handle_t*)handle;
  buffer_name(hnd, name_buf);
  gralloc_buffer_control_t* control = NULL;
  if (hnd->primary_offset) {
    control = reinterpret_cast<gralloc_buffer_control_t*>(hnd->base);
  }
  unsigned char* primary_buffer = reinterpret_cast<unsigned char*>(
      uintptr_t(hnd->base) + hnd->primary_offset);
  unsigned char* secondary_buffer = hnd->secondary_offset ?
      reinterpret_cast<unsigned char*>(
          uintptr_t(hnd->base) + hnd->secondary_offset) : primary_buffer;
  if (control &&
      (control->last_locked == gralloc_buffer_control_t::SECONDARY)) {
    ALOGI("Converting RGB32 to RGB16 %s", name_buf);
    libyuv::ARGBToRGB565(
        secondary_buffer, GceFrameBufferConfig::align(hnd->x_res * 4),
        primary_buffer, GceFrameBufferConfig::align(hnd->x_res * 2),
        hnd->x_res, hnd->y_res);
    control->last_locked = gralloc_buffer_control_t::PRIMARY;
  }
  *vaddr = primary_buffer;
  return 0;
}

int gralloc_unlock(gralloc_module_t const* /*module*/,
        buffer_handle_t handle) {
  // we're done with a software buffer. nothing to do in this
  // implementation. typically this is used to flush the data cache.

  if (private_handle_t::validate(handle) < 0)
    return -EINVAL;
  return 0;
}

#endif
