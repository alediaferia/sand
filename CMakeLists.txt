cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wc++11-extensions")
project(cmsfun)

set(sources
  main.cpp
  mpc.c
  lval.cpp
  parser.cpp
  error.cpp
  console.cpp
  parser.cpp
)

add_executable(cmsfun ${sources})
