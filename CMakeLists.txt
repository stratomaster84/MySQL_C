cmake_minimum_required(VERSION 3.5)

project(mysql)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

# Инструкции для включения MySQL (lib и include) в проект
set(MYSQL_DIR "E:/MAXSYS/mysql_5.7/server")
link_directories(${MYSQL_DIR}/lib)
include_directories(${MYSQL_DIR}/include)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(mysql
    main.cpp
    mainwindow.cpp
    mytextedit.cpp
    mainwindow.h
    mytextedit.h
    mainwindow.ui
)
# Включение библиотек QtWidgets и MySQL
target_link_libraries(${PROJECT_NAME} Qt5::Widgets MYSQL)
