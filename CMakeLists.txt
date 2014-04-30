cmake_minimum_required (VERSION 2.8)
project (RemoteDetection)

set (path C:/Users/enes/Dst3rdParty_x64_vs11)

#opencv control  
set(OpenCV_DIR			"${path}/OpenCV" CACHE PATH "Path to OpenCV directory")
set(ZEROMQ_INCLUDE_DIR	"${path}/zeroMQ/include" CACHE PATH "Path to OpenCV directory")
set(ZEROMQ_LIB_DIR		"${path}/zeroMQ/lib" CACHE PATH "Path to OpenCV directory")
 
find_library(ZEROMQ_LIBS NAMES zmq libzmq-v110-mt-3_2_3 HINTS ${ZEROMQ_LIB_DIR})
 
set(OPENCV_COMPONENT_LIST opencv_core opencv_highgui)
find_package(OpenCV COMPONENTS ${OPENCV_COMPONENT_LIST} REQUIRED)

# add the binary tree to the search path for include files
include_directories(${PROJECT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_BINARY_DIR} ${ZEROMQ_INCLUDE_DIR})
  
# add the executable
add_executable (RemoteDetection 
		main.cpp
		RemoteDetection.cpp
		RemoteDetection.h  
		)

target_link_libraries (RemoteDetection 
		${OpenCV_LIBS}
		${ZEROMQ_LIBS}
)




