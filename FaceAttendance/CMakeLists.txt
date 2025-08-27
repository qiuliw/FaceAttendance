cmake_minimum_required(VERSION 3.16)

project(FaceAttendance VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # 可执行文件 & DLL
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # 动态/静态库
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        faceattendance.cpp
        faceattendance.h
        faceattendance.ui
)

# ------------------- Qt 可执行程序 -------------------
# 现在直接使用 qt_add_executable，适合桌面平台
qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# SeetaFace2 库 (MSVC 生成的是 .lib)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

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
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENCV_DIR}/lib/opencv_world4120d.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceDetector.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceRecognizer.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceLandmarker.lib
    ${SEETA_FACE_DIR}/lib/SeetaFaceTracker.lib
    ${SEETA_FACE_DIR}/lib/SeetaQualityAssessor.lib
    ${SEETA_FACE_DIR}/lib/SeetaNet.lib
)

# ------------------- Qt 应用 Bundle 配置 -------------------
# # 添加头文件搜索路径
# include_directories(
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include # Qt UI 自动生成的头文件路径
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include_$<CONFIG>  # 自动匹配当前构建配置
# )


if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FaceAttendance)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
