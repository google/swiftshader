SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR="$( cd "${SRC_DIR}/.." >/dev/null 2>&1 && pwd )"
TESTS_DIR="$( cd "${ROOT_DIR}/tests" >/dev/null 2>&1 && pwd )"

CLANG_FORMAT=${CLANG_FORMAT:-clang-format}

for DIR in "${SRC_DIR}/Device" "${SRC_DIR}/Pipeline" "${SRC_DIR}/Reactor" "${SRC_DIR}/System" "${SRC_DIR}/Vulkan" "${SRC_DIR}/WSI" "${TESTS_DIR}"
do
    # Double clang-format, as it seems that one pass isn't always enough
    find ${DIR} -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.inl" | xargs ${CLANG_FORMAT} -i -style=file
    find ${DIR} -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.inl" | xargs ${CLANG_FORMAT} -i -style=file
done

