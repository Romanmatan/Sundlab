cmake_minimum_required(VERSION 3.15)
project(Soundlab
    VERSION 1.0.0
    DESCRIPTION "SoundLab - trovide an lab work for signal analysis"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt is installed into a non-standard prefix on some development machines.
if(EXISTS "C:/Qt/5.13.0/msvc2015_64")
    list(APPEND CMAKE_PREFIX_PATH "C:/Qt/5.13.0/msvc2015_64")
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Core Svg)

# Qwt (QwtPlot widgets used by the .ui files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Qwt REQUIRED)

# OpenAL (audio output)
include(FindPackageHandleStandardArgs)
find_path(OPENAL_INCLUDE_DIR
    NAMES AL/al.h
    HINTS "$ENV{OPENALDIR}" "${OPENALDIR}" "C:/Program Files (x86)/OpenAL"
    PATH_SUFFIXES include
    DOC "OpenAL include directory"
)
find_library(OPENAL_LIBRARY
    NAMES OpenAL32 openal32 openal
    HINTS "$ENV{OPENALDIR}" "${OPENALDIR}" "C:/Program Files (x86)/OpenAL"
    PATH_SUFFIXES lib
    DOC "OpenAL library"
)
find_package_handle_standard_args(OpenAL
    REQUIRED_VARS OPENAL_INCLUDE_DIR OPENAL_LIBRARY
)
add_executable(Soundlab WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    fftwindow.cpp
    fftwindow.h
    player.cpp
    player.h
    create_signal.cpp
    create_signal.h
    colorednoise.cpp
    colorednoise.h
    fft.c
    fft.h
    mainwindow.ui
    fftwindow.ui
    resources/resources.qrc
)

target_include_directories(Soundlab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QWT_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
)

target_link_libraries(Soundlab PRIVATE
    Qt5::Widgets
    Qt5::Gui
    Qt5::Core
    Qt5::Svg
    ${QWT_LIBRARIES}
    ${OPENAL_LIBRARY}
)

# CoInitialize / CoUninitialize (combaseapi.h) come from OLE32.
target_link_libraries(Soundlab PRIVATE ole32)

if(MSVC)
    target_compile_options(Soundlab PRIVATE /W3 /utf-8)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
endif()

set_target_properties(Soundlab PROPERTIES
    WIN32_EXECUTABLE TRUE
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Soundlab>"
)