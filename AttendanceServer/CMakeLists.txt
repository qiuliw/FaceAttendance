cmake_minimum_required(VERSION 3.16)

project(AttendanceServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql)

set(PROJECT_SOURCES
        main.cpp
        attendancewin.cpp
        attendancewin.h
        attendancewin.ui
        FaceEngine.h
        QFaceObject.cpp
        QFaceObject.h
)

# ------------------- Qt6 bundle -------------------

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AttendanceServer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AttendanceServer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AttendanceServer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AttendanceServer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# 添加头文件搜索路径
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include # Qt UI 自动生成的头文件路径
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include_$<CONFIG>  # 自动匹配当前构建配置
)

# 链接 Qt 库
target_link_libraries(AttendanceServer PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AttendanceServer)
endif()
set_target_properties(AttendanceServer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS AttendanceServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AttendanceServer)
endif()

# ------------------- SeetaFace2 & OpenCV 配置 -------------------
# 头文件目录
include_directories(
    D:/Software/SeetaFace2/build/install/include
    D:/Software/opencv/build/include
)

set(SEETA_FACE_DIR D:/Software/SeetaFace2/build/install)
set(OPENCV_DIR D:/Software/opencv/build/x64/vc16)

# 库文件目录
link_directories(
    ${SEETA_FACE_DIR}/lib
    ${OPENCV_DIR}/lib
)

# SeetaFace2 库 (MSVC 生成的是 .lib)    MSBUILD不支持模糊匹配，只能精确匹配
target_link_libraries(${PROJECT_NAME} PRIVATE # target_link 需要 target  lib或者exe
    ${OPENCV_DIR}/lib/opencv_world4120d.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceDetector.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceRecognizer.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceLandmarker.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceTracker.lib
    ${SEETA_FACE_DIR}/lib/SeetaQualityAssessor.lib
    ${SEETA_FACE_DIR}/lib/SeetaNet.lib
)