#ifndef ANDROID_COMMON
#define ANDROID_COMMON

namespace egl
{
class Image;
}

// Used internally
GLenum getColorFormatFromAndroid(int format);

// Used internally
GLenum getPixelFormatFromAndroid(int format);

// Used in V1 & V2 Context.cpp
GLenum isSupportedAndroidBuffer(GLuint name);

#endif  // ANDROID_COMMON
