cmake_minimum_required(VERSION 3.16)

project(casdoor-cpp-qt-example LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

# casdoor-cpp-sdk is downloaded from GitHub. To build against a local checkout
# instead, pass -DFETCHCONTENT_SOURCE_DIR_CASDOOR=/path/to/casdoor-cpp-sdk.
include(FetchContent)
FetchContent_Declare(casdoor
    GIT_REPOSITORY https://github.com/casdoor/casdoor-cpp-sdk.git
    GIT_TAG master)
set(CASDOOR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CASDOOR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(casdoor)

qt_add_executable(casdoor-cpp-qt-example
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    config.h
    assert/assert.qrc)

set_target_properties(casdoor-cpp-qt-example PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON)

target_link_libraries(casdoor-cpp-qt-example PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    casdoor::casdoor)
