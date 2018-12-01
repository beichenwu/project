cmake_minimum_required(VERSION 2.8)
project(a4-ece650)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
    add_subdirectory(minisat)
else()
    message (FATAL_ERROR "MiniSat not found. Use `git clone https://github.com/agurfinkel/minisat` to check it out.")
endif()


## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

# create the main executable
## add additional .cpp files if needed
add_executable(project project.cpp vsover.h vsolver.cpp graphplot.cpp)
add_executable(cupclock cpuclock.cpp)
add_executable(graphplot graphplot.cpp)
# link MiniSAT libraries
target_link_libraries(project minisat-lib-static -lpthread)
target_link_libraries(cupclock -lpthread)
target_link_libraries(graphplot -lboost_iostreams -lboost_system -lboost_filesystem)
# create the executable for tests
# link MiniSAT libraries
#target_link_libraries(test minisat-lib-static)
