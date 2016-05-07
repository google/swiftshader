#ifndef sw_MatrixStack_hpp
#define sw_MatrixStack_hpp

#include "Renderer/Matrix.hpp"

namespace sw
{
	class MatrixStack
	{
	public:
		MatrixStack(int size = 2);

		~MatrixStack();

		void identity();
		void load(const Matrix &M);
		void load(const float *M);
		void load(const double *M);

		void translate(float x, float y, float z);
		void translate(double x, double y, double z);
		void rotate(float angle, float x, float y, float z);
		void rotate(double angle, double x, double y, double z);
		void scale(float x, float y, float z);
		void scale(double x, double y, double z);
		void multiply(const float *M);
		void multiply(const double *M);

		void frustum(float left, float right, float bottom, float top, float zNear, float zFar);
		void ortho(double left, double right, double bottom, double top, double zNear, double zFar);

		bool push();   // False on overflow
		bool pop();    // False on underflow

		const Matrix &current();
		bool isIdentity() const;

	private:
		int top;
		int size;
		Matrix *stack;
	};
}

#endif   // sw_MatrixStack_hpp
