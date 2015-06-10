#include "context.h"

namespace cl
{
	struct Current
	{
		Devices::Context* context;
		cl_platform_id platform;
	};
	static Current *getCurrent(void);
	void makeCurrent(cl_platform_id platformId, Devices::Context *context);
	Devices::Context *getContext();
}