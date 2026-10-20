# Builds Hamlib 4.7.2 (upstream/, unchanged) as libhamlib.so for Android with the NDK.
# Settings: android/include/hamlib/config.h (from Hamlib's configure); file list:
# android/sources.cmake (from Hamlib's Makefiles). Hamlib is LGPL-2.1-or-later (upstream/COPYING.LIB).
cmake_minimum_required(VERSION 3.18)
project(hamlib C CXX)

include(${CMAKE_CURRENT_LIST_DIR}/android/sources.cmake)
set(U ${CMAKE_CURRENT_LIST_DIR}/upstream)
# Hamlib's Makefiles compile each backend with its own folder on the include path
# (rigs/yaesu/ftx1/*.c include "yaesu.h"); give each file only its own backend folder,
# since several backends have headers of the same name.
foreach(s ${HAMLIB_SOURCES})
    string(REGEX MATCH "^(rigs|rotators|amplifiers)/[^/]+" backend "${s}")
    if(backend)
        set_property(SOURCE ${U}/${s} APPEND PROPERTY INCLUDE_DIRECTORIES ${U}/${backend})
    endif()
endforeach()
list(TRANSFORM HAMLIB_SOURCES PREPEND ${U}/)

add_library(hamlib SHARED ${HAMLIB_SOURCES})
target_compile_definitions(hamlib PRIVATE HAVE_CONFIG_H IN_HAMLIB _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_include_directories(hamlib
    PUBLIC ${U}/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/android/include/hamlib
            ${CMAKE_CURRENT_LIST_DIR}/android/include
            ${U}/src ${U}/lib ${U}/include/hamlib)
target_compile_options(hamlib PRIVATE -O2 -w)
target_link_libraries(hamlib PRIVATE m android log)
# glob() exists on Android 9+ only; the app supports Android 8 (see the header).
set_source_files_properties(${U}/src/microham.c PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_LIST_DIR}/android/include/android_glob_compat.h")
