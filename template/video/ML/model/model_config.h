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

#ifndef MODEL_CONFIG_H_
#define MODEL_CONFIG_H_

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//  <s>NN Model Buffer Section Name
//  <i> Define the name of the NN model buffer section
//  <i> Default: "nn_model_buf"
#ifndef NN_MODEL_BUF_SECTION
#define NN_MODEL_BUF_SECTION        "nn_model_buf"
#endif
//  <o>NN Model Buffer Alignment
//  <i> Define the NN model buffer alignment in bytes
//  <i> Default: 16
#ifndef NN_MODEL_BUF_ALIGNMENT
#define NN_MODEL_BUF_ALIGNMENT      16
#endif

#endif /* MODEL_CONFIG_H_ */