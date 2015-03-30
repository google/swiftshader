set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Core
  IRReader
  NaClBitReader
  Support
  )

file(GLOB pnacl_sz_SRCS "src/*.cpp")
add_llvm_tool(pnacl-sz ${pnacl_sz_SRCS})

add_definitions(
  -DALLOW_DUMP=1
  -DALLOW_LLVM_CL=1
  -DALLOW_LLVM_IR=1
  -DALLOW_LLVM_IR_AS_INPUT=1
  -DALLOW_DISABLE_IR_GEN=1
  -DALLOW_MINIMAL_BUILD=0
  )

if(PNACL_BROWSER_TRANSLATOR)
  add_definitions(-DPNACL_BROWSER_TRANSLATOR=1)
else()
  add_definitions(-DPNACL_BROWSER_TRANSLATOR=0)
endif()

target_link_libraries(pnacl-sz pthread)
