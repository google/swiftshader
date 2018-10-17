#
# Copyright 2018 The Android Open-Source Project
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

ifneq ($(wildcard $(LOCAL_PATH)/../../third_party/googletest/googletest/src/gtest-all.cc),)

include $(CLEAR_VARS)

LOCAL_MODULE := libgtest_all_swiftshader
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true
LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
	../../third_party/googletest/googletest/src/gtest-all.cc

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../third_party/googletest/googletest/include/ \
	$(LOCAL_PATH)/../../third_party/googletest/googlemock/include/ \
	$(LOCAL_PATH)/../../third_party/googletest/googletest/

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := swiftshader-unittests
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
	main.cpp \
	unittests.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../third_party/googletest/googletest/include/ \
	$(LOCAL_PATH)/../../third_party/googletest/googlemock/include/ \
	$(LOCAL_PATH)/../../third_party/googletest/googletest/ \
	$(LOCAL_PATH)/../../include/

LOCAL_SHARED_LIBRARIES := \
	libEGL_swiftshader \
	libGLESv2_swiftshader

LOCAL_STATIC_LIBRARIES := \
	libgtest_all_swiftshader

include $(BUILD_EXECUTABLE)

endif  # gtest-all.cc exists
