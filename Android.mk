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

LOCAL_PATH:= $(call my-dir)

ifndef SWIFTSHADER_OPTIM
SWIFTSHADER_OPTIM := release
endif

ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),x86 x86_64))
ifneq ($(filter gce_x86 gce calypso, $(TARGET_DEVICE))$(filter sdk_google_% google_sdk_%, $(TARGET_PRODUCT)),)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
endif