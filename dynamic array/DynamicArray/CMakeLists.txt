cmake_minimum_required(VERSION 3.15)
project(DynamicArray)

set(CMAKE_CXX_STANDARD 20)

# Подключаем Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/065127f1e4b46c5f14fc73cf8d323c221f9dc68e.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# Основной исполняемый файл (можно тестовый, если main.cpp содержит тесты)
add_executable(DynamicArrayExec main.cpp DynamicArray.h)

# Линкуем GTest, если планируем использовать тесты
target_link_libraries(DynamicArrayExec PUBLIC GTest::gtest GTest::gtest_main)

# Включаем папку с заголовками
target_include_directories(DynamicArrayExec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})