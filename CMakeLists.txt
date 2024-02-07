add_library(pico-lfs INTERFACE)

target_include_directories(pico-lfs INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/libs/littlefs/
)

target_sources(pico-lfs INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src/pico_lfs.c
  ${CMAKE_CURRENT_LIST_DIR}/libs/littlefs/lfs.c
  ${CMAKE_CURRENT_LIST_DIR}/libs/littlefs/lfs_util.c
)

target_compile_definitions(pico-lfs INTERFACE
  LFS_THREADSAFE=1
  LFS_NO_DEBUG=1
)

target_compile_options(pico-lfs INTERFACE -Wno-unused-function)

target_link_libraries(pico-lfs INTERFACE hardware_flash)
