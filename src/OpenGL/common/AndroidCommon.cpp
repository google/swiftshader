#include "AndroidCommon.hpp"

#include "../../Common/DebugAndroid.hpp"

#include <GLES/glext.h>
#include <system/graphics.h>

#define GL_RGB565     0x8D62
#define SW_YV12_BT601 0x32315659   // YCrCb 4:2:0 Planar, 16-byte aligned, BT.601 color space, studio swing
#define SW_YV12_BT709 0x48315659   // YCrCb 4:2:0 Planar, 16-byte aligned, BT.709 color space, studio swing
#define SW_YV12_JFIF  0x4A315659   // YCrCb 4:2:0 Planar, 16-byte aligned, BT.601 color space, full swing

GLenum GLPixelFormatFromAndroid(int halFormat)
{
	switch(halFormat)
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
		return GL_RGB565;
	case HAL_PIXEL_FORMAT_YV12:
		return SW_YV12_BT601;
	case HAL_PIXEL_FORMAT_BLOB:
	case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
	default:
		ALOGE("%s badness unsupported HAL format=%x", __FUNCTION__, halFormat);
	}

	return GL_RGBA;
}

GLenum GLPixelTypeFromAndroid(int halFormat)
{
	switch(halFormat)
	{
	case HAL_PIXEL_FORMAT_RGBA_8888:
	case HAL_PIXEL_FORMAT_RGBX_8888:
	case HAL_PIXEL_FORMAT_RGB_888:
	case HAL_PIXEL_FORMAT_BGRA_8888:
		return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_RGB_565:
		return GL_UNSIGNED_SHORT_5_6_5;
	case HAL_PIXEL_FORMAT_YV12:
		return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_BLOB:
	case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
	default:
		ALOGE("%s badness unsupported HAL format=%x", __FUNCTION__, halFormat);
	}

	return GL_UNSIGNED_BYTE;
}
