project(roc_curves)
cmake_minimum_required(VERSION 2.8)

# paths
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)
LINK_DIRECTORIES(/usr/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-o3 -w -std=c++11")
SET(CMAKE_CXX_LINK_FLAGS "-pg")
SET(OpenCV_LIBRARIES opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc)
SET(Header_Files
                        src/headers/roc_analizer.h
                        src/headers/Ifeature.h
                        src/headers/dummyFeature.h
                        src/headers/Bayes.h
                        src/headers/graphview.h)
SET (Sources
                        src/roc_analizer.cpp
                        src/dummyFeature.cpp
                        ${Header_Files})

ADD_EXECUTABLE(roc_curves src/main.cpp ${Sources})
TARGET_LINK_LIBRARIES(roc_curves ${OpenCV_LIBRARIES})



SET(CMAKE_BUILD_TYPE Debug)

