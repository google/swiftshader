//TODO: copyrights

#ifndef __CPU_PROGRAM_H__
#define __CPU_PROGRAM_H__

#include "device_interface.h"

namespace llvm
{
	class ExecutionEngine;
	class Module;
}

namespace Devices
{

	class CPUDevice;
	class Program;

	/**
	* \brief CPU program
	*
	* This class implements the \c Coal::DeviceProgram interface for CPU
	* acceleration.
	*
	* It's main purpose is to initialize a \c llvm::JIT object to run LLVM bitcode,
	* in \c initJIT().
	*/
	class CPUProgram : public DeviceProgram
	{
	public:
		/**
		* \brief Constructor
		* \param device CPU device to which this program is attached
		* \param program \c Coal::Program that will be run
		*/
		CPUProgram(CPUDevice *device, Program *program);
		~CPUProgram();

		bool linkStdLib() const;
		void createOptimizationPasses(llvm::PassManager *manager, bool optimize);
		bool build(llvm::Module *module);

		/**
		* \brief Initialize an LLVM JIT
		*
		* This function creates a \c llvm::JIT object to run this program on
		* the CPU. A few implementation details :
		*
		* - The JIT is set not to resolve unknown symbols using \c dlsym().
		*   This way, a malicious kernel cannot execute arbitrary code on
		*   the host by declaring \c libc functions and calling them.
		* - All the unknown function names are passed to \c getBuiltin() to
		*   get native built-in implementations.
		*
		* \return true if success, false otherwise
		*/
		bool initJIT();
		llvm::ExecutionEngine *jit() const; /*!< \brief Current LLVM execution engine */

	private:
		CPUDevice *p_device;
		Program *p_program;

		llvm::ExecutionEngine *p_jit;
		llvm::Module *p_module;
	};

}

#endif
