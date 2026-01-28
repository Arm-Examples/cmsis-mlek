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

#include <stdint.h>
#include <string.h>

#include "AudioSource.h"
#include "cmsis_vstream.h"
#include "cmsis_os2.h"

#include "arm_math.h"

/* Define stereo (audio in) and mono (audio for inference) buffers */
#define STEREO_BLOCK_COUNT   (2)
#define STEREO_BLOCK_SAMPLES (16000)
#define STEREO_BLOCK_SIZE    (STEREO_BLOCK_SAMPLES * 2)
#define MONO_BLOCK_COUNT     (2)
#define MONO_BLOCK_SAMPLES   (8000)
#define MONO_BLOCK_SIZE      (MONO_BLOCK_SAMPLES * 2)

int16_t stereoBuffer[STEREO_BLOCK_SAMPLES * STEREO_BLOCK_COUNT];
int16_t monoBuffer[MONO_BLOCK_SAMPLES * MONO_BLOCK_COUNT];

uint32_t mono_block;

/* Reference to the underlying CMSIS vStream driver */
extern vStreamDriver_t          Driver_vStreamAudioIn;
#define vStream_AudioIn       (&Driver_vStreamAudioIn)

/* Audio processing functions */
static int32_t CalculateOffset(int16_t *audioData, uint32_t sampleCount);
static int32_t CalculateScale(int16_t *audioData, uint32_t sampleCount);
static void ApplyGainAndOffset(int16_t *audioData, uint32_t sampleCount, int32_t audioOffset, int32_t audioScale);
static void ConvertToMono(int16_t *stereoData, int16_t *monoData, uint32_t sampleCount);

osThreadId_t tid_app_main = NULL;
osThreadId_t tid_audio_capture = NULL;

void AudioDrv_Event_Callback (uint32_t event) {
  (void)event;

  osThreadFlagsSet(tid_audio_capture, 0x0001);
}

/**
  Process stereo buffer audio data and convert it to fit into mono buffer
*/
void audio_capture (void *arg) {
  int16_t *buf;
  int32_t audioGain   = 0;
  int32_t audioOffset = 0;

  mono_block = 0;

  /* Initialize audio in stream and set the receive buffer */
  vStream_AudioIn->Initialize(AudioDrv_Event_Callback);
  vStream_AudioIn->SetBuf(stereoBuffer, STEREO_BLOCK_COUNT * STEREO_BLOCK_SIZE, STEREO_BLOCK_SIZE);

  /* Start audio receiver */
  vStream_AudioIn->Start(VSTREAM_MODE_CONTINUOUS);

  while(1) {
    /* Wait for flag from audio callback */
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);

    /* Process block of currently received audio samples */
    buf = (int16_t *)vStream_AudioIn->GetBlock();

    /* Recalculate offset and gain */
    audioOffset = CalculateOffset(buf, STEREO_BLOCK_SAMPLES);
    audioGain = CalculateScale(buf, STEREO_BLOCK_SAMPLES);

    /* Apply offset and scaling factor (gain) to each audio sample */
    ApplyGainAndOffset(buf, STEREO_BLOCK_SAMPLES, audioOffset, audioGain);

    /* Move mono buffer data to the beginning (shift by one block) */
    memcpy(monoBuffer, &monoBuffer[MONO_BLOCK_SAMPLES], MONO_BLOCK_SAMPLES * (MONO_BLOCK_COUNT - 1) * 2);

    /* Populate the last block of the mono buffer from the freshly captured stereo audio */
    ConvertToMono(&monoBuffer[MONO_BLOCK_SAMPLES * (MONO_BLOCK_COUNT - 1)], buf, MONO_BLOCK_SAMPLES);

    /* Release buffer block to vStream driver */
    vStream_AudioIn->ReleaseBlock();

    /* Mono buffer is ready, start processing it */
    osThreadFlagsSet(tid_app_main, 0x0001);
  }
}

