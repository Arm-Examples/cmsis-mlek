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
#include <stdio.h>

#include "VideoConfiguration.h"
#include "VideoSource.h"
#include "BufAttributes.h"

#include "image_processing_func.h"
#include "cmsis_vstream.h"
#include "cmsis_os2.h"


/* Reference to the underlying CMSIS vStream VideoIn driver */
extern vStreamDriver_t          Driver_vStreamVideoIn;
#define vStream_VideoIn       (&Driver_vStreamVideoIn)

/* Reference to the underlying CMSIS vStream VideoOut driver */
extern vStreamDriver_t          Driver_vStreamVideoOut;
#define vStream_VideoOut      (&Driver_vStreamVideoOut)

/* Camera frame buffer (RAW8 or RGB565) */
static uint8_t CAM_Frame[CAMERA_FRAME_SIZE] CAMERA_FRAME_BUF_ATTRIBUTE;

/* RGB image buffer (RGB888) */
static uint8_t RGB_Image[RGB_IMAGE_SIZE] RGB_IMAGE_BUF_ATTRIBUTE;

/* ML image buffer (RGB888) */
static uint8_t ML_Image[ML_IMAGE_SIZE] ML_IMAGE_BUF_ATTRIBUTE;

/* Display frame buffer (RGB888) */
static uint8_t LCD_Frame[DISPLAY_IMAGE_SIZE] DISPLAY_FRAME_BUF_ATTRIBUTE;

static void DrawBox(uint8_t *imageData, const uint32_t x0, const uint32_t y0, const uint32_t w, const uint32_t h);
static void convert_frame_to_rgb(uint8_t *inFrame);

osThreadId_t tid_app_main = NULL;

uint8_t VideoIn_Ready = 0U;
uint8_t VideoOut_Ready = 0U;

uint32_t EventCnt_VideoIn = 0U;
uint32_t EventCnt_VideoOut = 0U;

/* Video In Stream Event Callback */
void VideoIn_Event_Callback (uint32_t event) {
  (void)event;

  if (event & VSTREAM_EVENT_DATA) {
    /* Video frame is available in camera frame buffer */
    osThreadFlagsSet(tid_app_main, 0x1);
  }
  EventCnt_VideoIn++;
}

/* Video Out Stream Event Callback */
void VideoOut_Event_Callback (uint32_t event) {
  (void)event;

  EventCnt_VideoOut++;
}

uint32_t open_img_source(const uint32_t idx) {
  uint8_t *inFrame;
  uint8_t *outFrame;
  vStreamStatus_t status;

  tid_app_main = osThreadGetId();

  if (VideoIn_Ready == 0U) {
    /* Initialize Video Input Stream */
    if (vStream_VideoIn->Initialize(VideoIn_Event_Callback) != VSTREAM_OK) {
      printf("Failed to initialise video input driver\n");
      return 0U;
    }

    /* Set Input Video buffer */
    if (vStream_VideoIn->SetBuf(CAM_Frame, sizeof(CAM_Frame), CAMERA_FRAME_SIZE) != VSTREAM_OK) {
      printf("Failed to set buffer for video input\n");
      return 0U;
    }

    VideoIn_Ready = 1U;
  }

  /* Start video capture */
  if (vStream_VideoIn->Start(VSTREAM_MODE_SINGLE) != VSTREAM_OK) {
    printf("Failed to start video capture\n");
    return 0U;
  }


  /* Wait for new video input frame */
  osThreadFlagsWait(0x1, osFlagsWaitAny, osWaitForever);

  /* Get input video frame buffer */
  inFrame = (uint8_t *)vStream_VideoIn->GetBlock();
  if (inFrame == NULL) {
    printf("Failed to get video input frame\n");
    return 0U;
  }

  /* Convert input frame and place it into RGB_Image buffer */
  convert_frame_to_rgb(inFrame);

  /* Resize RGB image to fit ML model expected size */
  image_resize(RGB_Image,
               RGB_IMAGE_WIDTH,
               RGB_IMAGE_HEIGHT,
               (uint8_t *)ML_Image,
               ML_IMAGE_WIDTH,
               ML_IMAGE_HEIGHT,
               IMAGE_FORMAT_RGB888,
               IMAGE_FORMAT_RGB888);

  /* Release input frame */
  if (vStream_VideoIn->ReleaseBlock() != VSTREAM_OK) {
    printf("Failed to release video input frame\n");
  }

  return 1U;
}

