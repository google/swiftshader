#ifndef egl_Context_hpp
#define egl_Context_hpp

namespace egl
{
class Surface;

class Context
{
public:
	virtual void destroy() = 0;
	virtual void bindTexImage(Surface *surface) = 0;
};
}

#endif   // egl_Context_hpp
