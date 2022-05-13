/**
 * Copyright (c) 2020 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include "../utils/utils.h"

#define BASE 64
#define LAST_BASE_INDEX BASE - 1
#define LOG_BASE 6

#define ROW_TYPE uint64_t

#define SWAP_WITHIN_BYTES(row_1, row_2, shift, mask)\
 do {\
   ROW_TYPE temp = ((row_1) ^ ((row_2) >> (shift))) & (mask);\
    (row_1) = (row_1) ^ (temp);\
     (row_2) = (row_2) ^ ((temp) << (shift));\
 } while(0)

#define SWAP_BYTES(row_1, row_2, shift, mask)\
 do {\
 ROW_TYPE temp = ((row_1) ^ ((row_2) << (shift))) & (mask);\
  (row_1) = (row_1) ^ (temp);\
   (row_2) = (row_2) ^ ((temp) >> (shift));\
   } while(0)

static void transpose_64(uint64_t *img) {
  // Transposes the 64x64 bit image by transposing submatrices of the image inward
 
  ROW_TYPE mask = 0xFFFFFFFF00000000;

  int shift = BASE >> 1, k, index_for_swap;
  
  // Handles the swaps for >= 8 bits (1 byte), to account for little endianness of machine
  while (shift != 4) {
    for (k = 0; k < BASE; k += shift<<1) {
      for (index_for_swap = k; index_for_swap < shift + k; index_for_swap++) {
        SWAP_BYTES(*(img + (index_for_swap + shift)), *(img + index_for_swap), shift, mask);
      } 
    }
    shift >>= 1;
    mask ^= mask >> shift;
  }

  mask >>= shift;
  // Swaps within single bytes, where endianness does not matter
  while (shift != 0) {
    for (k = 0; k < BASE; k += shift<<1) {
       for (index_for_swap = k; index_for_swap < shift + k; index_for_swap++) {
        SWAP_WITHIN_BYTES(*(img + (index_for_swap + shift)), *(img + index_for_swap), shift, mask);
      }
    }
    shift >>= 1;
    mask ^= mask << shift;
  }

}

// Rotates a bit array clockwise 90 degrees.
//
// The bit array is of `N` by `N` bits where N is a multiple of 64
void rotate_bit_matrix(uint8_t* restrict img, const bits_t N) {

  // just changing to pointer to achieve larger rows
  ROW_TYPE* img_64 = (ROW_TYPE*) img;

  // in these, we store BASExBASE blocks that need to be
  // rotated and cyclicly swapped
  ROW_TYPE block_1[BASE]; 
  ROW_TYPE block_2[BASE]; 
  ROW_TYPE block_3[BASE]; 
  ROW_TYPE block_4[BASE];

  // each of these corresponds to a pointer to the first
  // row of each of the blocks we want to rotate/swap
  ROW_TYPE *block_1_img_pointer;
  ROW_TYPE *block_2_img_pointer;
  ROW_TYPE *block_3_img_pointer;
  ROW_TYPE *block_4_img_pointer;
  const bits_t size = N >> LOG_BASE;

  // these are the pointers to reset the above ones
  // after the second loop
  ROW_TYPE* new_blocks_row_pointer_1 = img_64;
  ROW_TYPE* new_blocks_row_pointer_2 = img_64 + size - 1;
  ROW_TYPE* new_blocks_row_pointer_3 = img_64 + (N - BASE) * size + size - 1;
  ROW_TYPE* new_blocks_row_pointer_4 = img_64 + (N - BASE) * size;

  for(int i = 0; i < (size + 1) / 2; i++) { // looping through blocks of rows

      block_1_img_pointer = new_blocks_row_pointer_1;
      block_2_img_pointer = new_blocks_row_pointer_2;
      block_3_img_pointer = new_blocks_row_pointer_3;
      block_4_img_pointer = new_blocks_row_pointer_4;

  // changing which row of block we are focusing on
      new_blocks_row_pointer_1 += N;
      new_blocks_row_pointer_2--;
      new_blocks_row_pointer_3 -= N;
      new_blocks_row_pointer_4++;

    for(int j = 0; j < (size / 2); j++) {

      for(int k = 0; k < BASE; ++k){ // filling up each block
        block_1[k] = *(block_1_img_pointer + size * k);
        block_2[k] = *(block_2_img_pointer + size * k);
        block_3[k] = *(block_3_img_pointer + size * k);
        block_4[k] = *(block_4_img_pointer + size * k);
      }
      
      transpose_64(block_1);
      transpose_64(block_2);
      transpose_64(block_3);
      transpose_64(block_4);

      for(int k = 0; k < BASE; ++k) { 
        // putting blocks back in reverse order after transpose to achieve rotation
        *(block_2_img_pointer + size * k) = block_1[LAST_BASE_INDEX - k];
        *(block_3_img_pointer + size * k) = block_2[LAST_BASE_INDEX - k];
        *(block_4_img_pointer + size * k) = block_3[LAST_BASE_INDEX - k];
        *(block_1_img_pointer + size * k) = block_4[LAST_BASE_INDEX - k];
      }
      
      // changing which block in the block row we are focusing on
      block_1_img_pointer += 1;
      block_2_img_pointer += N;
      block_3_img_pointer -= 1;
      block_4_img_pointer -= N;
    }
  }
  if(size & 1) {
    ROW_TYPE column_index = size >> 1;
    ROW_TYPE row_index = (size >> 1) << LOG_BASE;
    block_1_img_pointer = img_64 + row_index * size + column_index;

    for(int k = 0; k < BASE; ++k) {
      block_1[k] = *(block_1_img_pointer + size * k);
    }
    
    transpose_64(block_1);

    for(int k = 0; k < BASE; ++k) {
      *(block_1_img_pointer + size * k) = block_1[LAST_BASE_INDEX - k];
    }
  }

  return;
}
