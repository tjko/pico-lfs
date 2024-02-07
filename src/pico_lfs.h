/* pico_lfs.h
   Copyright (C) 2024 Timo Kokkonen <tjko@iki.fi>

   SPDX-License-Identifier: GPL-3.0-or-later

   This file is part of pico-lfs Library.

   pico-lfs Library is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   pico-lfs Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with pico-lfs Library. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _PICO_LFS_H_
#define _PICO_LFS_H_

#include "pico/mutex.h"
#include "lfs.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct pico_lfs_context {
	uint8_t *fs_base;
#ifdef LFS_THREADSAFE
	recursive_mutex_t mutex;
#endif
} pico_lfs_context_t;

extern struct lfs_config pico_lfs_cfg;


struct lfs_config* pico_lfs_init(size_t offset, size_t size);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _PICO_LFS_H_ */
