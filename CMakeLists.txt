cmake_minimum_required(VERSION 3.15)
project(ParkingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subprojects
add_subdirectory(parking)
add_subdirectory(console)
