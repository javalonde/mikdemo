/*
    multisprite.h : multisprite display for the Atari 7800
    Copyleft 2023-2025 Bruno STEUX 

    This file is distributed as a companion file to cc7800 - a subset of C compiler for the Atari 7800
    
    v0.1: First working version
    v0.2: Added DMA Masking vertical scrolling support
    v0.3: Refactored scrolling to avoid memory copy from region to region when coarse scrolling 
    v0.4: Moved _MS_DLL_ARRAY_SIZE to 16 in order to find a way to display off-screen sprites
    v0.5: Added DMA_MASKING_ON_RAM option (DMA masking zone prepended on every DL. Faster but more memory consuming)
    v0.6: Allowing out of screen sprites in vertical scrolling mode 
    v0.6.1: Better memory sharing with conio.h
*/

#ifndef __ATARI7800_MULTISPRITE__
#define __ATARI7800_MULTISPRITE__

#include "prosystem.h"
#include "stdlib.h"
#include "assert.h"

#ifndef INIT_BANK
#define INIT_BANK
#endif

#define MS_YMAX 224
#define _MS_DLL_ARRAY_SIZE 16
#define _MS_DMA_START_VALUE (402 / 2)

#ifndef _MS_DL_SIZE
#define _MS_DL_SIZE 64
#endif
#ifndef _MS_DL_MALLOC
#define _MS_DL_MALLOC(y) _MS_DL_SIZE
#define _MS_DL_LIMIT (_MS_DL_SIZE - 7)
#else
#ifndef _MS_DL_LIMIT
const char _ms_dl_limits[_MS_DLL_ARRAY_SIZE * 2] = {
    _MS_DL_MALLOC(0) - 7, _MS_DL_MALLOC(1) - 7, _MS_DL_MALLOC(2) - 7, _MS_DL_MALLOC(3) - 7, _MS_DL_MALLOC(4) - 7, _MS_DL_MALLOC(5) - 7, _MS_DL_MALLOC(6) - 7, _MS_DL_MALLOC(7) - 7, _MS_DL_MALLOC(8) - 7, _MS_DL_MALLOC(9) - 7, _MS_DL_MALLOC(10) - 7, _MS_DL_MALLOC(11) - 7, _MS_DL_MALLOC(12) - 7, _MS_DL_MALLOC(13) - 7, _MS_DL_MALLOC(14) - 7, _MS_DL_MALLOC(15) - 7,
    _MS_DL_MALLOC(0) - 7, _MS_DL_MALLOC(1) - 7, _MS_DL_MALLOC(2) - 7, _MS_DL_MALLOC(3) - 7, _MS_DL_MALLOC(4) - 7, _MS_DL_MALLOC(5) - 7, _MS_DL_MALLOC(6) - 7, _MS_DL_MALLOC(7) - 7, _MS_DL_MALLOC(8) - 7, _MS_DL_MALLOC(9) - 7, _MS_DL_MALLOC(10) - 7, _MS_DL_MALLOC(11) - 7, _MS_DL_MALLOC(12) - 7, _MS_DL_MALLOC(13) - 7, _MS_DL_MALLOC(14) - 7, _MS_DL_MALLOC(15) - 7
};
#define _MS_DL_LIMIT _ms_dl_limits[X] 
#endif
#endif

// Zeropage variables
char _ms_dmaerror;
#define _ms_tmpptr _libc_tmpptr
#define _ms_tmpptr2 _libc_tmpptr2
#define _ms_tmp _libc_tmp
#define _ms_tmp2 _libc_tmp2
signed char _ms_tmp3;
ramchip char _ms_tmp4, _ms_tmp5;
char *_ms_sparse_tiles_ptr_high, *_ms_sparse_tiles_ptr_low;

#ifdef BIDIR_VERTICAL_SCROLLING
#ifndef VERTICAL_SCROLLING
#define VERTICAL_SCROLLING
#endif
#endif

#ifndef _MS_NB_TOP_ZONES
#define _MS_NB_TOP_ZONES 0
#endif

#ifndef _MS_NB_SCROLLING_ZONES
#define _MS_NB_SCROLLING_ZONES 14
#endif

#ifdef VERTICAL_SCROLLING
#define _MS_DMA_MASKING_OFFSET 17
signed char _ms_vscroll_fine_offset;
ramchip char _ms_vscroll_coarse_offset;
char _ms_vscroll_coarse_offset_shifted;
ramchip char _ms_delayed_vscroll;
#ifdef BIDIR_VERTICAL_SCROLLING
ramchip char _ms_top_sbuffer_size;
ramchip char _ms_top_sbuffer_dma;
ramchip char _ms_bottom_sbuffer_size;
ramchip char _ms_bottom_sbuffer_dma;
ramchip char _ms_scroll_buffers_refill;
#define MS_SCROLL_UP    1
#define MS_SCROLL_DOWN  2
#else
ramchip char _ms_sbuffer_size;
ramchip char _ms_sbuffer_dma;
#endif

#ifdef BIDIR_VERTICAL_SCROLLING
ramchip char _ms_top_sbuffer[_MS_DL_MALLOC(-1)];
ramchip char _ms_bottom_sbuffer[_MS_DL_MALLOC(-2)];
#else
ramchip char _ms_sbuffer[_MS_DL_MALLOC(-1)];
#endif
#else // VERTICAL_SCROLLING
#define _MS_DMA_MASKING_OFFSET 0
#endif

#ifdef HORIZONTAL_SCROLLING
ramchip signed char _ms_hscroll;
ramchip signed char _ms_delayed_hscroll;
#endif

#ifndef _MS_TOP_DISPLAY
#ifdef _MS_TOP_SCROLLING_ZONE
#define _MS_TOP_DISPLAY _MS_TOP_SCROLLING_ZONE
#else
#define _MS_TOP_DISPLAY 0
#endif
#endif

#ifdef __CONIO_H__
#define _MS_FIRST_7_DL_SIZE (\
    _MS_DL_MALLOC(0) + \
    _MS_DL_MALLOC(1) + \
    _MS_DL_MALLOC(2) + \
    _MS_DL_MALLOC(3) + \
    _MS_DL_MALLOC(4) + \
    _MS_DL_MALLOC(5) + \
    _MS_DL_MALLOC(6) + 7 * _MS_DMA_MASKING_OFFSET)
ramchip char _conio_screen[CONIO_NB_LINES * 40 - _MS_FIRST_7_DL_SIZE]; 
#endif