/*
  Convert stereo audio data to mono.
  The function takes stereo audio data (2 channels) and converts it to mono by
  averaging the two channels.
*/
static void ConvertToMono(int16_t *monoData, int16_t *stereoData, uint32_t n_samples) {
  int16_t* pIn  = stereoData;
  int16_t* pOut = monoData;

  for (int i = 0; i < n_samples; i++){
    pOut[i] = ((pIn[0] >> 1) + (pIn[1] >> 1));

    pIn += 2;
  }
}

/*
  Calculate offset correction value.
  Offset determines how much the audio signal should be shifted up or down to
  center it around zero.
*/
static int32_t CalculateOffset(int16_t *audioData, uint32_t sampleCount) {
  int16_t audioMean = 0;
  arm_mean_q15(audioData, sampleCount, &audioMean);
  return (int32_t)(0 - audioMean);
}

/*
  Calculate a scaling factor for audio normalization.
  The scaling factor is used to normalize or amplify the audio signal to a
  desired range (avoiding over-amplifying noise or silence).
*/
static int32_t CalculateScale(int16_t *audioData, uint32_t sampleCount) {
  /* Define the desired signal span to scale our input signal to. It can be based on
   * the training data set, or close to std::numeric_limits<int16_t>::max()/2; */
  const int32_t desirableSignalSpan = 18000;

  /* Maximum scaling factor. A factor bigger than this may amplify noise which can
   * lead to false detections. */
  const int32_t maxScale = 25;

  int16_t audioMin = 0;
  arm_min_no_idx_q15(audioData, sampleCount, &audioMin);

  int16_t audioMax = 0;
  arm_max_no_idx_q15(audioData, sampleCount, &audioMax);

  int32_t audioScale = desirableSignalSpan / (audioMax - audioMin);

  /* We don't want random silence to be amplified too much; we limit
   * the gain */
  if (audioScale > maxScale) {
    audioScale = maxScale;
  } else if (audioScale < 1) {
    audioScale = 1;
  }

  return audioScale;
}

/*
  Apply gain and offset to the audio data.
  Applies linear transformation with gain and offset and ensures that the values
  stay within the range of int16_t.
*/
static void ApplyGainAndOffset(int16_t *audioData, uint32_t sampleCount, int32_t audioOffset, int32_t audioScale) {
  int16_t *buf = audioData;
  int16_t sample;
  int32_t modified_val;

  /* Apply offset first and then gain */
  for (uint32_t i = 0; i < sampleCount; ++i) {
    sample = buf[i];
    modified_val = ((int32_t)(sample) + audioOffset) * audioScale;

    /* Clip the high end */
    if (modified_val > (int32_t)(INT16_MAX)) {
      modified_val = (int32_t)(INT16_MAX);
    }

    /* Clip the low end */
    if (modified_val < (int32_t)(INT16_MIN)) {
      modified_val = (int32_t)(INT16_MIN);
    }

    buf[i] = (int16_t)(modified_val);
  }
}

uint32_t open_audio_source(const uint32_t idx) {
  uint32_t flags;

  if (tid_audio_capture == NULL) {
    /* Get application thread ID */
    tid_app_main = osThreadGetId();

    /* Create audio capture thread */
    tid_audio_capture = osThreadNew(audio_capture, NULL, NULL);
  }

  /* Wait for flag from audio capture thread (2 sec timeout) */
  flags = osThreadFlagsWait(0x0001, osFlagsWaitAny, 2000);

  if (flags == osFlagsErrorTimeout) {
    /* Capture thread failed to retrieve audio samples */
    return 0;
  }

  return 1;
}

void close_audio_source(const uint32_t idx) {
  /* Unused, audio source is always the same */
  (void)idx;
}

const char* get_audio_name(const uint32_t idx) {
  /* This is audio from hardware audio stream */
  return "Live Audio Stream";
}

const int16_t* get_audio_array(const uint32_t idx) {
  /* Mono buffer is the audio source array */
  return monoBuffer;
}

uint32_t get_audio_array_size(const uint32_t idx) {
  /* Return number of elements in audio array */
  return MONO_BLOCK_SAMPLES * MONO_BLOCK_COUNT;
}
