LOCAL_PATH:= $(call my-dir)

COMMON_CFLAGS := \
	-DLOG_TAG=\"libEGL_swiftshader\" \
	-std=c++11 \
	-DEGLAPI= \
	-DEGL_EGLEXT_PROTOTYPES \
	-Wno-unused-parameter \
	-Wno-implicit-exception-spec-mismatch \
	-Wno-overloaded-virtual

COMMON_SRC_FILES := \
	Config.cpp \
	Display.cpp \
	Surface.cpp \
	libEGL.cpp \
	main.cpp

COMMON_C_INCLUDES := \
	bionic \
	$(GCE_STLPORT_INCLUDES) \
	device/google/gce/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../../

COMMON_STATIC_LIBRARIES := \
	swiftshader_top \
	libLLVM_swiftshader \
	libgcemetadata

COMMON_SHARED_LIBRARIES := \
	libgceframebufferconfig \
	libdl \
	liblog \
	libandroid \
	libutils \
	libcutils \
	libhardware \
	$(GCE_STLPORT_LIBS)

COMMON_LDFLAGS := \
	-Wl,--version-script=$(LOCAL_PATH)/exports.map \
	-Wl,--hash-style=sysv

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/debug/obj
LOCAL_UNSTRIPPED_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/debug/sym
LOCAL_MODULE := libEGL_swiftshader_vendor_debug
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libEGL_swiftshader.so
LOCAL_CFLAGS += $(COMMON_CFLAGS) -UNDEBUG -g -O0

LOCAL_CLANG := true
LOCAL_SRC_FILES := $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/release/obj
LOCAL_UNSTRIPPED_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/release/sym
LOCAL_MODULE := libEGL_swiftshader_vendor_release
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libEGL_swiftshader.so
LOCAL_CFLAGS += $(COMMON_CFLAGS) -DANGLE_DISABLE_TRACE

LOCAL_CLANG := true
LOCAL_SRC_FILES := $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
include $(BUILD_SHARED_LIBRARY)
