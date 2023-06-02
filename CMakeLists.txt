cmake_minimum_required(VERSION 3.24)
project(labwork_8_notoriginallink VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# add the lib
add_subdirectory(lib/CCircularBuffer)

#add the executable
add_executable(
        Buffer
        main.cpp
)

target_link_libraries(Buffer PRIVATE CCircularBuffer)

#for tests
enable_testing()
add_subdirectory(tests)

