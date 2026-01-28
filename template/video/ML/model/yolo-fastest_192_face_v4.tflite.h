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

#ifndef YOLO_FASTEST_192_FACE_V4_TFLITE_H_
#define YOLO_FASTEST_192_FACE_V4_TFLITE_H_

#include <stdint.h>
#include <stddef.h>

/**
  Model configuration parameters.
*/
typedef struct {
    int originalImageSize;        // Original input image size (assumed square)
    int channelsImageDisplayed;   // 
    const float *anchor1;         // Pointer to first anchor array
    const float *anchor2;         // Pointer to second anchor array
    int numAnchors1;              // Number of elements in anchor1
    int numAnchors2;              // Number of elements in anchor2
} ModelConfig;

#ifdef __cplusplus
extern "C"  {
#endif


/**
  Get a pointer to the model data.
*/
const uint8_t *GetModelPointer(void);

/**
  Get the length of the model data.
*/
size_t GetModelLen(void);

/*
  Get the model configuration parameters.
*/
const ModelConfig *GetModelConfig(void);


#ifdef __cplusplus
}
#endif

#endif /* YOLO_FASTEST_192_FACE_V4_TFLITE_H_ */