cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "$ENV{PSPDEV}/psp/share/pspdev.cmake" CACHE PATH "toolchain file")
project(me-drawing-demo C CXX ASM)

add_executable(me-drawing-demo
  main.cpp
)

target_compile_options(me-drawing-demo PRIVATE -Os -G0 -Wall -fno-pic)

target_compile_options(me-drawing-demo PRIVATE 
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_link_libraries(me-drawing-demo
  me-core-mapper
  -Wl,--start-group me-core-lib me-stask -Wl,--end-group
  psppower pspge pspdisplay pspctrl pspdebug pspaudiocodec
  pspkubridge
)

create_pbp_file(TARGET me-drawing-demo TITLE "Me Drawing Demo" BUILD_PRX)

