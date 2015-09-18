#===============================================================================
# Add include directories.
if(COCOS_TARGET_SYSTEM_MACOSX OR
        COCOS_TARGET_SYSTEM_WINDOWS OR
        COCOS_TARGET_SYSTEM_LINUX)
    target_include_directories(${COCOS2D_TARGET} PRIVATE "platform/desktop")
endif()

#===============================================================================
# Common sources.
cocos_target_sources(${COCOS2D_TARGET} GROUP "platform" FILES
    ${CMAKE_CURRENT_LIST_FILE}
    "platform/CCSAXParser.cpp"
    "platform/CCThread.cpp"
    "platform/CCGLView.cpp"
    "platform/CCFileUtils.cpp"
    "platform/CCImage.cpp"
    )

#===============================================================================
# System specific sources.
if(APPLE)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\apple" FILES
        "platform/apple/CCLock-apple.cpp"
        "platform/apple/CCFileUtils-apple.mm"
        "platform/apple/CCThread-apple.mm"
    )
endif()

#if(COCOS_TARGET_SYSTEM_WINSTORE AND COCOS_TARGET_SYSTEM_WINPHONE)
#    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\universal" FILES
#        )
#endif()

if(COCOS_TARGET_SYSTEM_MACOSX)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\mac" FILES
        "platform/mac/CCApplication-mac.mm"
        "platform/mac/CCCommon-mac.mm"
        "platform/mac/CCDevice-mac.mm"
        "platform/desktop/CCGLViewImpl-desktop.cpp"
    )
elseif(COCOS_TARGET_SYSTEM_WINDOWS)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\win32" FILES
        "platform/win32/CCStdC-win32.cpp"
        "platform/win32/CCFileUtils-win32.cpp"
        "platform/win32/CCCommon-win32.cpp"
        "platform/win32/CCApplication-win32.cpp"
        "platform/win32/CCDevice-win32.cpp"
        "platform/desktop/CCGLViewImpl-desktop.cpp"
    )
elseif(COCOS_TARGET_SYSTEM_LINUX)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\linux" FILES
        "platform/linux/CCStdC-linux.cpp"
        "platform/linux/CCFileUtils-linux.cpp"
        "platform/linux/CCCommon-linux.cpp"
        "platform/linux/CCApplication-linux.cpp"
        "platform/linux/CCDevice-linux.cpp"
        "platform/desktop/CCGLViewImpl-desktop.cpp"
    )
elseif(COCOS_TARGET_SYSTEM_IOS)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\ios" FILES
        "platform/ios/CCApplication-ios.h"
        "platform/ios/CCApplication-ios.mm"
        "platform/ios/CCCommon-ios.mm"
        "platform/ios/CCDevice-ios.mm"
        "platform/ios/CCDirectorCaller-ios.h"
        "platform/ios/CCDirectorCaller-ios.mm"
        "platform/ios/CCEAGLView-ios.h"
        "platform/ios/CCEAGLView-ios.mm"
        "platform/ios/CCES2Renderer-ios.h"
        "platform/ios/CCES2Renderer-ios.m"
        "platform/ios/CCESRenderer-ios.h"
        "platform/ios/CCGL-ios.h"
        "platform/ios/CCGLViewImpl-ios.h"
        "platform/ios/CCGLViewImpl-ios.mm"
        "platform/ios/CCImage-ios.mm"
        "platform/ios/CCPlatformDefine-ios.h"
        "platform/ios/CCStdC-ios.h"
        "platform/ios/OpenGL_Internal-ios.h"
    )
