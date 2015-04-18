#include <system/window.h>
#include "GL/glcorearb.h"
#include "GL/glext.h"
#include "EGL/egl.h"

#define GL_RGB565_OES                     0x8D62

#include "AndroidCommon.hpp"

#include "../../Common/DebugAndroid.hpp"
#include "../../Common/GrallocAndroid.hpp"

GLenum getColorFormatFromAndroid(int format)
{
    switch(format)
    {
        case HAL_PIXEL_FORMAT_RGBA_8888:
            return GL_RGBA;
        case HAL_PIXEL_FORMAT_RGBX_8888:
            return GL_RGB;
        case HAL_PIXEL_FORMAT_RGB_888:
            return GL_RGB;
        case HAL_PIXEL_FORMAT_BGRA_8888:
            return GL_BGRA_EXT;
        case HAL_PIXEL_FORMAT_RGB_565:
#if LATER
            if (GrallocModule::getInstance()->supportsConversion()) {
                return GL_RGB565_OES;
            } else {
                UNIMPLEMENTED();
                return GL_RGB565_OES;
            }
#else
            return GL_RGB565_OES;
#endif
        case HAL_PIXEL_FORMAT_YV12:
        case HAL_PIXEL_FORMAT_Y8:
        case HAL_PIXEL_FORMAT_Y16:
        case HAL_PIXEL_FORMAT_RAW_SENSOR:
        case HAL_PIXEL_FORMAT_BLOB:
        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        default:
            UNIMPLEMENTED();
    }
    return GL_RGBA;
}

// Used internally
GLenum getPixelFormatFromAndroid(int format)
{
    switch(format)
    {
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
        case HAL_PIXEL_FORMAT_RGB_888:
        case HAL_PIXEL_FORMAT_BGRA_8888:
            return GL_UNSIGNED_BYTE;
        case HAL_PIXEL_FORMAT_RGB_565:
#if LATER
            if (GrallocModule::getInstance()->supportsConversion()) {
                return GL_UNSIGNED_SHORT_5_6_5;
            } else {
                UNIMPLEMENTED();
                return GL_UNSIGNED_SHORT_5_6_5;
            }
#else
            return GL_UNSIGNED_SHORT_5_6_5;
#endif
        case HAL_PIXEL_FORMAT_YV12:
        case HAL_PIXEL_FORMAT_Y8:
        case HAL_PIXEL_FORMAT_Y16:
        case HAL_PIXEL_FORMAT_RAW_SENSOR:
        case HAL_PIXEL_FORMAT_BLOB:
        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        default:
            UNIMPLEMENTED();
    }
    return GL_UNSIGNED_BYTE;
}

// Used in V1 & V2 Context.cpp
GLenum isSupportedAndroidBuffer(GLuint name)
{
    ANativeWindowBuffer *nativeBuffer = reinterpret_cast<ANativeWindowBuffer*>(name);

    if(!name)
    {
        ALOGE("%s called with name==NULL %s:%d", __FUNCTION__, __FILE__, __LINE__);
        return EGL_BAD_PARAMETER;
    }
    if(nativeBuffer->common.magic != ANDROID_NATIVE_BUFFER_MAGIC)
    {
        ALOGE("%s: failed: bad magic", __FUNCTION__);
        return EGL_BAD_PARAMETER;
    }

    if(nativeBuffer->common.version != sizeof(ANativeWindowBuffer))
    {
        ALOGE("%s: failed: bad size", __FUNCTION__ );
        return EGL_BAD_PARAMETER;
    }

    switch(nativeBuffer->format)
    {
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
            return EGL_SUCCESS;
        case HAL_PIXEL_FORMAT_RGB_565:
#if LATER
            if (GrallocModule::getInstance()->supportsConversion()) {
                return EGL_SUCCESS;
            } else {
                ALOGE("%s: failed: bad format", __FUNCTION__ );
                return EGL_BAD_PARAMETER;
            }
#else
            return EGL_SUCCESS;
#endif
        default:
            ALOGE("%s: failed: bad format", __FUNCTION__ );
            return EGL_BAD_PARAMETER;
    }
}
