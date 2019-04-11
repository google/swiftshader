#
# Copyright 2015 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)
swiftshader_root := $(LOCAL_PATH)

# Subzero is an alternative JIT compiler. It is smaller and generally slower.
REACTOR_USE_SUBZERO := false

# SwiftShader requires C++11.
# Full C++11 support is only available from Marshmallow and up.
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -lt 23 && echo PreMarshmallow),PreMarshmallow)
swiftshader_unsupported_build := true
endif

# Check whether $(TARGET_ARCH) is supported.
ifneq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),x86 x86_64 arm arm64))
swiftshader_unsupported_build := true
endif

ifneq ($(swiftshader_unsupported_build),true)
include $(swiftshader_root)/src/Android.mk
include $(swiftshader_root)/tests/GLESUnitTests/Android.mk
include $(swiftshader_root)/third_party/llvm-7.0/Android.mk
endif