void close_img_source(const uint32_t idx) {
  vStreamStatus_t status;
  uint8_t *outFrame;

  if (VideoOut_Ready == 0U) {
    /* Initialize Video Output Stream */
    if (vStream_VideoOut->Initialize(VideoOut_Event_Callback) != VSTREAM_OK) {
      printf("Failed to initialise video output driver\n");
      return;
    }

    /* Set Output Video buffer */
    if (vStream_VideoOut->SetBuf(LCD_Frame, sizeof(LCD_Frame), DISPLAY_IMAGE_SIZE) != VSTREAM_OK) {
      printf("Failed to set buffer for video output\n");
      return;
    }

    VideoOut_Ready = 1U;
  }

  /* Wait for video output frame to be released */
  do {
    status = vStream_VideoOut->GetStatus();
  } while (status.active == 1U);

  /* Get output frame */
  outFrame = (uint8_t *)vStream_VideoOut->GetBlock();
  if (outFrame == NULL) {
    printf("Failed to get video output frame\n");
    return;
  }

  /* Copy ML image into the display frame buffer */
  image_copy_to_framebuffer(ML_Image,
                            ML_IMAGE_WIDTH,
                            ML_IMAGE_HEIGHT,
                            outFrame,
                            DISPLAY_FRAME_WIDTH,
                            DISPLAY_FRAME_HEIGHT,
                            (DISPLAY_FRAME_WIDTH - ML_IMAGE_WIDTH) / 2,
                            (DISPLAY_FRAME_HEIGHT - ML_IMAGE_HEIGHT)/2,
                            IMAGE_FORMAT_RGB888);

  /* Release output frame */
  if (vStream_VideoOut->ReleaseBlock() != VSTREAM_OK) {
    printf("Failed to release video output frame\n");
  }

  /* Start video output */
  if (vStream_VideoOut->Start(VSTREAM_MODE_SINGLE) != VSTREAM_OK) {
    printf("Failed to start video output\n");
  }
}

const char* get_filename(const uint32_t idx) {
  return "Live Video Stream";
}

const uint8_t* get_img_array(const uint32_t idx) {
  return ML_Image;
}

uint32_t get_img_array_size(const uint32_t idx) {
  /* Return image array size in bytes */
  return sizeof(ML_Image);
}

void set_img_object_box(const uint32_t idx, const uint32_t x0, const uint32_t y0, const uint32_t w, const uint32_t h) {
  /* Draw a box around detected object */
  DrawBox(ML_Image, x0, y0, w, h);
}

/**
 * Draw a box in the image.
 *
 * \param[out] imageData    Pointer to the start of the image.
 * \param[in]  width        Image width.
 * \param[in]  height       Image height.
 * \param[in]  result       Object detection result.
 */
static void DrawBox(uint8_t *imageData, const uint32_t x0, const uint32_t y0, const uint32_t w, const uint32_t h) {
  const uint32_t step = ML_IMAGE_WIDTH * 3;
  uint8_t* const imStart = imageData + (y0 * step) + (x0 * 3);

  uint8_t* dst_0 = imStart;
  uint8_t* dst_1 = imStart + (h * step);

  for (uint32_t i = 0; i < w; ++i) {
    dst_0[1] = 255;
    dst_1[1] = 255;

    dst_0 += 3;
    dst_1 += 3;
  }

  dst_0 = imStart;
  dst_1 = imStart + (w * 3);

  for (uint32_t j = 0; j < h; ++j) {
    dst_0[1] = 255;
    dst_1[1] = 255;

    dst_0 += step;
    dst_1 += step;
  }
}

