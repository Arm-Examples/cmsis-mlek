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

#ifndef APP_CONFIGURATION_H_
#define APP_CONFIGURATION_H_

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>Buffer Configuration
// ===================================

//  <o>Activation Buffer Size
//  <i> Define the size in bytes of the activation buffer.
//  <i> Default: 1048576 (1MB)
#ifndef ACTIVATION_BUF_SZ
#define ACTIVATION_BUF_SZ           1048576
#endif

//  <s>Activation Buffer Section Name
//  <i> Define the name of the activation buffer section
//  <i> Default: ".bss.activation_buf"
#ifndef ACTIVATION_BUF_SECTION
#define ACTIVATION_BUF_SECTION      ".bss.activation_buf"
#endif

//  <o>Activation Buffer Alignment
//  <i> Define the activation buffer alignment in bytes
//  <i> Default: 16
#ifndef ACTIVATION_BUF_ALIGNMENT
#define ACTIVATION_BUF_ALIGNMENT    16
#endif

// </h>

#endif /* APP_CONFIGURATION_H_ */
