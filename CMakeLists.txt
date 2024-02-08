project(pico-lfs
  VERSION 1.0.0
  LANGUAGES C CXX
)

add_library(pico-lfs INTERFACE)

target_include_directories(pico-lfs INTERFACE
  src/
  libs/littlefs/
)

target_sources(pico-lfs INTERFACE
  src/pico_lfs.c
  libs/littlefs/lfs.c
  libs/littlefs/lfs_util.c
)

#target_compile_definitions(pico-lfs INTERFACE
#  LFS_THREADSAFE=1
#  LFS_NO_DEBUG=1
#)

target_link_libraries(pico-lfs INTERFACE hardware_flash)


# CMake seems broken(?), if add compile options for interface library here,
# then this get set for every source file on project using this library...
#
#target_compile_options(pico-lfs INTERFACE -Wno-unused-function)