/*
  Converts camera frame and copies it to RGB image buffer.

  Camera frame may be square or non-square and must be in RAW8 or RGB565 format.
  RGB image buffer is always square and is in RGB888 format.

  The function handles the following cases:
    - If the camera frame is square and matches the RGB image size:
      - crop and debayer the RAW8 camera frame
      - convert RGB565 camera frame to RGB888
      - copy RGB888 camera frame to RGB image buffer.
    - If the camera frame is square and larger than the RGB image size:
      - crop and debayer the RAW8 camera frame
      - resize RGB565 camera frame to fit into RGB image buffer.
      - resize RGB888 camera frame to fit into RGB image buffer.
    - If the camera frame is not square:
      - crop and debayer the RAW8 camera frame
      - crop RGB565 camera frame to fit into RGB image buffer.
      - crop RGB888 camera frame to fit into RGB image buffer.
*/
static void convert_frame_to_rgb(uint8_t *inFrame) {
  #if (CAMERA_FRAME_WIDTH == CAMERA_FRAME_HEIGHT)
    /* Camera frame is square */
    #if (CAMERA_FRAME_WIDTH == RGB_IMAGE_WIDTH) && (CAMERA_FRAME_HEIGHT == RGB_IMAGE_HEIGHT)
        /* Camera frame size matches RGB image size */
        #if (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RAW8)
        /* For RAW8, crop and debayer into RGB image buffer (RGB888) */
        crop_and_debayer(inFrame,
                        CAMERA_FRAME_WIDTH,
                        CAMERA_FRAME_HEIGHT,
                        0, 0, /* Crop from top-left corner */
                        RGB_Image,
                        RGB_IMAGE_WIDTH,
                        RGB_IMAGE_HEIGHT,
                        CAMERA_FRAME_BAYER);
        #elif (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RGB565)
        /* For RGB565, convert frame to fit into RGB image buffer (RGB888) */
        convert_rgb565_to_rgb888(inFrame, RGB_Image, CAMERA_FRAME_WIDTH, CAMERA_FRAME_HEIGHT);
        #elif (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RGB888)
        /* For RGB888, just copy the frame */
        memcpy(RGB_Image, inFrame, CAMERA_FRAME_WIDTH * CAMERA_FRAME_HEIGHT * 3);
        #endif
    #else
        /* Camera frame size is larger than RGB image size */
        #if (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RAW8)
        /* For RAW8, crop and debayer into RGB image buffer (RGB888) */
        crop_and_debayer(inFrame,
                        CAMERA_FRAME_WIDTH,
                        CAMERA_FRAME_HEIGHT,
                        (CAMERA_FRAME_WIDTH - RGB_IMAGE_WIDTH) / 2, /* Center crop */
                        (CAMERA_FRAME_HEIGHT - RGB_IMAGE_HEIGHT) / 2,
                        RGB_Image,
                        RGB_IMAGE_WIDTH,
                        RGB_IMAGE_HEIGHT,
                        CAMERA_FRAME_BAYER);
        #elif (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RGB565)
        /* For RGB565, resize frame to fit into RGB image buffer (RGB888) */
        image_resize(inFrame,
                    CAMERA_FRAME_WIDTH,
                    CAMERA_FRAME_HEIGHT,
                    RGB_Image,
                    RGB_IMAGE_WIDTH,
                    RGB_IMAGE_HEIGHT,
                    IMAGE_FORMAT_RGB565,
                    IMAGE_FORMAT_RGB888);
        #elif (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RGB888)
        /* For RGB888, resize frame to fit into RGB image buffer (RGB888) */
        image_resize(inFrame,
                    CAMERA_FRAME_WIDTH,
                    CAMERA_FRAME_HEIGHT,
                    RGB_Image,
                    RGB_IMAGE_WIDTH,
                    RGB_IMAGE_HEIGHT,
                    IMAGE_FORMAT_RGB888,
                    IMAGE_FORMAT_RGB888);
        #endif
    #endif
  #endif

  #if (CAMERA_FRAME_WIDTH != CAMERA_FRAME_HEIGHT)
    /* Camera frame is not square, crop it to fit RGB buffer */
    #if (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RAW8)
      /* For RAW8, crop and debayer to RGB888 */
      crop_and_debayer(inFrame,
                      CAMERA_FRAME_WIDTH,
                      CAMERA_FRAME_HEIGHT,
                      (CAMERA_FRAME_WIDTH - RGB_IMAGE_WIDTH) / 2, /* Center crop */
                      (CAMERA_FRAME_HEIGHT - RGB_IMAGE_HEIGHT) / 2,
                      RGB_Image,
                      RGB_IMAGE_WIDTH,
                      RGB_IMAGE_HEIGHT,
                      CAMERA_FRAME_BAYER);
    #elif (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RGB565)
      /* For RGB565, crop and convert to RGB888 */
      crop_rgb565_to_rgb888(inFrame,
                           CAMERA_FRAME_WIDTH,
                           CAMERA_FRAME_HEIGHT,
                           RGB_Image,
                           (CAMERA_FRAME_WIDTH - RGB_IMAGE_WIDTH) / 2, /* Center crop */
                           (CAMERA_FRAME_HEIGHT - RGB_IMAGE_HEIGHT) / 2,
                           RGB_IMAGE_WIDTH,
                           RGB_IMAGE_HEIGHT);
    #elif (CAMERA_FRAME_TYPE == CAMERA_FRAME_TYPE_RGB888)
      /* For RGB888, just crop */
      crop_rgb888_to_rgb888(inFrame,
                           CAMERA_FRAME_WIDTH,
                           CAMERA_FRAME_HEIGHT,
                           RGB_Image,
                           (CAMERA_FRAME_WIDTH - RGB_IMAGE_WIDTH) / 2, /* Center crop */
                           (CAMERA_FRAME_HEIGHT - RGB_IMAGE_HEIGHT) / 2,
                           RGB_IMAGE_WIDTH,
                           RGB_IMAGE_HEIGHT);
    #endif
  #endif
}
