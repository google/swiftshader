#ifndef sw_Routine_hpp
#define sw_Routine_hpp

namespace sw
{
	class RoutineManager;

	class Routine
	{
		friend class RoutineManager;

	public:
		Routine(int bufferSize);
		Routine(void *memory, int bufferSize, int offset);

		~Routine();

		void setFunctionSize(int functionSize);

		const void *getBuffer();
		const void *getEntry();
		int getBufferSize();
		int getFunctionSize();   // Includes constants before the entry point
		int getCodeSize();       // Executable code only
		bool isDynamic();

		void bind();
		void unbind();

	private:
		void *buffer;
		const void *entry;
		int bufferSize;
		int functionSize;

		volatile int bindCount;
		const bool dynamic;   // Generated or precompiled
	};
}

#endif   // sw_Routine_hpp
