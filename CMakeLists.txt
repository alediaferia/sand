project(cmsfun)

set(sources
  main.c
  mpc.c
  lval.cpp
)

set(headers
  parser.h
  lval.h
)

add_executable(cmsfun ${sources})