ramchip char _ms_b0_dl0[_MS_DL_MALLOC(0) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl1[_MS_DL_MALLOC(1) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl2[_MS_DL_MALLOC(2) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl3[_MS_DL_MALLOC(3) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl4[_MS_DL_MALLOC(4) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl5[_MS_DL_MALLOC(5) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl6[_MS_DL_MALLOC(6) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl7[_MS_DL_MALLOC(7) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl8[_MS_DL_MALLOC(8) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl9[_MS_DL_MALLOC(9) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl10[_MS_DL_MALLOC(10) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl11[_MS_DL_MALLOC(11) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl12[_MS_DL_MALLOC(12) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl13[_MS_DL_MALLOC(13) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl14[_MS_DL_MALLOC(14) + _MS_DMA_MASKING_OFFSET], _ms_b0_dl15[_MS_DL_MALLOC(15) + _MS_DMA_MASKING_OFFSET];
ramchip char _ms_b1_dl0[_MS_DL_MALLOC(0) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl1[_MS_DL_MALLOC(1) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl2[_MS_DL_MALLOC(2) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl3[_MS_DL_MALLOC(3) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl4[_MS_DL_MALLOC(4) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl5[_MS_DL_MALLOC(5) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl6[_MS_DL_MALLOC(6) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl7[_MS_DL_MALLOC(7) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl8[_MS_DL_MALLOC(8) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl9[_MS_DL_MALLOC(9) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl10[_MS_DL_MALLOC(10) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl11[_MS_DL_MALLOC(11) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl12[_MS_DL_MALLOC(12) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl13[_MS_DL_MALLOC(13) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl14[_MS_DL_MALLOC(14) + _MS_DMA_MASKING_OFFSET], _ms_b1_dl15[_MS_DL_MALLOC(15) + 2 * _MS_DMA_MASKING_OFFSET];

#ifdef VERTICAL_SCROLLING
aligned(256) const char _ms_shift3[256] = {
    0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0,
    1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1,
    2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2,
    3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3,
    4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4,
    5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5,
    6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6,
    7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7,
    8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8,
    9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9,
    10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10,
    11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11,
    12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12,
    13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13,
    14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14,
    15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15,
    0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0,
    1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1,
    2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2,
    3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3,
    4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4,
    5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5,
    6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6,
    7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7,
    8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8,
    9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9,
    10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10,
    11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11,
    12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12,
    13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13,
    14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14,
    15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15
};
#else
aligned(256) const char _ms_shift4[16 * _MS_DLL_ARRAY_SIZE] = {
    0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0, 0, _MS_DLL_ARRAY_SIZE + 0,
    1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1, 1, _MS_DLL_ARRAY_SIZE + 1,
    2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2, 2, _MS_DLL_ARRAY_SIZE + 2,
    3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3, 3, _MS_DLL_ARRAY_SIZE + 3,
    4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4, 4, _MS_DLL_ARRAY_SIZE + 4,
    5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5, 5, _MS_DLL_ARRAY_SIZE + 5,
    6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6, 6, _MS_DLL_ARRAY_SIZE + 6,
    7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7, 7, _MS_DLL_ARRAY_SIZE + 7,
    8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8, 8, _MS_DLL_ARRAY_SIZE + 8,
    9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9, 9, _MS_DLL_ARRAY_SIZE + 9,
    10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10, 10, _MS_DLL_ARRAY_SIZE + 10,
    11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11, 11, _MS_DLL_ARRAY_SIZE + 11,
    12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12, 12, _MS_DLL_ARRAY_SIZE + 12,
    13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13, 13, _MS_DLL_ARRAY_SIZE + 13,
    14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14, 14, _MS_DLL_ARRAY_SIZE + 14,
    15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15, 15, _MS_DLL_ARRAY_SIZE + 15
};
#endif
const char *_ms_dls[_MS_DLL_ARRAY_SIZE * 2] = {
    _ms_b0_dl0 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl1 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl2 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl3 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl4 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl5 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl6 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl7 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl8 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl9 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl10 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl11 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl12 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl13 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl14 + _MS_DMA_MASKING_OFFSET, _ms_b0_dl15 + _MS_DMA_MASKING_OFFSET,
    _ms_b1_dl0 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl1 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl2 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl3 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl4 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl5 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl6 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl7 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl8 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl9 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl10 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl11 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl12 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl13 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl14 + _MS_DMA_MASKING_OFFSET, _ms_b1_dl15 + _MS_DMA_MASKING_OFFSET
};

const char _ms_set_wm_dl[7] = {0, 0x40, 0x21, 0xff, 160, 0, 0}; // Write mode 0
const char _ms_blank_dl[2] = {0, 0};

ramchip char _ms_b0_dll[(_MS_DLL_ARRAY_SIZE + 6 + 4) * 3]; 
ramchip char _ms_b1_dll[(_MS_DLL_ARRAY_SIZE + 6 + 4) * 3];
ramchip char _ms_dlend[_MS_DLL_ARRAY_SIZE * 2];
ramchip char _ms_dlend_save[_MS_DLL_ARRAY_SIZE];
#ifdef MULTISPRITE_OVERLAY
ramchip char _ms_dlend_save_overlay[_MS_DLL_ARRAY_SIZE * 2];
#endif

ramchip char _ms_buffer; // Double buffer state
ramchip char _ms_pal_detected;
ramchip char _ms_pal_frame_skip_counter;

INIT_BANK void multisprite_init();
INIT_BANK void multisprite_clear();
void multisprite_save();
void multisprite_restore();
void multisprite_flip();

#ifdef DMA_CHECK
ramchip char _ms_dldma[_MS_DLL_ARRAY_SIZE * 2];
ramchip char _ms_dldma_save[_MS_DLL_ARRAY_SIZE];
#define _MS_DMA_CHECK(x) \
        _ms_dldma[X] -= (x); \
        if (_ms_dldma[X] < 0) { \
            _ms_dmaerror++; \
            _ms_dldma[X] += (x); \
        } else  
#define _MS_DMA_SUB(x) _ms_dldma[X] -= (x)
#else
#define _MS_DMA_SUB(x)
#define _MS_DMA_CHECK(x) 
#endif 

#ifdef VERTICAL_SCROLLING
#define multisprite_display_sprite(x, y, gfx, width, palette) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
	_ms_tmp = _ms_tmp2 & 0x0f; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp) { \
	            X = _ms_shift3[Y = _ms_tmp3 + 8]; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (((gfx) >> 8) - 0x10) | _ms_tmp; \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_small_sprite(x, y, gfx, width, palette, margin) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
	_ms_tmp = _ms_tmp2 & 0x0f; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp >= margin) { \
                    X = _ms_shift3[Y = _ms_tmp3 + 8]; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = ((gfx) >> 8) - 0x10 + _ms_tmp; \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_small_sprite_ex(x, y, gfx, width, palette, margin, mode) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
	_ms_tmp = _ms_tmp2 & 0x0f; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp >= margin) { \
                    X = _ms_shift3[Y = _ms_tmp3 + 8]; \
                    _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = ((gfx) >> 8) - 0x10 + _ms_tmp; \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_big_sprite(x, y, gfx, width, palette, height, mode) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
	_ms_tmp = _ms_tmp2 & 0x0f; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmp5 = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = _ms_tmp5; \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp) { \
                    _ms_tmpptr2 = (gfx); \
                    _ms_tmp2 = ((_ms_tmpptr2 >> 8) - 0x10) | _ms_tmp; \
                    for (_ms_tmp4 = (height) - 1; _ms_tmp4 != 0; _ms_tmp4--) { \
                        _ms_tmp3 += 8; \
                        X = _ms_shift3[Y = _ms_tmp3]; \
                        _MS_DMA_CHECK((20 + 3 * width * 3 + 1) / 2) { \
                            _ms_tmpptr = _ms_dls[X];  \
                            Y = _ms_dlend[X]; \
                            if (Y >= _MS_DL_LIMIT - 5) { \
                                _ms_dmaerror++; \
                            } else { \
                                _ms_tmpptr[Y++] = _ms_tmpptr2; \
                                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                                _ms_tmpptr[Y++] = _ms_tmp2; \
                                _ms_tmpptr[Y++] = _ms_tmp5; \
                                _ms_tmpptr[Y++] = (x); \
                                _ms_tmpptr2 += width; \
                                _ms_tmpptr[Y++] = _ms_tmpptr2; \
                                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                                _ms_tmpptr[Y++] = (_ms_tmpptr2 >> 8) | _ms_tmp; \
                                _ms_tmpptr[Y++] = _ms_tmp5; \
                                _ms_tmpptr[Y++] = (x); \
                                _ms_dlend[X] = Y; \
                            } \
                        } \
                    } \
                    X = _ms_shift3[Y = _ms_tmp3 + 8]; \
                    _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = _ms_tmpptr2; \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = _ms_tmp2; \
                            _ms_tmpptr[Y++] = _ms_tmp5; \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                } else { \
                    _ms_tmpptr2 = (gfx); \
                    _ms_tmp2 = (_ms_tmpptr2 >> 8) | _ms_tmp; \
                    for (_ms_tmp4 = (height) - 1; _ms_tmp4 != 0; _ms_tmp4--) { \
                        _ms_tmp3 += 8; \
                        X = _ms_shift3[Y = _ms_tmp3]; \
                        _ms_tmpptr2 += width; \
                        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                            _ms_tmpptr = _ms_dls[X];  \
                            Y = _ms_dlend[X]; \
                            if (Y >= _MS_DL_LIMIT) { \
                                _ms_dmaerror++; \
                            } else { \
                                _ms_tmpptr[Y++] = _ms_tmpptr2; \
                                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                                _ms_tmpptr[Y++] = _ms_tmp2; \
                                _ms_tmpptr[Y++] = _ms_tmp5; \
                                _ms_tmpptr[Y++] = (x); \
                                _ms_dlend[X] = Y; \
                            } \
                        } \
                    } \
                } \
            } \
        }

#define multisprite_display_sprite_ex(x, y, gfx, width, palette, mode) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
	_ms_tmp = _ms_tmp2 & 0x0f; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp) { \
                    X = _ms_shift3[Y = _ms_tmp3 + 8]; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = (((gfx) >> 8) - 0x10) | _ms_tmp; \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_sprite_aligned(x, y, gfx, width, palette) \
        Y = ((((y + 15) >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
            }\
        }

#define multisprite_display_sprite_aligned_ex(x, y, gfx, width, palette, mode) \
        Y = ((((y + 15) >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
            }\
        }

#define multisprite_display_big_sprite_aligned(x, y, gfx, width, palette, height) \
        _ms_tmp3 = ((((y + 15) >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmp5 = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = _ms_tmp5; \
                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                _ms_tmp2 = (gfx); \
                for (_ms_tmp4 = (height) - 1; _ms_tmp4 != 0; _ms_tmp4--) { \
                    _ms_tmp3 += 8; \
                    X = _ms_shift3[Y = _ms_tmp3]; \
                    _ms_tmp2 += width; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = _ms_tmp2; \
                            _ms_tmpptr[Y++] = _ms_tmp5; \
                            _ms_tmpptr[Y++] = ((gfx) >> 8); \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                } \
            } \
        }

#define multisprite_display_big_sprite_aligned_ex(x, y, gfx, width, palette, height, mode) \
        _ms_tmp3 = ((((y + 15) >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmp5 = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                _ms_tmpptr[Y++] = _ms_tmp5; \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                _ms_tmp2 = (gfx); \
                for (_ms_tmp4 = (height) - 1; _ms_tmp4 != 0; _ms_tmp4--) { \
                    _ms_tmp3 += 8; \
                    X = _ms_shift3[Y = _ms_tmp3]; \
                    _ms_tmp2 += width; \
                    _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = _ms_tmp2; \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = ((gfx) >> 8); \
                            _ms_tmpptr[Y++] = _ms_tmp5; \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                } \
            } \
        }

#define multisprite_display_sprite_fast(x, y, gfx, width, palette) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
        _ms_tmp = _ms_tmp2 & 0x0f; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_SUB((8 + width * 3 + 1) / 2); \
        _ms_tmpptr = _ms_dls[X]; \
        Y = _ms_dlend[X]; \
        _ms_tmpptr[Y++] = (gfx); \
        _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
        _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
        _ms_tmpptr[Y++] = (x); \
        _ms_dlend[X] = Y; \
        if (_ms_tmp) { \
	    X = _ms_shift3[Y = _ms_tmp3 + 8]; \
            _MS_DMA_SUB((8 + width * 3 + 1) / 2); \
            _ms_tmpptr = _ms_dls[X];  \
            Y = _ms_dlend[X]; \
            _ms_tmpptr[Y++] = (gfx); \
            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
            _ms_tmpptr[Y++] = (((gfx) >> 8) - 0x10) | _ms_tmp; \
            _ms_tmpptr[Y++] = (x); \
            _ms_dlend[X] = Y; \
        }

#define multisprite_reserve_dma(y, nb_sprites, width) \
        _ms_tmp2 = (y) + _ms_vscroll_fine_offset; \
        _ms_tmp3 = (((_ms_tmp2 >> 1) + _ms_vscroll_coarse_offset_shifted) & 0xfe | _ms_buffer); \
	X = _ms_shift3[Y = _ms_tmp3]; \
        _MS_DMA_SUB(nb_sprites * (8 + width * 3 + 1) / 2); \
        if (_ms_tmp2 & 0x0f) { \
	    X = _ms_shift3[Y = _ms_tmp3 + 8]; \
            _MS_DMA_SUB(nb_sprites * (8 + width * 3 + 1) / 2); \
        }

#else
#define multisprite_display_sprite(x, y, gfx, width, palette) \
	_ms_tmp = (y) & 0x0f; \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp) { \
                    X++; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (((gfx) >> 8) - 0x10) | _ms_tmp; \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_small_sprite(x, y, gfx, width, palette, margin) \
	_ms_tmp = (y) & 0x0f; \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp >= margin) { \
                    X++; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = ((gfx) >> 8) - 0x10 + _ms_tmp; \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_small_sprite_ex(x, y, gfx, width, palette, margin, mode) \
	_ms_tmp = (y) & 0x0f; \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp >= margin) { \
                    X++; \
                    _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = ((gfx) >> 8) - 0x10 + _ms_tmp; \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_big_sprite(x, y, gfx, width, palette, height, mode) \
	_ms_tmp = (y) & 0x0f; \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp) { \
                    _ms_tmpptr2 = (gfx); \
                    _ms_tmp2 = ((_ms_tmpptr2 >> 8) - 0x10) | _ms_tmp; \
                    for (_ms_tmp3 = (height) - 1; _ms_tmp3 != 0; _ms_tmp3--) { \
                        X++; \
                        _MS_DMA_CHECK((20 + 3 * width * 3 + 1) / 2) { \
                            _ms_tmpptr = _ms_dls[X];  \
                            Y = _ms_dlend[X]; \
                            if (Y >= _MS_DL_LIMIT - 5) { \
                                _ms_dmaerror++; \
                            } else { \
                                _ms_tmpptr[Y++] = _ms_tmpptr2; \
                                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                                _ms_tmpptr[Y++] = _ms_tmp2; \
                                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                                _ms_tmpptr[Y++] = (x); \
                                _ms_tmpptr2 += width; \
                                _ms_tmpptr[Y++] = _ms_tmpptr2; \
                                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                                _ms_tmpptr[Y++] = (_ms_tmpptr2 >> 8) | _ms_tmp; \
                                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                                _ms_tmpptr[Y++] = (x); \
                                _ms_dlend[X] = Y; \
                            } \
                        } \
                    } \
                    X++; \
                    _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = _ms_tmpptr2; \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = _ms_tmp2; \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                } else { \
                    _ms_tmp2 = (gfx); \
                    for (_ms_tmp3 = (height) - 1; _ms_tmp3 != 0; _ms_tmp3--) { \
                        X++; \
                        _ms_tmp2 += width; \
                        _MS_DMA_CHECK((10 + width * 3 + 1) / 2) { \
                            _ms_tmpptr = _ms_dls[X];  \
                            Y = _ms_dlend[X]; \
                            if (Y >= _MS_DL_LIMIT) { \
                                _ms_dmaerror++; \
                            } else { \
                                _ms_tmpptr[Y++] = _ms_tmp2; \
                                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                                _ms_tmpptr[Y++] = (x); \
                                _ms_dlend[X] = Y; \
                            } \
                        } \
                    } \
                } \
            } \
        }

#define multisprite_display_sprite_ex(x, y, gfx, width, palette, mode) \
	_ms_tmp = (y) & 0x0f; \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
                if (_ms_tmp) { \
                    X++; \
                    _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
                        _ms_tmpptr = _ms_dls[X];  \
                        Y = _ms_dlend[X]; \
                        if (Y >= _MS_DL_LIMIT) { \
                            _ms_dmaerror++; \
                        } else { \
                            _ms_tmpptr[Y++] = (gfx); \
                            _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                            _ms_tmpptr[Y++] = (((gfx) >> 8) - 0x10) | _ms_tmp; \
                            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                            _ms_tmpptr[Y++] = (x); \
                            _ms_dlend[X] = Y; \
                        } \
                    } \
                }\
            }\
        }

#define multisprite_display_sprite_aligned(x, y, gfx, width, palette, mode) \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = (mode)?0xc0:0x40; \
                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
            }\
        }

#define multisprite_display_sprite_aligned_fast(x, y, gfx, width, palette) \
	X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_CHECK((8 + width * 3 + 1) / 2) { \
            _ms_tmpptr = _ms_dls[X]; \
            Y = _ms_dlend[X]; \
            if (Y >= _MS_DL_LIMIT) { \
                _ms_dmaerror++; \
            } else { \
                _ms_tmpptr[Y++] = (gfx); \
                _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
                _ms_tmpptr[Y++] = ((gfx) >> 8); \
                _ms_tmpptr[Y++] = (x); \
                _ms_dlend[X] = Y; \
            }\
        }

#define multisprite_display_sprite_fast(x, y, gfx, width, palette) \
        _ms_tmp = (y) & 0x0f; \
        X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_SUB((8 + width * 3 + 1) / 2);  \
        _ms_tmpptr = _ms_dls[X]; \
        Y = _ms_dlend[X]; \
        _ms_tmpptr[Y++] = (gfx); \
        _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
        _ms_tmpptr[Y++] = ((gfx) >> 8) | _ms_tmp; \
        _ms_tmpptr[Y++] = (x); \
        _ms_dlend[X] = Y; \
        if (_ms_tmp) { \
            X++; \
            _MS_DMA_SUB((8 + width * 3 + 1) / 2);  \
            _ms_tmpptr = _ms_dls[X];  \
            Y = _ms_dlend[X]; \
            _ms_tmpptr[Y++] = (gfx); \
            _ms_tmpptr[Y++] = -width & 0x1f | (palette << 5); \
            _ms_tmpptr[Y++] = (((gfx) >> 8) - 0x10) | _ms_tmp;  \
            _ms_tmpptr[Y++] = (x); \
            _ms_dlend[X] = Y; \
        }

#define multisprite_reserve_dma(y, nb_sprites, width) \
        _ms_tmp = (y) & 0x0f; \
        X = _ms_shift4[Y = (y & 0xfe | _ms_buffer)]; \
        _MS_DMA_SUB(nb_sprites * (8 + width * 3 + 1) / 2); \
        if ((y) & 0x0f) { \
            X++; \
            _MS_DMA_SUB(nb_sprites * (8 + width * 3 + 1) / 2); \
        }

#endif

#define multisprite_display_tiles(x, y, tiles, size, palette) \
    X = (y); \
    if (_ms_buffer) X += _MS_DLL_ARRAY_SIZE; \
    _MS_DMA_CHECK((10 + 3 + size * 9 + 1) / 2) { \
        _ms_tmpptr = _ms_dls[X]; \
        Y = _ms_dlend[X]; \
        if (Y >= _MS_DL_LIMIT) { \
            _ms_dmaerror++; \
         } else { \
            _ms_tmpptr[Y++] = (tiles); \
            _ms_tmpptr[Y++] = 0x60; \
            _ms_tmpptr[Y++] = (tiles) >> 8; \
            _ms_tmpptr[Y++] = -size & 0x1f | (palette << 5); \
            _ms_tmpptr[Y++] = (x); \
            _ms_dlend[X] = Y; \
        } \
    }

#define multisprite_display_tiles_fast(x, y, tiles, size, palette) \
    X = (y); \
    if (_ms_buffer) X += _MS_DLL_ARRAY_SIZE; \
    _MS_DMA_SUB((10 + 3 + size * 9 + 1) / 2); \
    _ms_tmpptr = _ms_dls[X]; \
    Y = _ms_dlend[X]; \
    _ms_tmpptr[Y++] = (tiles); \
    _ms_tmpptr[Y++] = 0x60; \
    _ms_tmpptr[Y++] = (tiles) >> 8; \
    _ms_tmpptr[Y++] = -size & 0x1f | (palette << 5); \
    _ms_tmpptr[Y++] = (x); \
    _ms_dlend[X] = Y; 

#define multisprite_set_charbase(ptr) *CHARBASE = (ptr) >> 8;

// Macro to convert NTSC colors to PAL colors
#define multisprite_color(color) ((color >= 0xf0)?(0x20 + (color & 0x0f)):((color >= 0x10)?(color + (_ms_pal_detected & 0x10)):color))

#define multisprite_enable_dli(line) _ms_tmp = line; _multisprite_enable_dli()
#define multisprite_disable_dli(line) _ms_tmp = line; _multisprite_disable_dli()

INIT_BANK void multisprite_get_tv()
{
    while (!(*MSTAT < 0)); // Wait for VBLANK
    while (*MSTAT < 0); // Wait for end of VBLANK

    X = 0;
    do {
        strobe(WSYNC);
        strobe(WSYNC);
        X++;
    } while (!(*MSTAT & 0x80));

    if (X >= 135) _ms_pal_detected = 0xff; 
    _ms_pal_frame_skip_counter = 0;
}

char multisprite_pal_frame_skip()
{
    if (!_ms_pal_detected) return 0;
    _ms_pal_frame_skip_counter++;
    if (_ms_pal_frame_skip_counter >= 5) {
        _ms_pal_frame_skip_counter = 0;
        return 1;
    }
    return 0;
}

inline void multisprite_start()
{
#ifdef VERTICAL_SCROLLING
    _ms_vscroll_fine_offset = 0;
    _ms_vscroll_coarse_offset = 0;
    _ms_delayed_vscroll = 0;
#ifdef BIDIR_VERTICAL_SCROLLING
    _ms_top_sbuffer_size = 0;
    _ms_top_sbuffer_dma = _MS_DMA_START_VALUE;
    _ms_bottom_sbuffer_size = 0;
    _ms_bottom_sbuffer_dma = _MS_DMA_START_VALUE;
    _ms_scroll_buffers_refill = 3;
#else
    _ms_sbuffer_size = 0;
    _ms_sbuffer_dma = _MS_DMA_START_VALUE;
#endif
#endif   
 
#ifdef HORIZONTAL_SCROLLING
    _ms_delayed_hscroll = 0;
#endif

    _ms_buffer = 0; // 0 is the current write buffer
    _ms_dmaerror = 0;
    *DPPH = _ms_b1_dll >> 8; // 1 the current displayed buffer
    *DPPL = _ms_b1_dll;
#ifdef MODE_320AC
    *CTRL = 0x53;
#else
#ifdef MODE_320BD
    *CTRL = 0x52;
#else
    *CTRL = 0x50; // DMA on, 160A/B mode, Two (2) byte characters mode
#endif
#endif
}

INIT_BANK void multisprite_init()
{
    *BACKGRND = 0x0;

    multisprite_get_tv();
    multisprite_clear();
    multisprite_save();

    _ms_tmpptr = _ms_b0_dll;
    for (X = 0, _ms_tmp = 0; _ms_tmp <= 1; _ms_tmp++) {
        // Build DLL
        // 69 blank lines for PAL
        // 19 blank lines for NTSC
        if (_ms_pal_detected) {
            // 16 blank lines
            _ms_tmpptr[Y = 0] = 0x0f;  // 16 lines
            _ms_tmpptr[++Y] = _ms_set_wm_dl >> 8;
            _ms_tmpptr[++Y] = _ms_set_wm_dl;
            // 16 blank lines
            _ms_tmpptr[++Y] = 0x2f;  // 16 lines.. 8 high zone Holey DMA enabled just in case...
            _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
            _ms_tmpptr[++Y] = _ms_blank_dl;
        } else {
            _ms_tmpptr[Y = 0] = 0x27; // 8 lines. 8 high zone Holey DMA enabled just in case...
            _ms_tmpptr[++Y] = _ms_set_wm_dl >> 8;
            _ms_tmpptr[++Y] = _ms_set_wm_dl;
        }
        for (_ms_tmp2 = 0; _ms_tmp2 != _MS_NB_TOP_ZONES; _ms_tmp2++) {
            _ms_tmpptr[++Y] = 0x4f; // 16 lines
            _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
            _ms_tmpptr[++Y] = _ms_blank_dl;
        }
        // 16 pixel high regions (14 regions = 224 pixels)
        for (_ms_tmp2 = 0; _ms_tmp2 != _MS_NB_SCROLLING_ZONES; X++, _ms_tmp2++) {
            _ms_tmpptr[++Y] = 0x4f; // 16 lines
            _ms_tmpptr[++Y] = _ms_dls[X] >> 8; // High address
            _ms_tmpptr[++Y] = _ms_dls[X]; // Low address
        }
        // 1 pixel high region for the last display list (for vertical scrolling)
        _ms_tmpptr[++Y] = 0x40; // 1 line
        _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
        _ms_tmpptr[++Y] = _ms_blank_dl;
        if (_ms_pal_detected) {
            // 16 blank lines
            _ms_tmpptr[++Y] = 0x2f;  // 16 lines. 8 high zone Holey DMA enabled just in case...
            _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
            _ms_tmpptr[++Y] = _ms_blank_dl;
            // 16 blank lines
            _ms_tmpptr[++Y] = 0x0f;  // 16 lines
            _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
            _ms_tmpptr[++Y] = _ms_blank_dl;
            // 4 blank lines
            _ms_tmpptr[++Y] = 0x03;  // 4 lines
            _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
            _ms_tmpptr[++Y] = _ms_blank_dl;
        } else {
            _ms_tmpptr[++Y] = 0x29; // 10 lines. 8 high zone Holey DMA enabled just in case...
            _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
            _ms_tmpptr[++Y] = _ms_blank_dl;
        }
        _ms_tmpptr = _ms_b1_dll;
        X = _MS_DLL_ARRAY_SIZE;
    }

#ifdef VERTICAL_SCROLLING
    // Fill the prepended data with DMA masking object
    for (X = 0; X != _MS_DLL_ARRAY_SIZE * 2; X++) {
        _ms_tmpptr = _ms_dls[X] - 17;
        Y = 0;
        _ms_tmpptr[Y++] = 0; 
        _ms_tmpptr[Y++] = 0xc0; // WM = 1, Direct mode
        _ms_tmpptr[Y++] = 0xa0;
        _ms_tmpptr[Y++] = 0;
        _ms_tmpptr[Y++] = 161; 
        for (_ms_tmp = 0; _ms_tmp != 3; _ms_tmp++) {
            _ms_tmpptr[Y++] = 0; 
            _ms_tmpptr[Y++] = 0xe1;
            _ms_tmpptr[Y++] = 0xa0;
            _ms_tmpptr[Y++] = 161; 
        }
    }
#endif

    multisprite_start();
}

INIT_BANK void multisprite_clear()
{
    // Reset DL ends for both buffers
    for (X = _MS_DLL_ARRAY_SIZE * 2 - 1; X >= 0; X--) {
        _ms_dlend[X] = 0;
#ifdef DMA_CHECK
        _ms_dldma[X] = _MS_DMA_START_VALUE;
#endif
#ifdef MULTISPRITE_OVERLAY
        _ms_dlend_save_overlay[X] = 0;
#endif
    }
    for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
        _ms_dlend_save[X] = 0;
#ifdef DMA_CHECK
        _ms_dldma_save[X] = _MS_DMA_START_VALUE;
#endif
    }
#ifdef VERTICAL_SCROLLING
#ifdef BIDIR_VERTICAL_SCROLLING
    _ms_top_sbuffer_size = 0;
    _ms_bottom_sbuffer_size = 0;
#else
    _ms_sbuffer_size = 0;
#endif
    _ms_delayed_vscroll = 0;
#endif
}

// This one should be done during VBLANK, since we are copying from write buffer to currently displayed buffer
void multisprite_save()
{
    while (!(*MSTAT < 0)); // Wait for VBLANK
    if (_ms_buffer) {
        for (Y = _MS_DLL_ARRAY_SIZE * 2 - 1, X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; Y--, X--) {
            _ms_dlend_save[X] = _ms_dlend[Y];
#ifdef MULTISPRITE_OVERLAY
            _ms_dlend_save_overlay[Y] = _ms_dlend[Y];
            _ms_dlend_save_overlay[X] = _ms_dlend[Y];
#endif
#ifdef DMA_CHECK
            _ms_dldma_save[X] = _ms_dldma[Y];
#endif
        }
        // Copy the DLs from current write buffer to all buffers
        for (_ms_tmp = _MS_DLL_ARRAY_SIZE - 1; _ms_tmp >= 0; _ms_tmp--) {
            _ms_tmpptr = _ms_dls[X = _ms_tmp + _MS_DLL_ARRAY_SIZE];
            _ms_tmpptr2 = _ms_dls[X = _ms_tmp];
            for (Y = _ms_dlend_save[X] - 1; Y >= 0; Y--) {
                _ms_tmpptr2[Y] = _ms_tmpptr[Y];
            } 
        }
    } else {
        for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
            _ms_dlend_save[X] = _ms_dlend[X];
#ifdef DMA_CHECK
            _ms_dldma_save[X] = _ms_dldma[X];
#endif
        }
#ifdef MULTISPRITE_OVERLAY
        for (Y = _MS_DLL_ARRAY_SIZE * 2 - 1, X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; Y--, X--) {
            _ms_dlend_save_overlay[Y] = _ms_dlend[X];
            _ms_dlend_save_overlay[X] = _ms_dlend[X];
        }
#endif
        // Copy the DLs from current write buffer to all buffers
        for (_ms_tmp = _MS_DLL_ARRAY_SIZE - 1; _ms_tmp >= 0; _ms_tmp--) {
            _ms_tmpptr = _ms_dls[X = _ms_tmp + _MS_DLL_ARRAY_SIZE];
            _ms_tmpptr2 = _ms_dls[X = _ms_tmp];
            for (Y = _ms_dlend_save[X] - 1; Y >= 0; Y--) {
                _ms_tmpptr[Y] = _ms_tmpptr2[Y];
            } 
        }
    }
}

#ifdef MULTISPRITE_OVERLAY
void multisprite_save_overlay()
{
    if (_ms_buffer) {
        for (Y = _MS_DLL_ARRAY_SIZE * 2 - 1, X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; Y--, X--) {
            _ms_dlend_save_overlay[Y] = _ms_dlend[Y];
        }
    } else {
        for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
            _ms_dlend_save_overlay[X] = _ms_dlend[X];
        }
    }
}

void multisprite_clear_overlay()
{
    if (_ms_buffer) {
        for (Y = _MS_DLL_ARRAY_SIZE * 2 - 1, X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; Y--, X--) {
            _ms_dlend_save_overlay[Y] = _ms_dlend_save[X];
            _ms_dlend[Y] = _ms_dlend_save[X];
        }
    } else {
        for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
            _ms_dlend_save_overlay[X] = _ms_dlend_save[X];
            _ms_dlend[X] = _ms_dlend_save[X];
        }
    }
}
#endif

void multisprite_restore()
{
    if (_ms_buffer) {
        for (Y = _MS_DLL_ARRAY_SIZE * 2 - 1, X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; Y--, X--) {
            _ms_dlend[Y] = _ms_dlend_save[X];
#ifdef DMA_CHECK
            _ms_dldma[Y] = _ms_dldma_save[X];
#endif
        }
    } else {
        for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
            _ms_dlend[X] = _ms_dlend_save[X];
#ifdef DMA_CHECK
            _ms_dldma[X] = _ms_dldma_save[X];
#endif
        }
    }
}

#ifdef VERTICAL_SCROLLING
#ifdef BIDIR_VERTICAL_SCROLLING
#define multisprite_top_vscroll_buffer_tiles(x, tiles, size, palette) \
    _ms_top_sbuffer_dma -= (10 + 3 + 21 * 9 + 1) / 2; \
    Y = _ms_top_sbuffer_size; \
    _ms_top_sbuffer[Y++] = (tiles); \
    _ms_top_sbuffer[Y++] = 0x60; \
    _ms_top_sbuffer[Y++] = (tiles) >> 8; \
    _ms_top_sbuffer[Y++] = -size & 0x1f | (palette << 5); \
    _ms_top_sbuffer[Y++] = (x); \
    _ms_top_sbuffer_size = Y;

#define multisprite_top_vscroll_buffer_sprite(x, gfx, width, palette) \
    _ms_top_sbuffer_dma -= (8 + width * 3 + 1) / 2; \
    Y = _ms_top_sbuffer_size; \
    _ms_top_sbuffer[Y++] = (gfx); \
    _ms_top_sbuffer[Y++] = -width & 0x1f | (palette << 5); \
    _ms_top_sbuffer[Y++] = ((gfx) >> 8); \
    _ms_top_sbuffer[Y++] = (x); \
    _ms_top_sbuffer_size = Y;

#define multisprite_bottom_vscroll_buffer_tiles(x, tiles, size, palette) \
    _ms_bottom_sbuffer_dma -= (10 + 3 + 21 * 9 + 1) / 2; \
    Y = _ms_bottom_sbuffer_size; \
    _ms_bottom_sbuffer[Y++] = (tiles); \
    _ms_bottom_sbuffer[Y++] = 0x60; \
    _ms_bottom_sbuffer[Y++] = (tiles) >> 8; \
    _ms_bottom_sbuffer[Y++] = -size & 0x1f | (palette << 5); \
    _ms_bottom_sbuffer[Y++] = (x); \
    _ms_bottom_sbuffer_size = Y;

#define multisprite_bottom_vscroll_buffer_sprite(x, gfx, width, palette) \
    _ms_bottom_sbuffer_dma -= (8 + width * 3 + 1) / 2; \
    Y = _ms_bottom_sbuffer_size; \
    _ms_bottom_sbuffer[Y++] = (gfx); \
    _ms_bottom_sbuffer[Y++] = -width & 0x1f | (palette << 5); \
    _ms_bottom_sbuffer[Y++] = ((gfx) >> 8); \
    _ms_bottom_sbuffer[Y++] = (x); \
    _ms_bottom_sbuffer_size = Y;

#define multisprite_vscroll_buffers_refill_status() (_ms_scroll_buffers_refill)
#define multisprite_vscroll_buffers_refilled() _ms_scroll_buffers_refill = 0

#else
#define multisprite_vscroll_buffer_tiles(x, tiles, size, palette) \
    _ms_sbuffer_dma -= (10 + 3 + size * 9 + 1) / 2; \
    Y = _ms_sbuffer_size & 0x7f; \
    _ms_sbuffer[Y++] = (tiles); \
    _ms_sbuffer[Y++] = 0x60; \
    _ms_sbuffer[Y++] = (tiles) >> 8; \
    _ms_sbuffer[Y++] = -size & 0x1f | (palette << 5); \
    _ms_sbuffer[Y++] = (x); \
    _ms_sbuffer_size = Y;

#define multisprite_vscroll_buffer_sprite(x, gfx, width, palette) \
    _ms_sbuffer_dma -= (8 + width * 3 + 1) / 2; \
    Y = _ms_sbuffer_size & 0x7f; \
    _ms_sbuffer[Y++] = (gfx); \
    _ms_sbuffer[Y++] = -width & 0x1f | (palette << 5); \
    _ms_sbuffer[Y++] = ((gfx) >> 8); \
    _ms_sbuffer[Y++] = (x); \
    _ms_sbuffer_size = Y;

#define multisprite_vscroll_buffer_empty() (!_ms_sbuffer_size)

#define multisprite_vscroll_init_sparse_tiles(ptr) \
    _ms_sparse_tiles_ptr_high = ptr[Y = 0];\
    _ms_sparse_tiles_ptr_low = ptr[Y = 1];\
    _ms_sbuffer_size = 128;

void multisprite_vscroll_buffer_sparse_tiles(char c)
{
    char *stiles, tmp, writemode = -1;
    Y = c;
    stiles = _ms_sparse_tiles_ptr_low[Y] | (_ms_sparse_tiles_ptr_high[Y] << 8);   
    Y = 1;
    tmp = stiles[Y];
    X = _ms_sbuffer_size & 0x7f;
    while (tmp != 0xff) {
        _ms_sbuffer[X++] = stiles[++Y]; // Low address
        char mode = stiles[++Y];
        if ((mode & 0x20) || ((mode & 0x80) != writemode)) { // Indirect or different writemode
            _ms_sbuffer[X++] = mode;
            _ms_sbuffer[X++] = stiles[++Y];
            _ms_sbuffer[X++] = stiles[++Y];
            writemode = mode & 0x80;
        } else {
            // We can do it in 4 bytes mode
            char ha = stiles[++Y];
            char wp = stiles[++Y];
            _ms_sbuffer[X++] = wp;
            _ms_sbuffer[X++] = ha;
        }
        _ms_sbuffer[X++] = tmp << 3;
        _ms_sbuffer_dma -= stiles[++Y];
        ++Y;
        tmp = stiles[++Y];
    }
    if (!X) X = 128; // To mark sbuffer_size != 0
    _ms_sbuffer_size = X;
}

#ifdef MULTISPRITE_USE_VIDEO_MEMORY
ramchip char _ms_vscroll_sparse_step;
ramchip char _ms_vscroll_sparse_vmem_ptr_low, _ms_vscroll_sparse_vmem_ptr_high, _ms_vscroll_charbase;
char *_ms_sbuffer_sparse_tilemap_ptr;

bank1 char multisprite_vmem[12288]; // Video memory in RAM
bank1 const char multisprite_vscroll_init_sparse_tiles_vmem_use_rom[] = {1};

#define multisprite_vscroll_init_sparse_tiles_vmem(ptr, tiles_ptr) \
{ \
    _ms_sparse_tiles_ptr_high = ptr[Y = 0]; \
    _ms_sparse_tiles_ptr_low = ptr[Y = 1]; \
    _ms_vscroll_charbase = (tiles_ptr) >> 8; \
    _ms_vscroll_sparse_vmem_ptr_low = 0x00; \
    _ms_vscroll_sparse_vmem_ptr_high = 0x40; \
    _ms_vscroll_sparse_step = 255; \
    _ms_sbuffer_size = 128; \
} \

void multisprite_vscroll_buffer_sparse_tiles_vmem(char c)
{
    char len, len2, tmp, low, tmp2, tmp3, high, first = 1, low2, high2;
    low = _ms_vscroll_sparse_vmem_ptr_low;
    high = _ms_vscroll_sparse_vmem_ptr_high;
    Y = c;
    _ms_sbuffer_sparse_tilemap_ptr = _ms_sparse_tiles_ptr_low[Y] | (_ms_sparse_tiles_ptr_high[Y] << 8);   
    Y = 0;
    len = _ms_sbuffer_sparse_tilemap_ptr[Y++];
    tmp = _ms_sbuffer_sparse_tilemap_ptr[Y];
    X = _ms_sbuffer_size & 0x7f;
    while (tmp != 0xff) {
        len2 = len - tmp + 1;
        len = len2 << 1;
        low2 = _ms_sbuffer_sparse_tilemap_ptr[++Y]; 
        if (_ms_sbuffer_sparse_tilemap_ptr[++Y] & 0x20) {
            Y++; Y++;
            tmp2 = low - len;
            if (tmp2 < 0) {
                low = -len;
                //high ^= 16;
                high += 16;
                if (high == 0x70) high = 0x40;
            } else low = tmp2;
            tmp3 = _ms_sbuffer_sparse_tilemap_ptr[Y] & 0xe0;
            tmp2 = ((-len) & 0x1f) | tmp3;
            if (first) {
                _ms_sbuffer[X++] = low;
                _ms_sbuffer[X++] = 0x40; // 160A only
                _ms_sbuffer[X++] = high;
                _ms_sbuffer[X++] = tmp2;
                _ms_sbuffer_dma -= 5 + len + len2;
                first = 0;
            } else {
                _ms_sbuffer[X++] = low;
                _ms_sbuffer[X++] = tmp2;
                _ms_sbuffer[X++] = high;
                _ms_sbuffer_dma -= 4 + len + len2;
            }
        } else {
            Y++;
            high2 = _ms_sbuffer_sparse_tilemap_ptr[Y++]; 
            tmp3 = _ms_sbuffer_sparse_tilemap_ptr[Y] & 0xe0;
            tmp2 = ((-len) & 0x1f) | tmp3;
            if (first) {
                _ms_sbuffer[X++] = low2;
                _ms_sbuffer[X++] = 0x40; // 160A only
                _ms_sbuffer[X++] = high2;
                _ms_sbuffer[X++] = tmp2;
                _ms_sbuffer_dma -= 5 + len + len2;
                first = 0;
            } else {
                _ms_sbuffer[X++] = low2;
                _ms_sbuffer[X++] = tmp2;
                _ms_sbuffer[X++] = high2;
                _ms_sbuffer_dma -= 4 + len + len2;
            }
        }
        Y++;
        _ms_sbuffer[X++] = tmp << 3;
        len = _ms_sbuffer_sparse_tilemap_ptr[++Y];
        tmp = _ms_sbuffer_sparse_tilemap_ptr[++Y];
    }
    if (!X) X = 128; // To mark sbuffer_size != 0
    _ms_sbuffer_size = X;
    _ms_vscroll_sparse_step = 15;
}

char multisprite_vscroll_buffer_sparse_tiles_vmem_step()
{
    char len, len2, tmp, low, tmp2, high, chlow, chhigh, *chptr, *tilesptr, *vmemptr, byte1;
    if (_ms_vscroll_sparse_step == 255) return 0;
    tmp = _ms_vscroll_charbase + _ms_vscroll_sparse_step;
    tilesptr = tmp << 8;
    low = _ms_vscroll_sparse_vmem_ptr_low;
    high = _ms_vscroll_sparse_vmem_ptr_high + _ms_vscroll_sparse_step;
    Y = 0;
    len = _ms_sbuffer_sparse_tilemap_ptr[Y++];
    tmp = _ms_sbuffer_sparse_tilemap_ptr[Y];
    while (tmp != 0xff) {
        chlow = _ms_sbuffer_sparse_tilemap_ptr[++Y];
        if (_ms_sbuffer_sparse_tilemap_ptr[++Y] & 0x20) {
            chhigh = _ms_sbuffer_sparse_tilemap_ptr[++Y];
            ++Y; ++Y;
            len2 = len - tmp + 1; // in chars
            len = len2 << 1; // in bytes
            tmp2 = low - len;
            if (tmp2 < 0) {
                low = -len;
                //high ^= 16;
                high += 16;
                if (high >= 0x70) high = 0x40 + _ms_vscroll_sparse_step;
            } else low = tmp2;
            vmemptr = low | (high << 8);

            _save_y = Y;
            // Copy the row of chars for current step
            chptr = chlow | (chhigh << 8);
            for (Y = 0; Y != len2; Y++) {
                tmp2 = Y;
                Y = chptr[Y];
                byte1 = tilesptr[Y++];
                X = tilesptr[Y];
                Y = tmp2 << 1;
                vmemptr[Y++] = byte1;
                vmemptr[Y] = X;
                Y = tmp2;
            }
            Y = _save_y;
        } else {
            ++Y; ++Y; ++Y;
        }
        len = _ms_sbuffer_sparse_tilemap_ptr[++Y];
        tmp = _ms_sbuffer_sparse_tilemap_ptr[++Y];
    }
    _ms_vscroll_sparse_step--;
    if (_ms_vscroll_sparse_step == 255) {
        _ms_vscroll_sparse_vmem_ptr_low = low;
        _ms_vscroll_sparse_vmem_ptr_high = high;
        return 0;
    }
    return 1;
}

#endif
#endif

void _ms_move_dlls_down()
{
    if (_ms_pal_detected) {
        Y = 6 + (_MS_NB_TOP_ZONES * 3);
    } else {
        Y = 3 + (_MS_NB_TOP_ZONES * 3);
    }
    if (_ms_buffer) {
        _ms_tmpptr = _ms_b1_dll;
        X = _ms_vscroll_coarse_offset + _MS_DLL_ARRAY_SIZE;
        // 16 pixel high regions
        for (_ms_tmp2 = 0; _ms_tmp2 != _MS_NB_SCROLLING_ZONES; _ms_tmp2++) {
            Y++;
            _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
            _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
            X++;
            if (X == _MS_DLL_ARRAY_SIZE + 16) X = _MS_DLL_ARRAY_SIZE;
        }
        Y++;
        _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
        _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
        
        // New: Restore DMA masking zone data on the last line
        _ms_tmpptr = _ms_dls[X] - 17;
        // Copy the scroll buffer to the first zone 
        X = _ms_vscroll_coarse_offset + _MS_DLL_ARRAY_SIZE;
    } else {
        _ms_tmpptr = _ms_b0_dll;
        X = _ms_vscroll_coarse_offset;
        // 16 pixel high regions
        for (_ms_tmp2 = 0; _ms_tmp2 != _MS_NB_SCROLLING_ZONES; _ms_tmp2++) {
            Y++;
            _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
            _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
            X++;
            if (X == 16) X = 0;
        }
        Y++;
        _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
        _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
        
        // New: Restore DMA masking zone data on the last line
        _ms_tmpptr = _ms_dls[X] - 17;
        // Copy the scroll buffer to the first zone 
        X = _ms_vscroll_coarse_offset;
    }
        
    // New: Restore DMA masking zone data on the last line
    Y = 0;
    _ms_tmpptr[Y++] = 0; 
    _ms_tmpptr[Y++] = 0xc0; // WM = 1, Direct mode
    _ms_tmpptr[Y++] = 0xa0;
    _ms_tmpptr[Y++] = 0;
    _ms_tmpptr[Y++] = 161; 
    for (_ms_tmp = 0; _ms_tmp != 3; _ms_tmp++) {
        _ms_tmpptr[Y++] = 0; 
        _ms_tmpptr[Y++] = 0xe1;
        _ms_tmpptr[Y++] = 0xa0;
        _ms_tmpptr[Y++] = 161; 
    }

    _ms_tmpptr = _ms_dls[X];
#ifdef BIDIR_VERTICAL_SCROLLING
    Y = _ms_top_sbuffer_size;
#else
    Y = _ms_sbuffer_size & 0x7f;
#endif
    _ms_dlend[X] = Y;
    for (Y--; Y >= 0; Y--) { 
#ifdef BIDIR_VERTICAL_SCROLLING
        _ms_tmpptr[Y] = _ms_top_sbuffer[Y];
#else
        _ms_tmpptr[Y] = _ms_sbuffer[Y];
#endif
    }
}

void _ms_move_save_down()
{
    X = _ms_vscroll_coarse_offset;

#ifdef BIDIR_VERTICAL_SCROLLING
    _ms_dlend_save[X] = _ms_top_sbuffer_size;
#ifdef DMA_CHECK
    _ms_dldma_save[X] = _ms_top_sbuffer_dma;
#endif
    _ms_bottom_sbuffer_size = 0;
    _ms_bottom_sbuffer_dma = _MS_DMA_START_VALUE;
    _ms_top_sbuffer_size = 0;
    _ms_top_sbuffer_dma = _MS_DMA_START_VALUE;
    _ms_scroll_buffers_refill = MS_SCROLL_UP;
#else
    _ms_dlend_save[X] = _ms_sbuffer_size & 0x7f;
#ifdef DMA_CHECK
    _ms_dldma_save[X] = _ms_sbuffer_dma;
#endif
    _ms_sbuffer_size = 0;
    _ms_sbuffer_dma = _MS_DMA_START_VALUE;
#endif
}

void _ms_move_dlls_up()
{
    // Move the DLLs
    if (_ms_pal_detected) {
        Y = 6 + (_MS_NB_TOP_ZONES * 3);
    } else {
        Y = 3 + (_MS_NB_TOP_ZONES * 3);
    }
    if (_ms_buffer) {
        _ms_tmpptr = _ms_b1_dll;
        X = _ms_vscroll_coarse_offset + _MS_DLL_ARRAY_SIZE;
        // 16 pixel high regions
        for (_ms_tmp2 = 0; _ms_tmp2 != _MS_NB_SCROLLING_ZONES; _ms_tmp2++) {
            Y++;
            _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
            _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
            X++;
            if (X == _MS_DLL_ARRAY_SIZE + 16) X = _MS_DLL_ARRAY_SIZE;
        }
    } else {
        _ms_tmpptr = _ms_b0_dll;
        X = _ms_vscroll_coarse_offset;
        // 16 pixel high regions
        for (_ms_tmp2 = 0; _ms_tmp2 != _MS_NB_SCROLLING_ZONES; _ms_tmp2++) {
            Y++;
            _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
            _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
            X++;
            if (X == 16) X = 0;
        }
    }
    // Last zone
    Y++;
    _ms_tmpptr[Y++] = _ms_dls[X] >> 8; // High address
    _ms_tmpptr[Y++] = _ms_dls[X]; // Low address
    // Copy the scroll buffer to the last zone 
    _ms_tmpptr = _ms_dls[X];
#ifdef BIDIR_VERTICAL_SCROLLING
    Y = _ms_bottom_sbuffer_size;
#else
    Y = _ms_sbuffer_size & 0x7f;
#endif
    _ms_dlend[X] = Y;
    for (Y--; Y >= 0; Y--) { 
#ifdef BIDIR_VERTICAL_SCROLLING
        _ms_tmpptr[Y] = _ms_bottom_sbuffer[Y];
#else
        _ms_tmpptr[Y] = _ms_sbuffer[Y];
#endif
    }
}

void _ms_move_save_up()
{
    X = _ms_vscroll_coarse_offset + _MS_NB_SCROLLING_ZONES;
    if (X >= 16) X -= 16;

#ifdef BIDIR_VERTICAL_SCROLLING
    _ms_dlend_save[X] = _ms_bottom_sbuffer_size;
#ifdef DMA_CHECK
    _ms_dldma_save[X] = _ms_bottom_sbuffer_dma;
#endif
    _ms_bottom_sbuffer_size = 0;
    _ms_bottom_sbuffer_dma = _MS_DMA_START_VALUE;
    _ms_top_sbuffer_size = 0;
    _ms_top_sbuffer_dma = _MS_DMA_START_VALUE;
    _ms_scroll_buffers_refill = MS_SCROLL_DOWN;
#else
    _ms_dlend_save[X] = _ms_sbuffer_size & 0x7f;
#ifdef DMA_CHECK
    _ms_dldma_save[X] = _ms_sbuffer_dma;
#endif
    _ms_sbuffer_size = 0;
    _ms_sbuffer_dma = _MS_DMA_START_VALUE;
#endif
}
#endif

void _ms_vertical_scrolling_adjust_bottom_of_screen();
void _ms_horizontal_scrolling_visible();

// This one should obvisouly executed during VBLANK, since it modifies the DPPL/H pointers
void multisprite_flip()
{
    if (_ms_buffer) {
        // Add DL end entry on each DL
        for (X = _MS_DLL_ARRAY_SIZE * 2 - 1; X >= _MS_DLL_ARRAY_SIZE; X--) {
            _ms_tmpptr = _ms_dls[X];
            Y = _ms_dlend[X];
            _ms_tmpptr[++Y] = 0; 
        }
        _ms_buffer = 0; // 0 is the current write buffer
#ifdef DEBUG
        *BACKGRND = 0x0;
#endif
        while (!(*MSTAT < 0)); // Wait for VBLANK
        *DPPH = _ms_b1_dll >> 8; // 1 the current displayed buffer
        *DPPL = _ms_b1_dll;
#ifdef DEBUG
        *BACKGRND = 0x0f;
#endif
#ifdef HORIZONTAL_SCROLLING
        if (_ms_delayed_hscroll) {
            _ms_horizontal_scrolling_visible();
            _ms_delayed_hscroll = 0;
        }
#endif
#ifdef VERTICAL_SCROLLING
        if (_ms_delayed_vscroll) {
            if (_ms_delayed_vscroll == 1) {
                _ms_move_dlls_down();
                _ms_move_save_down();
            } else if (_ms_delayed_vscroll == 2) {
                _ms_move_dlls_up();
                _ms_move_save_up();
            }
            _ms_vertical_scrolling_adjust_bottom_of_screen();
            _ms_delayed_vscroll = 0;
        }
#endif
        // Restore saved state 
        for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
#ifdef MULTISPRITE_OVERLAY
            _ms_dlend[X] = _ms_dlend_save_overlay[X];
#else
            _ms_dlend[X] = _ms_dlend_save[X];
#endif
#ifdef DMA_CHECK
            _ms_dldma[X] = _ms_dldma_save[X];
#endif
        }
    } else {
        // Add DL end entry on each DL
        for (X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; X--) {
            _ms_tmpptr = _ms_dls[X];
            Y = _ms_dlend[X];
            _ms_tmpptr[++Y] = 0; 
        }
        _ms_buffer = 1; // 1 is the current write buffer
#ifdef DEBUG
        *BACKGRND = 0x0;
#endif
        while (!(*MSTAT < 0)); // Wait for VBLANK
        *DPPH = _ms_b0_dll >> 8; // 0 the current displayed buffer
        *DPPL = _ms_b0_dll;
#ifdef DEBUG
        *BACKGRND = 0x0f;
#endif
#ifdef HORIZONTAL_SCROLLING
        if (_ms_delayed_hscroll) {
            _ms_horizontal_scrolling_visible();
            _ms_delayed_hscroll = 0;
        }
#endif
#ifdef VERTICAL_SCROLLING
        if (_ms_delayed_vscroll) {
            if (_ms_delayed_vscroll == 1) {
                _ms_move_dlls_down();
                _ms_move_save_down();
            } else if (_ms_delayed_vscroll == 2) {
                _ms_move_dlls_up();
                _ms_move_save_up();
            }
            _ms_vertical_scrolling_adjust_bottom_of_screen();
            _ms_delayed_vscroll = 0;
        }
#endif
        // Restore saved state 
        for (Y = _MS_DLL_ARRAY_SIZE * 2 - 1, X = _MS_DLL_ARRAY_SIZE - 1; X >= 0; Y--, X--) {
#ifdef MULTISPRITE_OVERLAY
            _ms_dlend[Y] = _ms_dlend_save_overlay[Y];
#else
            _ms_dlend[Y] = _ms_dlend_save[X];
#endif
#ifdef DMA_CHECK
            _ms_dldma[Y] = _ms_dldma_save[X];
#endif
        }
    }
}

#ifdef VERTICAL_SCROLLING

// Vertical scrolling
#define multisprite_vertical_scrolling(x) { _ms_tmp = (x); _ms_vertical_scrolling(); }

void _ms_vertical_scrolling_adjust_bottom_of_screen()
{
    X = _ms_vscroll_coarse_offset + _MS_NB_SCROLLING_ZONES;
    if (X >= 16) X -= 16;

    if (_ms_buffer) {
        _ms_tmpptr = _ms_b1_dll;
        X += _MS_DLL_ARRAY_SIZE;
    } else {
        _ms_tmpptr = _ms_b0_dll;
    }
    if (_ms_pal_detected) { Y = 6 + 3 * _MS_NB_TOP_ZONES; } else { Y = 3 + 3 * _MS_NB_TOP_ZONES; }
    _ms_tmpptr[Y] = (_ms_tmpptr[Y] & 0xf0) | (15 - _ms_vscroll_fine_offset); // 16 - _ms_vscroll_fine_offset lines
    Y +=  3 * _MS_NB_SCROLLING_ZONES;
    if (_ms_vscroll_fine_offset) {
        _ms_tmpptr[Y] = (_ms_tmpptr[Y] & 0xf0) | 0x0f; // 16 lines
        _ms_tmpptr2 = _ms_dls[X] - 17; // Point last line to prepended DMA masking object
        _ms_tmpptr[++Y] = _ms_tmpptr2 >> 8;
        _ms_tmpptr[++Y] = _ms_tmpptr2;
        ++Y;
        _ms_tmpptr[Y] = (_ms_tmpptr[Y] & 0xf0) | _ms_vscroll_fine_offset;  // _ms_vscroll_fine_offset + 1 lines
        _ms_tmp = 0xa0 | _ms_vscroll_fine_offset;
        _ms_tmpptr2[Y = 2] = _ms_tmp;
        _ms_tmpptr2[Y = 7] = _ms_tmp;
        _ms_tmpptr2[Y = 11] = _ms_tmp;
        _ms_tmpptr2[Y = 15] = _ms_tmp;
    } else {
        _ms_tmpptr[Y] &= 0xf0; // 1 line
        _ms_tmpptr[++Y] = _ms_blank_dl >> 8;
        _ms_tmpptr[++Y] = _ms_blank_dl;
        ++Y;
        _ms_tmpptr[Y] = (_ms_tmpptr[Y] & 0xf0) | 0x0f;  // 16 lines
    }
}

void _ms_vertical_scrolling()
{
    _ms_vscroll_fine_offset -= _ms_tmp;
    if (_ms_vscroll_fine_offset < 0) {
        _ms_vscroll_coarse_offset--; 
        if (_ms_vscroll_coarse_offset < 0)
            _ms_vscroll_coarse_offset = _MS_DLL_ARRAY_SIZE - 1;
        _ms_vscroll_coarse_offset_shifted = _ms_vscroll_coarse_offset << 3; 
        _ms_move_dlls_down();
        _ms_vscroll_fine_offset += 16;
        _ms_delayed_vscroll = 1;
    } else if (_ms_vscroll_fine_offset >= 16) {
        // TODO: Check the current last zone and remove the DMA masking object
        _ms_vscroll_coarse_offset++; 
        if (_ms_vscroll_coarse_offset == _MS_DLL_ARRAY_SIZE)
            _ms_vscroll_coarse_offset = 0;
        _ms_vscroll_coarse_offset_shifted = _ms_vscroll_coarse_offset << 3; 
        _ms_move_dlls_up();
        _ms_vscroll_fine_offset -= 16;
        _ms_delayed_vscroll = 2;
    } else {
        _ms_delayed_vscroll = 3;
    }
    _ms_vertical_scrolling_adjust_bottom_of_screen();
}
#endif

#ifdef HORIZONTAL_SCROLLING
// Horizontal scrolling
#define multisprite_horizontal_tiles_scrolling(offset, y, value) \
    _ms_hscroll = (value); \
    X = (y); \
    if (_ms_buffer) X += _MS_DLL_ARRAY_SIZE; \
    _ms_tmpptr = _ms_dls[X]; \
    Y = (offset) + 4; \
    if (Y < _ms_dlend[X]) { \
        _ms_horizontal_tiles_scrolling(); \
    }

void _ms_horizontal_tiles_scrolling()
{
    _ms_tmp = _ms_tmpptr[Y] - _ms_hscroll; // X position
    if (_ms_hscroll > 0) {
        if (_ms_tmp > 160 && _ms_tmp < 256 - 8) {
            // We have reached the left border of the screen
            // Advance the pointer
            _ms_tmpptr[Y] = _ms_tmp + 8;
#ifdef TILES_CHECK_BOUNDARIES
            Y -= 4;
            _ms_tmpptr2 = _ms_tmpptr[Y];
            Y++; Y++;
            _ms_tmpptr2 |= (_ms_tmpptr[Y] << 8);
            Y++;
            // Get the width of this tileset
            _ms_tmp = Y;
            Y = -(_ms_tmpptr[Y] | 0xe0);
            if (_ms_tmpptr2[Y] == 0xff) { // This is a boundary
                // Reduce the width of this string
                X = --Y;
                Y = _ms_tmp;
                X = (-X) & 0x1f;
                _ms_tmpptr[Y] = _ms_tmpptr[Y] & 0xe0 | X; 
            }
            Y = _ms_tmp;
            _ms_tmpptr2++;
            _ms_tmpptr[--Y] = _ms_tmpptr2 >> 8;
            Y--; Y--;
            _ms_tmpptr[Y] = _ms_tmpptr2;
#else 
            Y -= 4;
            _ms_tmpptr[Y]++;
            if (_ms_tmpptr[Y] == 0) {
                // Crossing page boundary
                Y++; Y++;
                _ms_tmpptr[Y]++;
            }
#endif
        } else {
            _ms_tmpptr[Y] = _ms_tmp;
#ifdef TILES_CHECK_BOUNDARIES
            // TODO : Check if we we should expand the width of the tileset
            // If it touches the right border of the screen
#endif
        }
    } else {
        _ms_tmpptr[Y] = _ms_tmp;
        if (_ms_tmp >= 0) {
#ifdef TILES_CHECK_BOUNDARIES
            // TODO: If the tileset touches the right side of the screen, reduce its width...
            // TODO: Do below only if one touches the left border of the screen
            X = _ms_tmp;
            Y -= 4;
            _ms_tmpptr2 = _ms_tmpptr[Y];
            Y++; Y++;
            _ms_tmpptr2 |= (_ms_tmpptr[Y] << 8);
            _ms_tmpptr2--;
            _ms_tmp = Y;
            if (_ms_tmpptr2[Y = 0] == 0xff) { // This is a boundary
                _ms_tmpptr2++; // Go back to previous pointer
            } else {
                // Extend the width up to 21
                Y = _ms_tmp;
                Y++; Y++;
                _ms_tmpptr[Y] = X - 8;
                Y--;
                X = -(_ms_tmpptr[Y] | 0xe0);
                if (X < 21) {
                    X++;
                    X = (-X) & 0x1f;
                    _ms_tmpptr[Y] = _ms_tmpptr[Y] & 0xe0 | X; 
                }
            }
            Y = _ms_tmp;
            _ms_tmpptr[Y] = _ms_tmpptr2 >> 8;
            Y--; Y--;
            _ms_tmpptr[Y] = _ms_tmpptr2;
#else
            _ms_tmpptr[Y] = _ms_tmp - 8;
            Y -= 4;
            _ms_tmpptr[Y]--;
            if (_ms_tmpptr[Y] == 0xff) {
                // Crossing page boundary
                Y++; Y++;
                _ms_tmpptr[Y]--;
            }
#endif
        }
    } 
}

#define multisprite_horizontal_scrolling(x) _ms_delayed_hscroll = (x); _ms_horizontal_scrolling() 

void _ms_horizontal_scrolling_visible()
{
    for (_ms_tmp2 = 0; _ms_tmp2 != _MS_DLL_ARRAY_SIZE; _ms_tmp2++) {
        multisprite_horizontal_tiles_scrolling(0, _ms_tmp2, _ms_delayed_hscroll);
    }
}

void _ms_horizontal_scrolling()
{
    for (_ms_tmp2 = 0; _ms_tmp2 != _MS_DLL_ARRAY_SIZE; _ms_tmp2++) {
        multisprite_horizontal_tiles_scrolling(0, _ms_tmp2, _ms_delayed_hscroll);
    }
#ifdef BIDIR_VERTICAL_SCROLLING
    // Scroll also the preloaded scrolling bands
    _ms_tmpptr = _ms_top_sbuffer;
    Y = 4; 
    _ms_horizontal_tiles_scrolling();
    _ms_tmpptr = _ms_bottom_sbuffer;
    Y = 4; 
    _ms_horizontal_tiles_scrolling();
#endif
}

#endif

#define _ms_width_palette(width, palette) (((-(width)) & 0x1f) | ((palette) << 5))

INIT_BANK void multisprite_set_top_zone_dl(char line, char *dl)
{
    line = (line << 2) - line + 4; // line = line * 3 + 4
    if (_ms_pal_detected) line += 3;
    _ms_b0_dll[X = line] = dl >> 8;
    _ms_b1_dll[X] = dl >> 8;
    _ms_b0_dll[++X] = dl;
    _ms_b1_dll[X] = dl;
}

// _ms_tmp : display list entry to apply DLI flag
void _multisprite_enable_dli()
{
    _ms_tmp = (_ms_tmp << 2) - _ms_tmp + 3; // _ms_tmp = _ms_tmp * 3 + 3
    if (_ms_pal_detected) _ms_tmp += 3;
    _ms_b0_dll[X = _ms_tmp] |= 0x80;
    _ms_b1_dll[X] |= 0x80;
}

// _ms_tmp : display list entry to apply DLI flag
void _multisprite_disable_dli()
{
    _ms_tmp = (_ms_tmp << 2) - _ms_tmp + 3; // _ms_tmp = _ms_tmp * 3 + 3
    if (_ms_pal_detected) _ms_tmp += 3;
    _ms_b0_dll[X = _ms_tmp] &= 0x7f;
    _ms_b1_dll[X] &= 0x7f;
}

const char _ms_bit_extract[8] = {128, 64, 32, 16, 8, 4, 2, 1};

// ~100 cycles max pixel accurate collision detection (60us)
#define multisprite_compute_collision(x1, y1, w1, h1, x2, y2, w2, h2, collision_map) {\
    _ms_tmp3 = 0; \
    _ms_tmp2 = (y1) + ((h1) - 1) - (y2); \
    if (_ms_tmp2 >= 0) { \
        if ((x1) <= (x2) + ((w2) - 1)) { \
            if ((y1) <= (y2) + ((h2) - 1)) { \
                _ms_tmp = (x1) + ((w1) - 1) - (x2); \
                if (_ms_tmp >= 0) { \
                    Y = _ms_tmp2 << ((w1 + w2 - 1) / 8); \
                    while (_ms_tmp >= 8) { \
                        Y++; \
                        _ms_tmp -= 8; \
                    } \
                    _ms_tmp3 = collision_map[Y] & _ms_bit_extract[X = _ms_tmp]; \
                } \
            } \
        } \
    } \
}

// 320 pixels resolution collision function
// x1 and x2 are 160 pixels resolution (always)
// w1 and w2 are 320 pixels resolution
#define multisprite_compute_collision_320(x1, y1, w1, h1, x2, y2, w2, h2, collision_map) {\
    _ms_tmp3 = 0; \
    _ms_tmp2 = (y1) + ((h1) - 1) - (y2); \
    if (_ms_tmp2 >= 0) { \
        if ((x1) <= (x2) + (((w2) >> 1) - 1)) { \
            if ((y1) <= (y2) + ((h2) - 1)) { \
                _ms_tmp = (x1) + (((w1) >> 1) - 1) - (x2); \
                if (_ms_tmp >= 0) { \
                    Y = _ms_tmp2 << ((w1 + w2 - 1) / 8); \
                    while (_ms_tmp >= 8) { \
                        Y++; \
                        _ms_tmp -= 8; \
                    } \
                    _ms_tmp3 = collision_map[Y] & _ms_bit_extract[X = _ms_tmp]; \
                } \
            } \
        } \
    } \
}

#define multisprite_compute_box_collision(x1, y1, w1, h1, x2, y2, w2, h2) {\
    _ms_tmp3 = 0; \
    if ((y1) + ((h1) - 1) >= (y2)) { \
        if ((x1) <= (x2) + ((w2) - 1)) { \
            if ((y1) <= (y2) + ((h2) - 1)) { \
                if ((x1) + ((w1) - 1) >= (x2)) { \
                    _ms_tmp3 = 1; \
                } \
            } \
        } \
    } \
}

#define multisprite_collision_detected (_ms_tmp3)

#define multisprite_sparse_tiling(ptr, line, top, left, height) \
    _ms_sparse_tiles_ptr_high = ptr[Y = 0]; \
    _ms_sparse_tiles_ptr_low = ptr[Y = 1]; \
    _ms_sparse_tiling(line, top, left, height);

// Sparse tiling simple display
void _ms_sparse_tiling(char line, char top, char left, char height)
{
    char *ptr, data[5], y, bottom;
    _ms_tmp2 = line;

    bottom = top + height;
    if (_ms_buffer) {
        top += _MS_DLL_ARRAY_SIZE; 
        bottom += _MS_DLL_ARRAY_SIZE;
    }

    for (X = top; X < bottom; _ms_tmp2++) {
        Y = _ms_tmp2;
        ptr = _ms_sparse_tiles_ptr_low[Y] | (_ms_sparse_tiles_ptr_high[Y] << 8);   
        _ms_tmpptr = _ms_dls[X];
        Y = 1;
        y = _ms_dlend[X];
        data[4] = ptr[Y];
        while (data[4] != 0xff) {
            data[0] = ptr[++Y];
            data[1] = ptr[++Y];
            data[2] = ptr[++Y];
            data[3] = ptr[++Y];
#ifdef DMA_CHECK 
            _ms_dldma[X] -= ptr[++Y]; // 18 cycles
#else
            ++Y;
#endif
            _save_y = Y;
            Y = y; // 6 cycles
            _ms_tmpptr[Y++] = data[0]; // 11 cycles
            _ms_tmpptr[Y++] = data[1];
            _ms_tmpptr[Y++] = data[2];
            _ms_tmpptr[Y++] = data[3];
            _ms_tmpptr[Y++] = (data[4] << 3) + left;
            y = Y; // 21 cycles
            Y = _save_y;
            Y++;
            data[4] = ptr[++Y];
        } 
        _ms_dlend[X++] = y;
    }
}

char multisprite_sparse_tiling_collision(char top, char left, char right)
{
    char *ptr, start, end, intersect = -1;
    Y = top;
    ptr = _ms_sparse_tiles_ptr_low[Y] | (_ms_sparse_tiles_ptr_high[Y] << 8);
    // Find the first possibly intersecting tileset on this line
    Y = 0;
    while (ptr[Y] < left) Y += 7;
    // This one possibly intersects
    end = ptr[Y++];
    start = ptr[Y++];
    while (right >= start) {
        char l = (left < start)?start:left;
        char r = (end < right)?end:right;
        char n = r - l;
        if (n < 0) break;
        char tmp = ptr[Y++];
        char tmp2 = ptr[Y++];
        if (!(tmp2 & 0x20)) { // Direct mode. We don't care about the content
            return 0;
        } 
        char *ptr_tiles = tmp | (ptr[Y] << 8);
        _save_y = Y;
        Y = l - start;
        if (tmp2 < 0) { // WM = 1: 2 entries per tile
            X = n << 1;
        } else {
            X = n;
        }
        for (; X >= 0; Y++, X--) {
            if (ptr_tiles[Y] < intersect) intersect = ptr_tiles[Y];    
        }
        Y = _save_y;
        Y += 3;
        end = ptr[Y++];
        start = ptr[Y++];
    }
    return intersect;
}

#define multisprite_display_bitmap(ptr, left, top, height) \
    _ms_sparse_tiles_ptr_high = ptr[Y = 0]; \
    _ms_sparse_tiles_ptr_low = ptr[Y = 1]; \
    _ms_display_bitmap(left, top, height);

// Sparse tiling simple display
void _ms_display_bitmap(char left, char top, char height)
{
    char *ptr, data[5], y, bottom;
    _ms_tmp2 = 0;

    bottom = top + height;
    if (_ms_buffer) {
        top += _MS_DLL_ARRAY_SIZE; 
        bottom += _MS_DLL_ARRAY_SIZE;
    }

    for (X = top; X < bottom; _ms_tmp2++) {
        Y = _ms_tmp2;
        ptr = _ms_sparse_tiles_ptr_low[Y] | (_ms_sparse_tiles_ptr_high[Y] << 8);   
        _ms_tmpptr = _ms_dls[X];
        y = _ms_dlend[X];
        Y = 0;
        // First 5 bytes serie (setting the mode)
        data[0] = ptr[Y++];
        data[1] = ptr[Y++];
        data[2] = ptr[Y++];
        data[3] = ptr[Y++];
        data[4] = ptr[Y++];
        _save_y = Y;
        Y = y; // 6 cycles
        _ms_tmpptr[Y++] = data[0]; // 11 cycles
        _ms_tmpptr[Y++] = data[1];
        _ms_tmpptr[Y++] = data[2];
        _ms_tmpptr[Y++] = data[3];
        _ms_tmpptr[Y++] = data[4] + left;
        y = Y; // 21 cycles
        Y = _save_y;
        // The other series of 4 bytes
        data[0] = ptr[Y++];
        data[1] = ptr[Y++];
        while (data[1]) {
            data[2] = ptr[Y++];
            data[3] = ptr[Y++];
            _save_y = Y;
            Y = y; // 6 cycles
            _ms_tmpptr[Y++] = data[0]; // 11 cycles
            _ms_tmpptr[Y++] = data[1];
            _ms_tmpptr[Y++] = data[2];
            _ms_tmpptr[Y++] = data[3] + left;
            y = Y; // 21 cycles
            Y = _save_y;
            data[0] = ptr[Y++];
            data[1] = ptr[Y++];
        }
        _ms_tmpptr[++Y] = 0;
        _ms_dlend[X++] = y;
    }
}

void multisprite_disable_holeydma()
{
    if (_ms_pal_detected) Y = 3; else Y = 0;
    for (X = 0; X != 14; X++) {
        Y++; Y++; Y++;
        _ms_b0_dll[Y] &= 0x9f;
        _ms_b1_dll[Y] &= 0x9f;
    }
}

void multisprite_enable_holeydma()
{
    if (_ms_pal_detected) Y = 3; else Y = 0;
    for (X = 0; X != 14; X++) {
        Y++; Y++; Y++;
        if (X >= _MS_TOP_DISPLAY) {
            _ms_b0_dll[Y] |= 0x40;
            _ms_b1_dll[Y] |= 0x40;
        }
    }
}

#endif // __ATARI7800_MULTISPRITE__
