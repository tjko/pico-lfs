# pico-lfs
A light-weight littlefs interface library for Raspberry Pi Pico C SDK.


This software is under GPL license, while "littlefs" is subject to its own license.


## Adding library in a project

This is meant to be included as a submodule in a Pico C SDK project.

First add submodule under your project directory:
```
$ mkdir libs
$ cd libs
$ git submodule add https://github.com/tjko/pico-lfs.git
```


Then in _CMakeLists.txt_ add following:
```
# Include pico-lfs library
add_subdirectory(libs/pico-lfs)

...


target_link_libraries(myproject PRIVATE pico-lfs)

target_compile_definitions(myproject PRIVATE
  LFS_THREADSAFE=1
  LFS_NO_DEBUG=1
)

```


## Using library from a program

To use this library you must decide location and size for the _littlefs_ filesystem in the flash memory.

This example assumes using last 256kb of the flash memory:
```
#include "pico_lfs.h"

#define FS_SIZE (256 * 1024)

static struct lfs_config *lfs_cfg;
static lfs_t lfs;

...


int main()
{
  lfs_file_t file;

  /* Near the beginning of your program initialize LFS */

  lfs_cfg = pico_lfs_init(PICO_FLASH_SIZE_BYTES - FS_SIZE, FS_SIZE);
  if (!lfs_cfg)
    panic("out of memory");

  /* Format new FS if needed */

  int err = lfs_mount(&lfs, lfs_cfg);
  if (err != LFS_ERR_OK) {
    /* Initialize new filesystem */
    err = lfs_format(&lfs, lfs_cfg);
    if (err != LFS_ERR_OK)
      panic("failed to format filesystem");
    err = lfs_mount(&lfs, lfs_cfg);
    if (err != LFS_ERR_OK)
      panic("failed to mount new filesystem");
  }


  ...

}
```

After lfs is "mounted" then it can be used as normal via standard littlefs functions...
(see [littlefs documentation](https://github.com/littlefs-project/littlefs))


## Functions

This library implements I/O functions for littlefs to operate on built-in flash on Pico.

This library provides only functions to initialize littlefs and another function to free the filesystem
data structures (application only briefly needs filesystem access, etc).


```
/* Initialize LFS configuration. This allocates memory for the
   the configuration. pico_lfs_destroy() can be used to free
   the configuration if needed.

   Parameters:

   offset = offset in flash memory (must be aligned on 4K page size)
   size = size of the filesystem (must be multiple of 4K page size)
*/
struct lfs_config* pico_lfs_init(size_t offset, size_t size);

/* Release FLS configuration. lfs_unmount() should be called before
   calling this.
*/
void pico_lfs_destroy(struct lfs_config *cfg);
```

