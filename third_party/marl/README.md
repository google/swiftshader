# Marl

Marl is a hybrid thread / fiber task scheduler written in C++ 11.

## About

Marl is a C++ 11 library that provides a fluent interface for running tasks across a number of threads.

Marl uses a combination of fibers and threads to allow efficient execution of tasks that can block, while keeping a fixed number of hardware threads.

Marl supports Windows, macOS, Linux, Fuchsia and Android (arm, aarch64, ppc64 (ELFv2), x86 and x64).

Marl has no dependencies on other libraries (with exception on googletest fo building the optional unit tests).

Marl is in early development and will have breaking API changes.


**More documentation and examples coming soon.**


Note: This is not an officially supported Google product
