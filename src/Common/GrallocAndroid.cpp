#include "GrallocAndroid.hpp"

#include <cutils/log.h>

GrallocModule *GrallocModule::getInstance()
{
    static GrallocModule instance;
    return &instance;
}

GrallocModule::GrallocModule()
{
    const hw_module_t *module = nullptr;
    hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module);

    if(!module)
    {
        ALOGE("Failed to load standard gralloc");
    }

    m_module = reinterpret_cast<const gralloc_module_t*>(module);
}
