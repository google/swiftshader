LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CLANG := true

LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/egl
LOCAL_MODULE := libGLESv1_CM_swiftshader

LOCAL_SRC_FILES := \
	../../Common/CPUID.cpp \
	../../Common/Configurator.cpp \
	../../Common/Debug.cpp \
	../../Common/Half.cpp \
	../../Common/Math.cpp \
	../../Common/Memory.cpp \
	../../Common/Resource.cpp \
	../../Common/Socket.cpp \
	../../Common/Thread.cpp \
	../../Common/Timer.cpp

LOCAL_SRC_FILES += \
	../../Main/Config.cpp \
	../../Main/FrameBuffer.cpp \
	../../Main/FrameBufferAndroid.cpp \
	../../Main/Logo.cpp \
	../../Main/Register.cpp \
	../../Main/SwiftConfig.cpp \
	../../Main/crc.cpp \
	../../Main/serialvalid.cpp \

LOCAL_SRC_FILES += \
	../../Reactor/Nucleus.cpp \
	../../Reactor/Routine.cpp \
	../../Reactor/RoutineManager.cpp

LOCAL_SRC_FILES += \
	../../Renderer/Blitter.cpp \
	../../Renderer/Clipper.cpp \
	../../Renderer/Color.cpp \
	../../Renderer/Context.cpp \
	../../Renderer/Matrix.cpp \
	../../Renderer/PixelProcessor.cpp \
	../../Renderer/Plane.cpp \
	../../Renderer/Point.cpp \
	../../Renderer/QuadRasterizer.cpp \
	../../Renderer/Rasterizer.cpp \
	../../Renderer/Renderer.cpp \
	../../Renderer/Sampler.cpp \
	../../Renderer/SetupProcessor.cpp \
	../../Renderer/Surface.cpp \
	../../Renderer/TextureStage.cpp \
	../../Renderer/Vector.cpp \
	../../Renderer/VertexProcessor.cpp \

LOCAL_SRC_FILES += \
	../../Shader/Constants.cpp \
	../../Shader/PixelRoutine.cpp \
	../../Shader/PixelShader.cpp \
	../../Shader/SamplerCore.cpp \
	../../Shader/SetupRoutine.cpp \
	../../Shader/Shader.cpp \
	../../Shader/ShaderCore.cpp \
	../../Shader/VertexPipeline.cpp \
	../../Shader/VertexProgram.cpp \
	../../Shader/VertexRoutine.cpp \
	../../Shader/VertexShader.cpp \

LOCAL_SRC_FILES += \
	../common/NameSpace.cpp \
	../common/Object.cpp \
	../common/debug.cpp \
	../common/MatrixStack.cpp \

LOCAL_SRC_FILES += \
	Buffer.cpp \
	Context.cpp \
	Device.cpp \
	Framebuffer.cpp \
	Image.cpp \
	IndexDataManager.cpp \
	libGLES_CM.cpp \
	main.cpp \
	Renderbuffer.cpp \
	ResourceManager.cpp \
	Texture.cpp \
	utilities.cpp \
	VertexDataManager.cpp

LOCAL_CFLAGS += -DLOG_TAG=\"libGLES_CM_swiftshader\"
LOCAL_CFLAGS += -fno-operator-names -msse2 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
LOCAL_CFLAGS += -std=c++11

# Android's make system also uses NDEBUG, so we need to set/unset it forcefully
# Uncomment for ON:
LOCAL_CFLAGS += -UNDEBUG
# Uncomment for OFF:
#LOCAL_CFLAGS += -fomit-frame-pointer -ffunction-sections -fdata-sections -DNDEBUG -DANGLE_DISABLE_TRACE

LOCAL_SHARED_LIBRARIES += libdl liblog libcutils libhardware libui libutils
LOCAL_STATIC_LIBRARIES += libLLVM_swiftshader
LOCAL_LDFLAGS += -Wl,--gc-sections -Wl,--version-script=$(LOCAL_PATH)/exports.map -Wl,--hash-style=sysv

LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/../include \
        $(LOCAL_PATH)/../ \
        $(LOCAL_PATH)/../../ \
        $(LOCAL_PATH)/../../LLVM/include-android \
        $(LOCAL_PATH)/../../LLVM/include-linux \
        $(LOCAL_PATH)/../../LLVM/include \
        $(LOCAL_PATH)/../../LLVM/lib/Target/X86 \
        $(LOCAL_PATH)/../../Renderer/ \
        $(LOCAL_PATH)/../../Common/ \
        $(LOCAL_PATH)/../../Shader/ \
        $(LOCAL_PATH)/../../Main/

include external/stlport/libstlport.mk

include $(BUILD_SHARED_LIBRARY)
