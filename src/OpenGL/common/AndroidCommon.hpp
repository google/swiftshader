#ifndef ANDROID_COMMON
#define ANDROID_COMMON

#include <GLES/gl.h>

GLenum GLPixelFormatFromAndroid(int format);
GLenum GLPixelTypeFromAndroid(int format);

#endif  // ANDROID_COMMON
