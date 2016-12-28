cmake_minimum_required(VERSION 3.5) # TODO: 3.6+
project(curlyfile)

# libcurl configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CURL_STATICLIB ON CACHE BOOL "Build static lib")
set(BUILD_CURL_EXE OFF CACHE BOOL "Skip executable")
set(HTTP_ONLY ON CACHE BOOL "HTTP Only")
set(BUILD_TESTING OFF CACHE BOOL "Disable test projects")

include_directories(${CMAKE_JS_INC})

# Build libcurl
add_subdirectory(deps/curl)
target_compile_definitions(libcurl PUBLIC BUILDING_LIBCURL)
target_compile_definitions(libcurl PUBLIC CURL_MAX_WRITE_SIZE=5242880)

# Add its include dirs
include_directories(deps/curl/include)
include_directories(build/deps/curl/include/curl/)

# Project-specific
file(GLOB SOURCE_FILES "src/*.cc" "src/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

if (WIN32)
target_link_libraries(${PROJECT_NAME} libcurl ${CMAKE_JS_LIB})
else(WIN32)
target_link_libraries(${PROJECT_NAME} curl ${CMAKE_JS_LIB})
endif(WIN32)
