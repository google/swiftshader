cd $(dirname $0)
go run strip_unneeded.go --file=../CMakeLists.txt --test="cmake -DREACTOR_BACKEND=LLVM -DREACTOR_EMIT_DEBUG_INFO=1 .. && cmake --build ."