cmake_minimum_required(VERSION 3.14)

project(qqq LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick  )


add_executable(qqq
  main.cpp
  utils.h
)

target_link_libraries(qqq Qt${QT_VERSION_MAJOR}::Quick )


install(TARGETS qqq
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


