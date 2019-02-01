SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
pushd ${SRC_DIR}
for DIR in "Device" "Pipeline" "Reactor" "System" "Vulkan" "WSI"
do
    # Double clang-format, as it seems that one pass isn't always enough
    find ${SRC_DIR}/$DIR  -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.inl" | xargs clang-format -i
    find ${SRC_DIR}/$DIR  -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.inl" | xargs clang-format -i
done
popd

