cmake_minimum_required(VERSION 3.11)

project(Radio CXX)
set(CMAKE_CXX_STANDARD 20)

# Ищем Boost версии 1.78
find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Под Windows нужно определить макрос NOMINMAX для корректной работы при включении
# библиотеки minisound
if(WIN32)
  add_definitions(-DNOMINMAX)
endif()

include_directories(${CMAKE_INCLUDE_PATH})
add_executable(radio src/main.cpp src/audio.hpp)
target_link_libraries(radio PRIVATE Threads::Threads)
