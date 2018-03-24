/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <nativebase/nativebase.h>
#include <system/graphics.h>

#define ANDROID_NATIVE_WINDOW_MAGIC ANDROID_NATIVE_MAKE_CONSTANT('_', 'w', 'n', 'd')

enum {
    NATIVE_WINDOW_WIDTH = 0,
    NATIVE_WINDOW_HEIGHT = 1,
};

struct ANativeWindow {
    ANativeWindow() : flags(0), minSwapInterval(0), maxSwapInterval(0), xdpi(0), ydpi(0) {
        common.magic = ANDROID_NATIVE_BUFFER_MAGIC;
        common.version = sizeof(ANativeWindowBuffer);
        memset(common.reserved, 0, sizeof(common.reserved));
    }

    android_native_base_t common;

    const uint32_t flags;
    const int minSwapInterval;
    const int maxSwapInterval;
    const float xdpi;
    const float ydpi;
    intptr_t oem[4];

    int (*setSwapInterval)(ANativeWindow*, int);
    int (*dequeueBuffer_DEPRECATED)(ANativeWindow*, ANativeWindowBuffer**);
    int (*lockBuffer_DEPRECATED)(ANativeWindow*, ANativeWindowBuffer*);
    int (*queueBuffer_DEPRECATED)(ANativeWindow*, ANativeWindowBuffer*);
    int (*query)(const ANativeWindow*, int, int*);
    int (*perform)(ANativeWindow*, int, ...);
    int (*cancelBuffer_DEPRECATED)(ANativeWindow*, ANativeWindowBuffer*);
    int (*dequeueBuffer)(ANativeWindow*, ANativeWindowBuffer**, int*);
    int (*queueBuffer)(ANativeWindow*, ANativeWindowBuffer*, int);
    int (*cancelBuffer)(ANativeWindow*, ANativeWindowBuffer*, int);
};

static inline int native_window_set_usage(ANativeWindow*, uint64_t) {
    // No-op
    return 0;
}

static inline int native_window_dequeue_buffer_and_wait(ANativeWindow* anw,
                                                        ANativeWindowBuffer** anwb) {
    return anw->dequeueBuffer_DEPRECATED(anw, anwb);
}
