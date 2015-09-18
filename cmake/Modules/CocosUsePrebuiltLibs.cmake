# CocosUsePrebuiltLibs - sets external libs variables to link with

if(_COCOS_USE_PREBUILT_LIBS_CMAKE)
    return()
endif()
set(_COCOS_USE_PREBUILT_LIBS_CMAKE TRUE)

# This file use some macros defined in Cocosbuildhelpers.cmake.
if(NOT _COCOS_BUILD_HELPERS_CMAKE)
    include(CocosBuildHelpers)
endif()

#===============================================================================
# Helper variable, indicated the <cocos>/external directory.
get_filename_component(external_dir 
    "${CMAKE_CURRENT_LIST_DIR}/../../external" ABSOLUTE
    )

if(COCOS_ARCH_FOLDER_SUFFIX)
    set(_suffix "/${COCOS_ARCH_FOLDER_SUFFIX}")
endif()
if(COCOS_TARGET_SYSTEM_MACOSX)
    set(prebuilt_dir "${external_dir}/mac${_suffix}")
elseif(COCOS_TARGET_SYSTEM_WINDOWS)
    set(prebuilt_dir "${external_dir}/win32${_suffix}")
elseif(COCOS_TARGET_SYSTEM_LINUX)
    set(prebuilt_dir "${external_dir}/linux${_suffix}")
elseif(COCOS_TARGET_SYSTEM_IOS)
    set(prebuilt_dir "${external_dir}/ios${_suffix}")
elseif(COCOS_TARGET_SYSTEM_ANDROID)
    set(prebuilt_dir "${external_dir}/android${_suffix}")
elseif(COCOS_TARGET_SYSTEM_WINSTORE)
    set(prebuilt_dir "${external_dir}/winrt_8.1${_suffix}")
endif()

include(CMakeParseArguments)

