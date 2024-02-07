add_library(pico-lfs INTERFACE)
target_include_directories(pico-lfs INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/libs/littlefs/
  )	
target_link_libraries(pico-lfs INTERFACE hardware_flash)
target_sources(pico-lfs INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src/pico_lfs.c
  ${CMAKE_CURRENT_LIST_DIR}/libs/littlefs/lfs.c
  ${CMAKE_CURRENT_LIST_DIR}/libs/littlefs/lfs_util.c
  )
