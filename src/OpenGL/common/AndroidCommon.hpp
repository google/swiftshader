#ifndef ANDROID_COMMON
#define ANDROID_COMMON

static inline GLenum getColorFormatFromAndroid(int format)
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
            return GL_RGB565_OES;
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

static inline GLenum getPixelFormatFromAndroid(int format)
{
    switch(format)
    {
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
        case HAL_PIXEL_FORMAT_RGB_888:
        case HAL_PIXEL_FORMAT_BGRA_8888:
            return GL_UNSIGNED_BYTE;
        case HAL_PIXEL_FORMAT_RGB_565:
            return GL_UNSIGNED_SHORT_5_6_5;
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
#endif  // ANDROID_COMMON
