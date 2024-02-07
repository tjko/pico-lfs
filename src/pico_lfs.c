/* pico_lfs.c
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

#include <stdio.h>

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/mutex.h"
#include "lfs.h"

#include "pico_lfs.h"


static int pico_block_device_read(const struct lfs_config *c, lfs_block_t block,
				lfs_off_t off, void *buffer, lfs_size_t size)
{
	pico_lfs_context_t *ctx = (pico_lfs_context_t*)c->context;

	// check if read is valid
	LFS_ASSERT (off  % c->read_size == 0);
	LFS_ASSERT (size % c->read_size == 0);
	LFS_ASSERT (block < c->block_count);

	// read data
	memcpy (buffer, &ctx->fs_base[block * c->block_size + off] + XIP_NOCACHE_NOALLOC_BASE, size);

	return LFS_ERR_OK;
}


static int pico_block_device_prog(const struct lfs_config *c, lfs_block_t block,
				lfs_off_t off, const void *buffer, lfs_size_t size)
{
	pico_lfs_context_t *ctx = (pico_lfs_context_t*)c->context;

	// check if write is valid
	LFS_ASSERT (off  % c->prog_size == 0);
	LFS_ASSERT (size % c->prog_size == 0);
	LFS_ASSERT (block < c->block_count);

	// program data
#if defined (PICO_MCLOCK)
	multicore_lockout_start_blocking ();
#endif
	uint32_t ints = save_and_disable_interrupts ();
	flash_range_program (&ctx->fs_base[block * c->block_size + off] - (uint8_t *)XIP_BASE, buffer, size);
	restore_interrupts (ints);
#if defined (PICO_MCLOCK)
	multicore_lockout_end_blocking ();
#endif

	return LFS_ERR_OK;
}


static int pico_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
	pico_lfs_context_t *ctx = (pico_lfs_context_t*)c->context;

	// check if erase is valid
	LFS_ASSERT (block < c->block_count);

#if defined (PICO_MCLOCK)
	multicore_lockout_start_blocking ();
#endif
	uint32_t ints = save_and_disable_interrupts ();
	flash_range_erase (&ctx->fs_base[block * cfg->block_size] - (uint8_t *)XIP_BASE, c->block_size);
	restore_interrupts (ints);
#if defined (PICO_MCLOCK)
	multicore_lockout_end_blocking ();
#endif

	return LFS_ERR_OK;
}


#ifdef LFS_THREADSAFE
static int pico_block_device_lock(const struct lfs_config *c)
{
	pico_lfs_context_t *ctx = (pico_lfs_context_t*)c->context;

	recursive_mutex_enter_blocking(&ctx->mutex);

	return LFS_ERR_OK;
}


static int pico_block_device_unlock(const struct lfs_config *c)
{
	pico_lfs_context_t *ctx = (pico_lfs_context_t*)c->context;

	recursive_mutex_exit(&ctx->mutex);

	return LFS_ERR_OK;
}
#endif


struct lfs_config* pico_lfs_init(size_t offset, size_t size)
{
	struct lfs_config *cfg;
	struct pico_lfs_context *ctx;

	if (size == 0 || offset % FLASH_SECTOR_SIZE != 0
		|| size % FLASH_SECTOR_SIZE != 0)
		return NULL;

	if (!(cfg = calloc(1, sizeof(struct lfs_config) + sizeof(pico_lfs_context))))
		return NULL;

	ctx = (uint8_t*)cfg + sizeof(struct_lfs_config);
	ctx->fs_base = (uint8_t*)(XIP_BASE + offset);
	cfg->context = ctx;

	// block device operations
	cfg->read  = pico_block_device_read;
	cfg->prog  = pico_block_device_prog;
	cfg->erase = pico_block_device_erase;
	cfg->sync  = NULL;
#ifdef LFS_THREADSAFE
	cfg->lock = pico_block_device_lock;
	cfg->unlock = pico_block_device_unlock;
	recursive_mutex_init(&ctx->mutex);
#endif

	// block device configuration
	cfg->read_size = 1;
	cfg->prog_size = FLASH_PAGE_SIZE;
        cfg->block_size = FLASH_SECTOR_SIZE;
        cfg->block_count = size / FLASH_SECTOR_SIZE;
	cfg->cache_size = FLASH_PAGE_SIZE * 4;
	cfg->lookahead_size = 32;
	cfg->block_cycles = 300;

	return cfg;
}



struct lfs_config pico_lfs_cfg = {

};
