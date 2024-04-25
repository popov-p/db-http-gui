cmake_minimum_required(VERSION 3.5)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(glog REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/connect-widget.cpp
        src/connect-widget.h
        src/main-widget.cpp
        src/main-widget.h
        src/settings-dialog.cpp
        src/settings-dialog.h
        src/add-element-dialog.cpp
        src/add-element-dialog.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(client
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(client
                      PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                      glog::glog
                      Boost::program_options
                      Qt6::Network
)

set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(client)
endif()