elseif(COCOS_TARGET_SYSTEM_ANDROID)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\android" FILES
        "platform/android/CCApplication-android.cpp"
        "platform/android/CCCommon-android.cpp"
        "platform/android/CCDevice-android.cpp"
        "platform/android/CCGLViewImpl-android.cpp"
        "platform/android/CCFileUtils-android.cpp"
        "platform/android/javaactivity-android.cpp"
        )
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\android\\jni" FILES
        "platform/android/jni/CocosPlayClient.cpp"
        "platform/android/jni/DPIJni.cpp"
        "platform/android/jni/IMEJni.cpp"
        "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxAccelerometer.cpp"
        "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxBitmap.cpp"
        "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.cpp"
        "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxRenderer.cpp"
        "platform/android/jni/JniHelper.cpp"
        "platform/android/jni/TouchesJni.cpp"
    )
elseif(COCOS_TARGET_SYSTEM_WINSTORE)
    cocos_target_sources(${COCOS2D_TARGET} GROUP "platform\\winrt" FILES
        "platform/winrt/shaders/precompiledshaders.h"
        "platform/winrt/CCApplication.cpp"
        "platform/winrt/CCApplication.h"
        "platform/winrt/CCCommon.cpp"
        "platform/winrt/CCDevice.cpp"
        "platform/winrt/CCFileUtilsWinRT.cpp"
        "platform/winrt/CCFileUtilsWinRT.h"
        "platform/winrt/CCFreeTypeFont.cpp"
        "platform/winrt/CCFreeTypeFont.h"
        "platform/winrt/CCGL.h"
        "platform/winrt/CCGLViewImpl-winrt.cpp"
        "platform/winrt/CCGLViewImpl-winrt.h"
        #"platform/winrt/CCGLViewImpl.cpp"
        "platform/winrt/CCGLViewImpl.h"
        "platform/winrt/CCGL_Angle.h"
        "platform/winrt/CCPThreadWinRT.cpp"
        "platform/winrt/CCPThreadWinRT.h"
        "platform/winrt/CCPlatformDefine-winrt.h"
        "platform/winrt/CCStdC.cpp"
        "platform/winrt/CCStdC.h"
        "platform/winrt/CCWinRTUtils.cpp"
        "platform/winrt/CCWinRTUtils.h"
        "platform/winrt/DirectXBase.cpp"
        "platform/winrt/DirectXBase.h"
        "platform/winrt/DirectXHelper.h"
        "platform/winrt/InputEvent.cpp"
        "platform/winrt/InputEvent.h"
        "platform/winrt/InputEventTypes.h"
        "platform/winrt/Keyboard-winrt.cpp"
        "platform/winrt/Keyboard-winrt.h"
        "platform/winrt/inet_ntop_winrt.cpp"
        "platform/winrt/inet_ntop_winrt.h"
        "platform/winrt/inet_pton_winrt.cpp"
        "platform/winrt/inet_pton_winrt.h"
        "platform/winrt/pch.cpp"
        "platform/winrt/pch.h"
        "platform/winrt/sha1.cpp"
        "platform/winrt/sha1.h"
        "platform/winrt/shaders"
        "platform/winrt/targetver.h"
        )
else()
    message(AUTHOR_WARNING
        "${CMAKE_CURRENT_LIST_FILE} build on unsupport system."
        )
endif()

# CCGLViewImpl-ios.mm depends on CCSet.h yet, need remove sometime.
if(COCOS_TARGET_SYSTEM_IOS)
    target_include_directories(${COCOS2D_TARGET} PRIVATE
        "${COCOS2D_SOURCE_DIR}/deprecated"
        )
endif()

# DPIJni.cpp include "jni/JniHelper.h" yet, need change path sometime.
if(COCOS_TARGET_SYSTEM_ANDROID)
    target_include_directories(${COCOS2D_TARGET} PRIVATE
        "platform/android"
        )
endif()

if(COCOS_TARGET_SYSTEM_WINSTORE)
    # FIXME: this file compile failed if not FI cocos2d.h
    set_source_files_properties(
        "platform/winrt/InputEvent.cpp"
        "platform/winrt/keyboard-winrt.cpp"
        PROPERTIES COMPILE_FLAGS
        /FI"cocos2d.h"
        )
endif()
