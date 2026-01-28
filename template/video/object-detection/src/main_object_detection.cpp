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

#include "BufAttributes.h" /* Buffer attributes to be applied */
#include "DetectionResult.hpp"
#include "DetectorPostProcessing.hpp" /* Post Process */
#include "DetectorPreProcessing.hpp"  /* Pre Process */
#include "VideoSource.h"
#include "YoloFastestModel.hpp"       /* Model API */
#include "yolo-fastest_192_face_v4.tflite.h" /* Model data */

#include "cmsis_os2.h"                /* ::CMSIS:RTOS2 */
#include "main.h"

using namespace arm::app;

/* Tensor arena buffer */
static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

/* Model object */
YoloFastestModel model;

/* Tensors and shapes */
TfLiteTensor   *inputTensor;
TfLiteTensor   *outputTensor0;
TfLiteTensor   *outputTensor1;
TfLiteIntArray *inputShape;

/* Input image dimensions */
int inputImgCols;
int inputImgRows;

/* Object to hold detection results */
std::vector<object_detection::DetectionResult> results;

/* Post-processing parameters */
object_detection::PostProcessParams postProcessParams;

/* Pre and Post processing objects */
DetectorPreProcess  *preProcess  = nullptr;
DetectorPostProcess *postProcess = nullptr;


void app_main_thread(void *arg) {
  const uint8_t *img_buf;
  uint32_t img_idx;
  size_t   img_sz;

  ModelConfig modelConfig = *GetModelConfig();

  /* Initialize model object */
  if (!model.Init(tensorArena, sizeof(tensorArena), GetModelPointer(), GetModelLen())) {
    printf("Failed to initialise model\n");
    return;
  }

  inputTensor   = model.GetInputTensor(0);
  outputTensor0 = model.GetOutputTensor(0);
  outputTensor1 = model.GetOutputTensor(1);

  /* Get input shape dimensions */
  inputShape = model.GetInputShape(0);
  inputImgCols = inputShape->data[YoloFastestModel::ms_inputColsIdx];
  inputImgRows = inputShape->data[YoloFastestModel::ms_inputRowsIdx];

  /* Set up pre and post-processing. */
  preProcess = new DetectorPreProcess(inputTensor, true, model.IsDataSigned());

  postProcessParams = {
    .inputImgRows        = inputImgRows,
    .inputImgCols        = inputImgCols,
    .originalImageSize   = modelConfig.originalImageSize,
    .anchor1             = modelConfig.anchor1,
    .anchor2             = modelConfig.anchor2
  };

  postProcess = new DetectorPostProcess(outputTensor0, outputTensor1, results, postProcessParams);

  img_idx = 0;

  while (open_img_source(img_idx)) {
    results.clear();

    /* Input buffer is the image frame */
    img_buf = get_img_array(img_idx);
    img_sz  = get_img_array_size(img_idx);

    /* Run the pre-processing, inference and post-processing. */
    if (!preProcess->DoPreProcess(img_buf, img_sz)) {
      printf("Pre-processing failed.\n");
      return;
    }

    printf("Image %d: ", img_idx);

    /* Run inference over this image. */
    if (!model.RunInference()) {
      printf("Inference failed.\n");
      return;
    }

    if (!postProcess->DoPostProcess()) {
      printf("Post-processing failed.\n");
      return;
    }

    /* Check the detection results */
    if (results.empty()) {
      printf("No object detected\n");
    }
    else {
      printf("Detected objects ");

      for (const auto& result : results) {
        /* Set object detection box to the image */
        set_img_object_box(img_idx, result.m_x0, result.m_y0, result.m_w, result.m_h);

        /* Sent detection coordinates to the console */
        printf(":: [x=%d, y=%d, w=%d, h=%d]", result.m_x0, result.m_y0, result.m_w, result.m_h);
      }
      printf("\n");
    }

    close_img_source(img_idx++);
  }
}

/* Application initialization */
int app_main (void) {
  const osThreadAttr_t attr = {
    .stack_size = 4096U
  };

  /* Initialize CMSIS-RTOS2, create application thread and start the kernel */
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &attr);
  osKernelStart();
  return 0;
}
