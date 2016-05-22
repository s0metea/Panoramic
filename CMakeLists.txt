cmake_minimum_required(VERSION 2.8.9)
project(Panoramic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp
		DescriptorsManager.cpp
		DescriptorsManager.h
		PanoramaMaker.cpp
		PanoramaMaker.h main.cpp)
find_package( OpenCV REQUIRED )
add_executable(Panoramic ${SOURCE_FILES})
target_link_libraries(Panoramic ${OpenCV_LIBS} -lmongoose -lpthread)

