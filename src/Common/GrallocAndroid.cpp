#include "GrallocAndroid.hpp"

#include <cutils/log.h>

GrallocModule* GrallocModule::getInstance()
{
    static GrallocModule instance;
    return &instance;
}

GrallocModule::GrallocModule()
{
    const hw_module_t* module;
    hw_get_module("converting_gralloc", &module);
    if (module)
    {
        m_supportsConversion = true;
        ALOGI("Loaded converting gralloc");
    }
    else
    {
        m_supportsConversion = false;
        ALOGE("Falling back to standard gralloc with reduced format support");
        hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module);
    }
    if (!module)
    {
        ALOGE("Failed to load standard gralloc");
    }
    m_module = reinterpret_cast<const gralloc_module_t*>(module);
}
