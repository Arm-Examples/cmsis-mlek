/*
 * Copyright 2025-2026 Arm Limited and/or its affiliates.
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

#include <cstdint>
#include <string>
#include <vector>

#include "AudioUtils.hpp"
#include "AudioSource.h"        /* Interface to audio data array */
#include "BufAttributes.h"      /* Buffer attributes to be applied */

#include "KwsProcessing.hpp"    /* Pre and Post Process */
#include "KwsResult.hpp"        /* KWS results class */

#include "MicroNetKwsModel.hpp" /* Model API */
#include "kws_micronet_m.tflite.h"

#include "cmsis_os2.h"          /* CMSIS-RTOS2 API */
#include "main.h"

using namespace arm::app;

/* Tensor arena buffer */
static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

/* Model object */
MicroNetKwsModel model;

/* Tensors and shapes */
TfLiteTensor   *inputTensor;
TfLiteTensor   *outputTensor;
TfLiteIntArray *inputShape;

/* MFCC feature extraction parameters */
uint32_t numMfccFeatures;
uint32_t numMfccFrames;

/* Classifier object */
KwsClassifier classifier;

/* Object to hold classification results (single inference) */
std::vector<ClassificationResult> singleInfResult;

/* Object to hold results (from across the whole audio clip) */
std::vector<kws::KwsResult> results;

/* Object to hold label strings. */
std::vector<std::string> labels;

/* Pre and Post processing objects */
KwsPreProcess  *preProcess  = nullptr;
KwsPostProcess *postProcess = nullptr;

/* Words that can be recognized */
static const char* labelsVec[] = {
  "down",
  "go",
  "left",
  "no",
  "off",
  "on",
  "right",
  "stop",
  "up",
  "yes",
  "_silence_",
  "_unknown_",
};

void app_main_thread(void *arg) {
  const ModelConfig *modelConfig;
  uint32_t file_idx;

  /* Initialize model object */
  if (!model.Init(tensorArena, sizeof(tensorArena), GetModelPointer(), GetModelLen())) {
    printf("Failed to initialise model\n");
    return;
  }

  /* Get Input and Output tensors for pre/post processing. */
  inputTensor  = model.GetInputTensor(0);
  outputTensor = model.GetOutputTensor(0);

  /* Get input shape for feature extraction. */
  inputShape = model.GetInputShape(0);
  numMfccFeatures = inputShape->data[MicroNetKwsModel::ms_inputColsIdx];
  numMfccFrames   = inputShape->data[MicroNetKwsModel::ms_inputRowsIdx];

  /* We expect to be sampling 1 second worth of data at a time.
   * NOTE: This is only used for time stamp calculation. */
  const float secondsPerSample = 1.0 / audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;

  /* Populate the labels here. */
  labels.assign(std::begin(labelsVec), std::end(labelsVec));

  /* Get model training configuration */
  modelConfig = GetModelConfig();

  /* Set up pre and post-processing. */
  preProcess = new KwsPreProcess(inputTensor, numMfccFeatures, numMfccFrames, modelConfig->mfccFrameLength, modelConfig->mfccFrameStride);
  postProcess = new KwsPostProcess(outputTensor, classifier, labels, singleInfResult);

  file_idx = 0;
  uint32_t inferenceCount = 0;
  std::string lastValidKeywordDetected;

  while (open_audio_source(file_idx)) {
    /* Initialize results and last detected keyword */
    results.clear();
    lastValidKeywordDetected.clear();

    /* Creating a sliding window through the whole audio clip. */
    auto audioDataSlider = audio::SlidingWindow<const int16_t>(get_audio_array(file_idx),
                                                               get_audio_array_size(file_idx),
                                                               preProcess->m_audioDataWindowSize,
                                                               preProcess->m_audioDataStride);

    /* Reset sliding window position */
    audioDataSlider.Reset();

    while (audioDataSlider.HasNext()) {
      /* Set a pointer to the current chunk of audio data */
      const int16_t *dataWindow = audioDataSlider.Next();

      /* Run the pre-processing, inference and post-processing. */
      if (!preProcess->DoPreProcess(dataWindow, audioDataSlider.Index())) {
        printf("Pre-processing failed.");
        return;
      }

      printf("Inference #: %d\n", ++inferenceCount);
      if (!model.RunInference()) {
        printf("Inference failed.");
        return;
      }

      if (!postProcess->DoPostProcess()) {
        printf("Post-processing failed.");
        return;
      }

      /* Get frame index and timestamp for the current inference window */
      const auto frameIndex = audioDataSlider.Index();
      const auto timestamp = frameIndex * secondsPerSample * preProcess->m_audioDataStride;

      /* Add current inference result to final results vector */
      results.emplace_back(singleInfResult, timestamp, frameIndex, modelConfig->detectionThreshold);
    }

    for (const auto& result : results) {
      if (!result.m_resultVec.empty()) {
        /* Result vector is not empty, get the top result */
        const auto& topResult = result.m_resultVec[0];
        const std::string& topKeyword = topResult.m_label;
        const float score = topResult.m_normalisedVal;

        if (topKeyword != "<none>" && topKeyword != "_unknown_" && topKeyword != lastValidKeywordDetected) {
          /* Update last keyword. */
          lastValidKeywordDetected = topKeyword;
          printf("Detected: %s; Prob: %0.2f\n", topKeyword.c_str(), score);
        }
      }
    }

    close_audio_source(file_idx++);
  }
}

/* Application initialization */
int app_main (void) {
  const osThreadAttr_t attr = {
    .stack_size = 4096,
  };

  /* Initialize CMSIS-RTOS2, create application thread and start the kernel */
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &attr);
  osKernelStart();
  return 0;
}
