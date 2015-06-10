#include "cl_gl_ext.h"
#include "debug.h"

extern CL_API_ENTRY cl_event CL_API_CALL
clCreateEventFromGLsyncKHR(cl_context           /* context */,
cl_GLsync            /* cl_GLsync */,
cl_int *             /* errcode_ret */) CL_EXT_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}