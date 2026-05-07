cmake_minimum_required(VERSION 4.3.0)
project(sounds)
include(FetchContent)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O2")

find_package(OpenMP REQUIRED)

include_directories(include)

add_executable(${PROJECT_NAME}
    src/main.cpp)

target_link_libraries(${PROJECT_NAME}
    OpenMP::OpenMP_CXX)

