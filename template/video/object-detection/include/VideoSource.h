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

#ifndef VIDEO_SOURCE_H_
#define VIDEO_SOURCE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t open_img_source(const uint32_t idx);
void close_img_source(const uint32_t idx);
const char* get_img_name(const uint32_t idx);
const uint8_t* get_img_array(const uint32_t idx);
uint32_t get_img_array_size(const uint32_t idx);
void set_img_object_box(const uint32_t idx, const uint32_t x0,
                                            const uint32_t y0,
                                            const uint32_t w,
                                            const uint32_t h);

#ifdef __cplusplus
}
#endif

#endif /* VIDEO_SOURCE_H_ */