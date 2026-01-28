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

#ifndef BUF_ATTRIBUTES_H_
#define BUF_ATTRIBUTES_H_

#include "AppConfiguration.h"   /* Application configuration  */

/* Attributes applied to the activation buffer object */
#define ACTIVATION_BUF_ATTRIBUTE \
  __attribute__((section(ACTIVATION_BUF_SECTION), aligned(ACTIVATION_BUF_ALIGNMENT)))

#endif /* BUF_ATTRIBUTES_H_ */
