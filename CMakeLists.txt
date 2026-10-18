cmake_minimum_required(VERSION 3.21)
project(CPUSchedulingSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(CPUSchedulingSimulator
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/scheduler.h
    src/scheduler.cpp
    src/ganttwidget.h
    src/ganttwidget.cpp
)

target_link_libraries(CPUSchedulingSimulator PRIVATE Qt6::Widgets)
target_include_directories(CPUSchedulingSimulator PRIVATE src)
