cmake_minimum_required(VERSION 3.20)
project(FPLinker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Network)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/AppCard.cpp
    src/MarketCard.cpp
    src/FlatpakManager.cpp
)

set(HEADERS
    src/MainWindow.h
    src/AppCard.h
    src/MarketCard.h
    src/FlatpakManager.h
)

add_executable(FPLinker ${SOURCES} ${HEADERS})

target_link_libraries(FPLinker PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Network
)

target_include_directories(FPLinker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

install(TARGETS FPLinker DESTINATION bin)
