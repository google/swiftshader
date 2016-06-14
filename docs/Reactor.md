Reactor Documentation
=====================

Reactor is an embedded language for C++ to facilitate dynamic code generation and specialization.

Introduction
------------

To generate the code for an expression such as
```C++
float y = 1 - x;
```
using the LLVM compiler framework, one needs to execute
```C++
Value *valueY = BinaryOperator::CreateSub(ConstantInt::get(Type::getInt32Ty(Context), 1), valueX, "y", basicBlock);
```
For large expressions this quickly becomes hard to read, and tedious to write and modify.

With Reactor, it becomes as simple as writing
```C++
Float y = 1 - x;
```
Note the capital letter for the type. This is not the code to perform the calculation. It's the code that when executed will record the calculation to be performed.

This is possible through the use of C++ operator overloading. Reactor also supports control flow constructs and pointer arithmetic with C-like syntax, using some macros and templates.

Motivation
----------

Just-in-time compilation (JIT) is often regarded as inferior to static compilation, when it comes to performance. Its qualities for portability are readily recognised, but JIT has another quality that has the potential to make it faster than any static compilation: [run-time specialization](http://en.wikipedia.org/wiki/Run-time_algorithm_specialisation).

Specialization in general is the use of a more optimal routine that is specific for a certain set of conditions. For example when sorting two numbers it is faster to swap them if they are not yet in order, than to call the generic quicksort function on them. Specialization can be done statically, by explicitly writing each variant or by using metaprogramming to generate multiple variants at static compile time, or dynamically by examining the parameters at run-time and generating a specialized path.

Because specialization can be done statically, sometimes aided by metaprogramming, the ability of a JIT-compiler to do it at run-time is mostly considered useless. Specialized benchmarks show no advantage of JIT code over static code. However, having a specialized benchmark does not take into account that a typical real-world application deals with many unpredictable conditions. Systems can have one core or several dozen cores, and many different ISA extensions. This alone can make it impractical to write fully specialized routines manually, and with the help of metaprogramming it results in code bloat. Worse yet, any non-trivial application has a layered architecture, in which lower layers (e.g. framework APIs) know very little or nothing about the usage by higher layers. Various parameters also depend on user input. Run-time specialization can have access to the full context in which each routine executes, and although the optimization contribution of specialization for a single parameter is small, the combined speedup can be huge. As an extreme example, interpreters can execute any generic program, but by specializing for a specific program you get a compiled version of that program. But you don’t need a full-blown language to observe a huge difference between interpretation and specialization through compilation. Most applications process some form of list of commands in an interpreted fashion, and even the series of calls into a framework API can be compiled into a more efficient whole at run-time.

While the benefit of run-time specialization should now be apparent, JIT-compiled languages lack many of the practical advantages of static compilation. JIT-compilers are very constrained in how much time they can spend on compiling the bytecode into machine code. This limits their ability to even reach parity with static compilation, let alone attempt to exceed it by performing run-time specialization. Also, even if the compilation time was not as constrained, they can’t specialize at every opportunity because it would result in an explosive growth of the amount of generated code. There’s a need to be very selective in only specializing the hotspots for often recurring conditions, and to manage a cache of the different variants. Even just selecting the size of the set of variables that form the entire condition to specialize for can get immensely complicated. Ironically there are too many unknowns to make this feasible.

Clearly we need a manageable way to benefit from run-time specialization where it would help significantly, while still resorting to static compilation for anything else. A crucial observation is that the developer has expectations about the application’s behavior, which is valuable information which can be exploited to choose between static or JIT-compilation. One way to do that is to use an API which JIT-compiles the commands provided by the application developer. An example of this is an advanced DBMS which compiles the query into an optimized sequence of routines, each specialized to the data types involved, the sizes of the CPU caches, etc. Another example is a modern graphics API, which takes shaders (a routine executed per pixel or other element) and a set of parameters which affect their execution, and compiles them into GPU-specific code. However, these examples have a very hard divide between what goes on inside the API and outside. You can’t exchange data between the statically compiled outside world and the JIT-compiled routines, unless through the API, and they have very different execution models. In other words they are highly domain specific and not generic ways to exploit run-time specialization in arbitrary code.

This is becoming especially problematic for GPUs, as they are now just as programmable as CPUs but you can still only command them through an API. Attempts to disguise this by using a single language, such as C++AMP and SYCL, still have difficulties expressing how data is exchanged, don’t actually provide control over the specialization, they have hidden overhead, and unpredictable performance characteristics across devices. Meanwhile CPUs gain ever more cores and wider SIMD vector units, but statically compiled languages don’t readily exploit this and can’t deal with the many code paths required to extract optimal performance. A different language and framework is required.