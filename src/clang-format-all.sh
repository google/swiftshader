SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

CLANG_FORMAT=${CLANG_FORMAT:-clang-format}

for DIR in "Device" "Pipeline" "Reactor" "System" "Vulkan" "WSI"
do
    # Double clang-format, as it seems that one pass isn't always enough
    find ${SRC_DIR}/${DIR} -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.inl" | xargs ${CLANG_FORMAT} -i -style=file
    find ${SRC_DIR}/${DIR} -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.inl" | xargs ${CLANG_FORMAT} -i -style=file
done

