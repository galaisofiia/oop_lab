cmake_minimum_required(VERSION 3.16)
project(oop_lab LANGUAGES CXX)

# настройка стандарта С++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# включение автоматической обработки QT макросов
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql)

add_executable(oop_lab
    main.cpp
    db/database.cpp
    scene/custom_scene.cpp
    scene/shape_item.cpp
    ui/mainwindow.cpp
    delegates/shape_delegate.cpp
)

target_include_directories(oop_lab PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(oop_lab PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql)