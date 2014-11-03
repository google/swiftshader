#ifndef egl_Texture2D_hpp
#define egl_Texture2D_hpp

namespace egl
{
class Texture2D
{
public:
	virtual void releaseTexImage() = 0;
};
}

#endif   // egl_Texture2D_hpp
