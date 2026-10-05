cmake_minimum_required(VERSION 3.15)
project(WarrantyIQ VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(warrantyiq_core STATIC
    src/weibull_mle.cpp
    src/streaming_quantile.cpp
    src/failure_cluster_dsu.cpp
)

add_executable(test_runner tests/test_runner.cpp)
target_link_libraries(test_runner PRIVATE warrantyiq_core)
