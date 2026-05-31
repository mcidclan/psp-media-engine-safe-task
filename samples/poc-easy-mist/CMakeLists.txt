cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "$ENV{PSPDEV}/psp/share/pspdev.cmake" CACHE PATH "toolchain file")
project(me-stask-easy-mist C CXX ASM)

add_executable(me-stask-easy-mist
  main.cpp
)

target_compile_options(me-stask-easy-mist PRIVATE -Os -G0 -Wall -fno-pic)

target_compile_options(me-stask-easy-mist PRIVATE 
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_link_libraries(me-stask-easy-mist
  me-core-mapper
  -Wl,--start-group me-core-lib me-stask -Wl,--end-group
  psppower pspge pspdisplay pspctrl pspdebug
  pspkubridge
)

create_pbp_file(TARGET me-stask-easy-mist TITLE "Me Safe Task Easy MIST PoC" BUILD_PRX)