# Partial implement for cocos_load_prebuilt_package on win32 systems.
#
# Windows prebuilt package has two configrations, process sepratly.
# If DLL_NAMES are not defined or can't find DLL_NAMES files,
#   package treat as static lib, otherwise treat as shared lib.
# Usage ref to cocos_load_prebuilt_package.
function(_cocos_load_prebuilt_package_win32 pkg)
    set(multiValueArgs  LIB_NAMES)
    set(oneValueArgs    INCLUDE_PATH INSTALL_DIR DLL_NAMES)
    cmake_parse_arguments(_opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if(_opt_INSTALL_DIR)
        set(_dir ${_opt_INSTALL_DIR})
    else()
        set(_dir ${prebuilt_dir})
    endif()

    if(NOT _opt_LIB_NAMES)
        set(_opt_LIB_NAMES "${pkg}" "lib${pkg}")
    endif()

    string(TOUPPER ${pkg} _up_pkg)
    set(_target ${_up_pkg}::${_up_pkg})

    if(TARGET ${_target} OR ${_target}_FOUND})
        message(AUTHOR_WARNING "Already has a target named '${_target}'")
        return()
    endif()

    # if package has special include directory, check it exists or not.
    if(_opt_INCLUDE_PATH)
        find_file(${_up_pkg}_INCLUDE_DIR ${_opt_INCLUDE_PATH}
            HINTS ${_dir}
            PATH_SUFFIXES "include"
            NO_DEFAULT_PATH
            )
        if(NOT ${_up_pkg}_INCLUDE_DIR)
            message(AUTHOR_WARNING
                "Load prebuilt package error: can't find include path "
                "'${_opt_INCLUDE_PATH}' in '${_dir}/include'."
                )
            return()
        endif()
    endif()

    # Find library.
    find_library(${_up_pkg}_LIBRARY_RELEASE NAMES ${_opt_LIB_NAMES}
        HINTS ${_dir}
        PATH_SUFFIXES "libs/Release" 
        NO_DEFAULT_PATH
        )
    find_library(${_up_pkg}_LIBRARY_DEBUG NAMES ${_opt_LIB_NAMES}
        HINTS ${_dir}
        PATH_SUFFIXES "libs/Debug"
        NO_DEFAULT_PATH
        )
    if(NOT ${_up_pkg}_LIBRARY_RELEASE AND NOT ${_up_pkg}_LIBRARY_DEBUG})
        message(AUTHOR_WARNING
            "Load prebuilt package error: can't find lib "
            "named'${_opt_LIB_NAMES}' in '${_dir}/libs'."
            )
        return()
    endif()

    if(_opt_DLL_NAMES)
        # Find DLL file exists or not.
        foreach(_dll_name _opt_DLL_NAMES)
            find_file(${_up_pkg}_RUNTIME_RELEASE ${_opt_DLL_NAMES}
                HINTS ${_dir}
                PATH_SUFFIXES "libs/Release"
                NO_DEFAULT_PATH
                )
            if(${_up_pkg}_RUNTIME_RELEASE)
                break()
            endif()
        endforeach()
        foreach(_dll_name _opt_DLL_NAMES)
            find_file(${_up_pkg}_RUNTIME_DEBUG ${_opt_DLL_NAMES}
                HINTS ${_dir}
                PATH_SUFFIXES "libs/Debug"
                NO_DEFAULT_PATH
                )
            if(${_up_pkg}_RUNTIME_DEBUG)
                break()
            endif()
        endforeach()

        if(NOT ${_up_pkg}_RUNTIME_RELEASE AND NOT ${_up_pkg}_RUNTIME_DEBUG)
            # If no dll file found, treat as NOT input DLL_NAMES.
            unset(_opt_DLL_NAMES)
        endif()
    endif()

    if(_opt_DLL_NAMES)
        # Package is shared library.
        add_library(${_target} SHARED IMPORTED)
        if(${_up_pkg}_LIBRARY_RELEASE AND ${_up_pkg}_LIBRARY_DEBUG)
            set_target_properties(${_target} PROPERTIES
                IMPORTED_IMPLIB_RELEASE ${${_up_pkg}_LIBRARY_RELEASE}
                IMPORTED_IMPLIB_DEBUG ${${_up_pkg}_LIBRARY_DEBUG}
                )
        elseif(${_up_pkg}_LIBRARY_RELEASE)
            set_target_properties(${_target} PROPERTIES
                IMPORTED_IMPLIB ${${_up_pkg}_LIBRARY_RELEASE}
                )
        else()
            set_target_properties(${_target} PROPERTIES
                IMPORTED_IMPLIB ${${_up_pkg}_LIBRARY_DEBUG}
                )
        endif()

        if(${_up_pkg}_RUNTIME_RELEASE AND ${_up_pkg}_RUNTIME_DEBUG)
            set_target_properties(${_target} PROPERTIES
                IMPORTED_LOCATION_RELEASE ${${_up_pkg}_RUNTIME_RELEASE}
                IMPORTED_LOCATION_DEBUG ${${_up_pkg}_RUNTIME_DEBUG}
                )
        elseif(${_up_pkg}_RUNTIME_RELEASE)
            set_target_properties(${_target} PROPERTIES
                IMPORTED_LOCATION ${${_up_pkg}_RUNTIME_RELEASE}
                )
        else()
            set_target_properties(${_target} PROPERTIES
                IMPORTED_LOCATION ${${_up_pkg}_RUNTIME_DEBUG}
                )
        endif()

        mark_as_advanced(
            ${_up_pkg}_LIBRARY_RELEASE
            ${_up_pkg}_LIBRARY_DEBUG
            ${_up_pkg}_RUNTIME_RELEASE
            ${_up_pkg}_RUNTIME_DEBUG
            )
    else()
        # Package is static library.
        add_library(${_target} STATIC IMPORTED)

        if(${_up_pkg}_LIBRARY_RELEASE AND ${_up_pkg}_LIBRARY_DEBUG)
            set_target_properties(${_target} PROPERTIES
                IMPORTED_LOCATION_RELEASE ${${_up_pkg}_LIBRARY_RELEASE}
                IMPORTED_LOCATION_DEBUG ${${_up_pkg}_LIBRARY_DEBUG}
                )
        elseif(${_up_pkg}_LIBRARY_RELEASE)
            set_target_properties(${_target} PROPERTIES
                IMPORTED_LOCATION ${${_up_pkg}_LIBRARY_RELEASE}
                )
        else()
            set_target_properties(${_target} PROPERTIES
                IMPORTED_LOCATION ${${_up_pkg}_LIBRARY_DEBUG}
                )
        endif()
        mark_as_advanced(
            ${_up_pkg}_LIBRARY_RELEASE
            ${_up_pkg}_LIBRARY_DEBUG
            )
    endif()

    if(_opt_INCLUDE_PATH)
        set_target_properties(${_target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${${_up_pkg}_INCLUDE_DIR}
            )
        mark_as_advanced(${_up_pkg}_INCLUDE_DIR)
    endif()

    set(${_up_pkg}_FOUND TRUE CACHE INTERNAL "")
    message(STATUS "Found prebuilt package:'${pkg}', target '${_target}'.")
endfunction()

# Define a helper function for load a prebuilt package as target.
#
# Every package should keep the folder structure like cocos external dir.
# If function success, each prebuilt package will define:
# Target Name   : <UPPER_NAME>::<UPPER_NAME>
# Header Path   : <UPPER_NAME>_INCLUDE_DIR
# Lib Path      : <UPPER_NAME>_LIBRARY
# DLL Path      : <UPPER_NAME>_RUNTIME:  If is a dll library, has this variable.
# Usage:
#   cocos_load_prebuilt_package(
#       <pkg>               # the package name
#       <dir>               # the dir contain two subdirs: include and libs
#       [INCLUDE_PATH path] # the key header file to identifiy the include dir
#       [LIB_NAMES n1 ...]  # the posiable lib name of the package
#       [DLL_NAMES n1 ...]  # the possiable fullnames of dll, only win32 support
#       )
function(cocos_load_prebuilt_package pkg)
    if(COCOS_TARGET_SYSTEM_WINDOWS OR
            COCOS_TARGET_SYSTEM_WINSTORE OR
            COCOS_TARGET_SYSTEM_WINPHONE)
        _cocos_load_prebuilt_package_win32(${ARGV})
        return()
    endif()

    # deal with unix based system
    set(multiValueArgs  LIB_NAMES)
    set(oneValueArgs    INCLUDE_PATH INSTALL_DIR)
    cmake_parse_arguments(_opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if(_opt_INSTALL_DIR)
        set(_dir ${_opt_INSTALL_DIR})
    else()
        set(_dir ${prebuilt_dir})
    endif()

    if(NOT _opt_LIB_NAMES)
        set(_opt_LIB_NAMES "${pkg}" "lib${pkg}")
    endif()

    string(TOUPPER ${pkg} _up_pkg)
    set(_target ${_up_pkg}::${_up_pkg})

    if(TARGET ${_target} OR ${_target}_FOUND})
        message(AUTHOR_WARNING "Already has a target named '${_target}'")
        return()
    endif()

    # Android and iOS toolchain disabled these option, need open them.
    set(_old_mode_include ${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})
    set(_old_mode_library ${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY})
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)

    # Find include directory and library directory.
    if(_opt_INCLUDE_PATH)
        find_file(${_up_pkg}_INCLUDE_DIR ${_opt_INCLUDE_PATH}
            HINTS ${_dir}
            PATH_SUFFIXES "include"
            NO_DEFAULT_PATH
            )
    endif()
    find_library(${_up_pkg}_LIBRARY NAMES ${_opt_LIB_NAMES}
        HINTS ${_dir}
        PATH_SUFFIXES "libs"
        NO_DEFAULT_PATH
        )

    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ${_old_mode_library})
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ${_old_mode_include})

    # set include or lib directory not exists, return
    if(_opt_INCLUDE_PATH AND NOT ${_up_pkg}_INCLUDE_DIR)
        message(AUTHOR_WARNING
            "Load prebuilt package error: can't find include path "
            "'${_opt_INCLUDE_PATH}' in '${_dir}/include'."
            )
        return()
    endif()
    if(NOT ${_up_pkg}_LIBRARY)
        message(AUTHOR_WARNING
            "Load prebuilt package error: can't find lib "
            "named'${_opt_LIB_NAMES}' in '${_dir}/libs'."
            )
        return()
    endif()

    add_library(${_target} UNKNOWN IMPORTED)
    if(_opt_INCLUDE_PATH)
        set_target_properties(${_target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${${_up_pkg}_INCLUDE_DIR}
            )
        mark_as_advanced(${_up_pkg}_INCLUDE_DIR)
    endif()
    set_target_properties(${_target} PROPERTIES
        IMPORTED_LOCATION ${${_up_pkg}_LIBRARY}
    )
    mark_as_advanced(${_up_pkg}_LIBRARY)

    set(${_up_pkg}_FOUND TRUE CACHE INTERNAL "")
    message(STATUS "Found prebuilt package:'${pkg}', target '${_target}'.")
endfunction()

#===============================================================================
# Add prebuilt header path to include directory.
include_directories("${prebuilt_dir}/include")

#===============================================================================
cocos_load_prebuilt_package(box2d)
cocos_load_prebuilt_package(chipmunk)


if(NOT COCOS_TARGET_SYSTEM_WINSTORE AND NOT COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(cjson)
endif()

cocos_load_prebuilt_package(convertutf)

if(NOT COCOS_TARGET_SYSTEM_LINUX)
    cocos_load_prebuilt_package(curl
        LIB_NAMES "curl" "libcurl" "libcurl_imp"
        DLL_NAMES "libcurl.dll"
        )
    cocos_load_prebuilt_package(openssl
        LIB_NAMES "ssl" "libcurl" "libcurl_imp"
        DLL_NAMES "ssleay32.dll"
        )
    cocos_load_prebuilt_package(crypto
        LIB_NAMES "crypto" "libcurl" "libcurl_imp"
        DLL_NAMES "libeay32.dll"
        )
    message(STATUS "Prebuilt package 'CURL' depends: CRYPTO, OPENSSL.")
endif()

cocos_load_prebuilt_package(edtaa3func
    LIB_NAMES "edtaa3func" "libedtaa3func" "edtaa3"
    )

if(NOT COCOS_TARGET_SYSTEM_WINSTORE AND NOT COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(flatbufffers
        LIB_NAMES "flatbuffer" "flatbuffers" "libflatbuffers"
        )
endif()

if(COCOS_TARGET_SYSTEM_LINUX)
    cocos_load_prebuilt_package(fmodex
        INCLUDE_PATH "fmod"
        LIB_NAMES "fmodex64"
        )
    cocos_load_prebuilt_package(fmodexl
        INCLUDE_PATH "fmod"
        LIB_NAMES "fmodexL64"
        )
endif()

cocos_load_prebuilt_package(freetype
    INCLUDE_PATH "freetype"
    LIB_NAMES "freetype" "freetype250"
    )

if(COCOS_TARGET_SYSTEM_WINDOWS)
    cocos_load_prebuilt_package(glew
        LIB_NAMES "glew32"
        DLL_NAMES "glew32.dll"
        )
endif()

if(COCOS_TARGET_SYSTEM_WINSTORE OR COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(angle_egl
        INCLUDE_PATH "angle"
        LIB_NAMES "libEGL"
        DLL_NAMES "libELG.dll"
        )
    cocos_load_prebuilt_package(angle_gles
        LIB_NAMES "libGLESv2"
        DLL_NAMES "libGLESv2.dll"
        )
endif()

if(COCOS_TARGET_SYSTEM_MACOSX OR
        COCOS_TARGET_SYSTEM_WINDOWS OR
        COCOS_TARGET_SYSTEM_LINUX
        )
    cocos_load_prebuilt_package(glfw3
        LIB_NAMES "glfw" "glfw3"
        )
    if(COCOS_TARGET_SYSTEM_MACOSX)
        set_target_properties(GLFW3::GLFW3 PROPERTIES
            INTERFACE_LINK_LIBRARIES "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo"
            )
    elseif(COCOS_TARGET_SYSTEM_WINDOWS)
        find_package(OpenGL REQUIRED)
        set_target_properties(GLFW3::GLFW3 PROPERTIES
            INTERFACE_LINK_LIBRARIES "${OPENGL_gl_LIBRARY}"
            )
    endif()
endif()

cocos_load_prebuilt_package(jpeg)
#cocos_load_prebuilt_package(json) #This package is all herder file.

if(COCOS_TARGET_SYSTEM_IOS)
    cocos_load_prebuilt_package(lua
        INCLUDE_PATH "lua"
        )
endif()

if(NOT COCOS_TARGET_SYSTEM_WINSTORE AND NOT COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(luajit
        INCLUDE_PATH "luajit"
        LIB_NAMES "luajit" "lua51"
        DLL_NAMES "lua51.dll"
        )
    if(COCOS_TARGET_SYSTEM_MACOSX)
        set_target_properties(LUAJIT::LUAJIT PROPERTIES
            IMPORTED_LINK_INTERFACE_LIBRARIES "-pagezero_size 10000 -image_base 100000000"
            )
    else(COCOS_TARGET_SYSTEM_LINUX)
        set_target_properties(LUAJIT::LUAJIT PROPERTIES
            IMPORTED_LINK_INTERFACE_LIBRARIES "dl"
            )
    endif()

    cocos_load_prebuilt_package(luasocket)
endif()

cocos_load_prebuilt_package(minizip)

if(COCOS_TARGET_SYSTEM_WINDOWS)
    cocos_load_prebuilt_package(mpg123
        LIB_NAMES "libmpg123"
        DLL_NAMES "libmpg123.dll"
        )
    cocos_load_prebuilt_package(ogg
        LIB_NAMES "libogg"
        DLL_NAMES "libogg.dll"
        )
    cocos_load_prebuilt_package(vorbis
        LIB_NAMES "libvorbis"
        DLL_NAMES "libvorbis.dll"
        )
    cocos_load_prebuilt_package(vorbisfile
        LIB_NAMES "libvorbisfile"
        DLL_NAMES "libvorbisfile.dll"
        )
    cocos_load_prebuilt_package(OpenAL
        LIB_NAMES "OpenAL32"
        DLL_NAMES "OpenAL32.dll"
        )
endif()

cocos_load_prebuilt_package(png)
message(STATUS "Prebuilt package 'PNG' depends: ZLIB.")

if(NOT COCOS_TARGET_SYSTEM_WINSTORE AND NOT COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(spidermonkey
        INCLUDE_PATH "spidermonkey"
        LIB_NAMES "js_static" "mozjs-28"
        DLL_NAMES "mozjs-28.dll"
        )
endif()

if(COCOS_TARGET_SYSTEM_WINDOWS OR
        COCOS_TARGET_SYSTEM_WINSTORE OR
        COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(sqlite3
        LIB_NAMES "sqlite3"
        DLL_NAMES "sqlite3.dll"
        )
endif()

cocos_load_prebuilt_package(tiff
    DLL_NAMES "libtiff.dll"
    )
cocos_load_prebuilt_package(tinyxml2)

if(NOT COCOS_TARGET_SYSTEM_WINSTORE AND NOT COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(webp)
    if(COCOS_TARGET_SYSTEM_ANDROID)
        message(STATUS "Target '${_target}' depends: cpufeatures.")
    endif()
endif()

cocos_load_prebuilt_package(websockets
    DLL_NAMES "websockets.dll"
    )
cocos_load_prebuilt_package(xxhash)

if(NOT COCOS_TARGET_SYSTEM_WINSTORE AND NOT COCOS_TARGET_SYSTEM_WINPHONE)
    cocos_load_prebuilt_package(xxtea)
endif()

cocos_load_prebuilt_package(zlib
    INCLUDE_PATH "zlib"
    LIB_NAMES "z" "libzlib" "zlib"
    DLL_NAMES "zlib1.dll"
    )

