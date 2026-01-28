/*
 * Copyright 2026 Arm Limited and/or its affiliates.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <stdint.h>

#include "VideoSource.h"

#define NUMBER_OF_FILES (1U)

extern const uint8_t image0[];

static const char* img_names[] = {
  "sample_image.png"
};

static const uint8_t* img_arrays[] = {
  image0
};

static const size_t img_array_sizes[NUMBER_OF_FILES] = {
  110592U,
};

uint32_t open_img_source(const uint32_t idx) {
  if(idx < NUMBER_OF_FILES) {
    return 1U;
  }
  return 0U;
}

void close_img_source(const uint32_t idx) {
  (void)idx;
}

const char* get_img_name(const uint32_t idx) {
  if (idx < NUMBER_OF_FILES) {
    return img_names[idx];
  }
  return NULL;
}

const uint8_t* get_img_array(const uint32_t idx) {
  if (idx < NUMBER_OF_FILES) {
    return img_arrays[idx];
  }
  return NULL;
}

uint32_t get_img_array_size(const uint32_t idx) {
  /* Return image array size in bytes */
  if (idx < NUMBER_OF_FILES) {
    return img_array_sizes[idx];
  }
  return 0U;
}

void set_img_object_box(const uint32_t idx, const uint32_t x0, const uint32_t y0, const uint32_t w, const uint32_t h) {
  (void)idx;
  (void)x0;
  (void)y0;
  (void)w;
  (void)h;
}