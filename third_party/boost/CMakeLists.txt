# Copyright 2020 The SwiftShader Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Boost is downloaded if necessary

# From https://www.boost.org/users/download/
set(BOOST_VER 1.70.0)
set(BOOST_HASH_TARGZ "882b48708d211a5f48e60b0124cf5863c1534cd544ecd0664bb534a4b5d506e9")
set(BOOST_HASH_ZIP   "48f379b2e90dd1084429aae87d6bdbde9670139fa7569ee856c8c86dd366039d")

string(REPLACE "." "_" BOOST_VER_FNAME ${BOOST_VER})
set(BOOST_PARENT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(BOOST_DIR ${BOOST_PARENT_DIR}/boost_${BOOST_VER_FNAME})

function(DownloadBoost)
    if (LINUX)
        set(BOOST_EXT "tar.gz")
        set(BOOST_HASH ${BOOST_HASH_TARGZ})
    else()
        set(BOOST_EXT "zip")
        set(BOOST_HASH ${BOOST_HASH_ZIP})
    endif()

    # Note: bintray.com has rate limiting, so use the sourceforge mirror
    # set(BOOST_URL https://dl.bintray.com/boostorg/release/${BOOST_VER}/source/boost_${BOOST_VER_FNAME}.${BOOST_EXT})
    set(BOOST_URL https://iweb.dl.sourceforge.net/project/boost/boost/${BOOST_VER}/boost_${BOOST_VER_FNAME}.${BOOST_EXT})

    if (NOT TARGET Boost::boost)
        if(NOT EXISTS ${BOOST_DIR})
            message(WARNING "
          ${BOOST_DIR} is missing.
          Downloading and extracting boost:
            ")

            set(BOOST_ARCHIVE ${CMAKE_BINARY_DIR}/temp/boost_archive)
            message(STATUS "Downloading ${BOOST_URL} to ${BOOST_ARCHIVE}...")
            file(DOWNLOAD ${BOOST_URL} ${BOOST_ARCHIVE} EXPECTED_HASH SHA256=${BOOST_HASH})
            message(STATUS "Extracting ${BOOST_ARCHIVE} to ${BOOST_DIR}...")
            execute_process(
                WORKING_DIRECTORY ${BOOST_PARENT_DIR}
                COMMAND cmake -E tar xf ${BOOST_ARCHIVE}
            )
        endif()
    endif()
endfunction()

DownloadBoost()

set(BOOST_INCLUDEDIR ${BOOST_DIR})
find_package(Boost REQUIRED)

# Expose Boost::boost target for parent project
add_library(boost INTERFACE)
target_link_libraries(boost INTERFACE
    Boost::boost
)
add_library(Boost::boost ALIAS boost)
