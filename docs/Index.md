SwiftShader Documentation
=========================

SwiftShader provides high-performance graphics rendering on the CPU. It eliminates the dependency on graphics hardware or its capabilities.

Architecture
------------

SwiftShader provides shared libraries (DLLs) which implement standardized graphics APIs. Applications already using these APIs thus don't require any changes to use SwiftShader. It can run entirely in user space, or as a driver (for Android), and output to either a frame buffer, window, or an offscreen buffer.

There are four major layers:

* API
* Renderer
* Reactor
* JIT

The API layer is an implementation of a graphics API, such as OpenGL (ES) or Direct3D, on top of the Renderer interface. It is responsible for managing API-level resources and rendering state, as well as compiling high-level shaders to bytecode form. 

The Renderer layer generates specialized processing routines for draw calls and coordinates the execution of rendering tasks. It defines the data structures used and how the processing is performed.

Reactor is an embedded language for C++ to dynamically generate code in a WYSIWYG fashion. It allows to specialize the processing routines for the state and shaders used by each draw call. Its syntax closely resembles C and shading languages, to make the code generation easily readable.

The JIT layer is a run-time compiler, such as LLVM. Reactor records its operations in an in-memory intermediate form which can be materialized by the JIT into a function which can be called directly.