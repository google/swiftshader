#ifndef ANDROID_COMMON
#define ANDROID_COMMON

// Used internally
GLenum getColorFormatFromAndroid(int format);

// Used internally
GLenum getPixelFormatFromAndroid(int format);

// Used in V1 & V2 Context.cpp
GLenum isSupportedAndroidBuffer(GLuint name);

// Used in V1 & V2 Context.cpp
template <typename I> I* wrapAndroidNativeWindow(GLuint name)
{
    ANativeWindowBuffer *nativeBuffer = reinterpret_cast<ANativeWindowBuffer*>(name);
    ALOGV("%s: wrapping %p", __FUNCTION__, nativeBuffer);
    nativeBuffer->common.incRef(&nativeBuffer->common);

    GLenum format = getColorFormatFromAndroid(nativeBuffer->format);
    GLenum type = getPixelFormatFromAndroid(nativeBuffer->format);

    I *image = new I(0, nativeBuffer->width, nativeBuffer->height, format, type);
    image->setNativeBuffer(nativeBuffer);
    image->markShared();

    return image;
}

#endif  // ANDROID_COMMON
