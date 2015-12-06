cmake_minimum_required(VERSION 3.1.0)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

project(cmsfun)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wc++11-extensions")

file(GLOB SRCS
  "*.cpp"
)

file(GLOB HDRS
  "*.h"
  )


add_executable(cmsfun ${SRCS} ${HDRS})
