cmake_minimum_required(VERSION 2.8.9)
project(Panoramic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp
		src/DescriptorsManager.cpp
		include/DescriptorsManager.h
		src/PanoramaMaker.cpp
		include/PanoramaMaker.h src/main.cpp
        include/json/json.h config.json include/json/json-forwards.h
        src/json/jsoncpp.cpp
        src/mongoose.c include/mongoose.h
        )
find_package( OpenCV REQUIRED )
add_executable(Panoramic ${SOURCE_FILES})
target_link_libraries(Panoramic ${OpenCV_LIBS})

