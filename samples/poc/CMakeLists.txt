cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "$ENV{PSPDEV}/psp/share/pspdev.cmake" CACHE PATH "toolchain file")
project(me-stask-demo C CXX ASM)

add_executable(me-stask-demo
  main.cpp
)

target_compile_options(me-stask-demo PRIVATE -Os -G0 -Wall -fno-pic)

target_compile_options(me-stask-demo PRIVATE 
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_link_libraries(me-stask-demo
  -Wl,--start-group me-core me-stask -Wl,--end-group
  psppower pspge pspdisplay pspctrl pspdebug
)

create_pbp_file(TARGET me-stask-demo TITLE "Me Safe Task Demo" BUILD_PRX)

