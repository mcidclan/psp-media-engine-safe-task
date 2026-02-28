cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "$ENV{PSPDEV}/psp/share/pspdev.cmake" CACHE PATH "toolchain file")
project(me-stask C ASM)

add_library(me-stask STATIC
  me-stask.c
  patch.S
)

target_compile_options(me-stask PRIVATE 
  -Os
  -G0
  -Wall 
  -fno-pic
)

target_include_directories(me-stask PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS me-stask
  ARCHIVE DESTINATION "$ENV{PSPDEV}/psp/lib"
)

file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
list(FILTER HEADER_FILES EXCLUDE REGEX ".*\\.def\\.h$")

install(FILES ${HEADER_FILES}
  DESTINATION "$ENV{PSPDEV}/psp/include/me-safe-task"
)
