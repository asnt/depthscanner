set(DEPTHSENSESDK_DIR "/opt/softkinetic/DepthSenseSDK")

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${OPENCV_INCLUDE_DIR})
include_directories(${DEPTHSENSESDK_DIR}/include)
link_directories(${DEPTHSENSESDK_DIR}/lib)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
#set(CMAKE_LDFLAGS ${CMAKE_LDFLAGS} "-lDepthSense" "-lDepthSensePlugins")
add_executable(scanner main.cpp camera.cpp gl-window.cpp ds-image.cpp)

target_link_libraries(scanner ${CMAKE_THREADS_LIBS_INIT}
                              ${OPENGL_LIBRARIES}
                              ${OpenCV_LIBS}
                              DepthSense)
