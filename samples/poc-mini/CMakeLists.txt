cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "$ENV{PSPDEV}/psp/share/pspdev.cmake" CACHE PATH "toolchain file")
project(me-stask-mini C CXX ASM)

add_executable(me-stask-mini
  main.cpp
)

target_compile_options(me-stask-mini PRIVATE -Os -G0 -Wall -fno-pic)

target_compile_options(me-stask-mini PRIVATE 
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_link_libraries(me-stask-mini
  me-core-lib me-core-mapper me-stask
  psppower pspge pspdisplay pspctrl pspdebug
  pspkubridge
)

create_pbp_file(TARGET me-stask-mini TITLE "Me Safe Task Mini Poc" BUILD_PRX)

