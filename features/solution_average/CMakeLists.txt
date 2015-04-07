project(solution_average)
cmake_minimum_required(VERSION 2.8)

# paths
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)
LINK_DIRECTORIES(/usr/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-o3 -w")
SET(CMAKE_CXX_LINK_FLAGS "-pg")
SET(OpenCV_LIBRARIES opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc)

ADD_EXECUTABLE(solution_average src/main.cpp)
TARGET_LINK_LIBRARIES(solution_average ${OpenCV_LIBRARIES})


SET(CMAKE_BUILD_TYPE Debug)

