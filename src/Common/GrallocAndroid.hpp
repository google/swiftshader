#ifndef GRALLOC_ANDROID
#define GRALLOC_ANDROID

#include <hardware/gralloc.h>

class GrallocModule
{
public:
	static GrallocModule *getInstance();
	int lock(buffer_handle_t handle, int usage, int left, int top, int width, int height, void **vaddr)
	{
		return m_module->lock(m_module, handle, usage, left, top, width, height, vaddr);
	}

	int unlock(buffer_handle_t handle)
	{
		return m_module->unlock(m_module, handle);
	}

private:
	GrallocModule();
	const gralloc_module_t *m_module;
};

#endif  // GRALLOC_ANDROID
