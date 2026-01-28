# ML Layer

This directory contains the Machine Learning layer for Keyword Spotting using the
MicroNet ML model. The ML layer provides the necessary components and models to
perform keyword spotting on embedded devices with or without Arm Ethos-U NPU acceleration.

## Components

### CMSIS Libraries

The layer integrates industry-standard CMSIS libraries for efficient ML operations:

- **CMSIS-DSP**: Digital signal processing optimizations
- **CMSIS-NN**: Neural network kernels optimized for Cortex-M processors

### TensorFlow Lite Micro

Complete TensorFlow Lite Micro integration including:

- **Kernel Variants**:
  - Ethos-U optimized kernels for NPU acceleration
  - CMSIS-NN kernels for devices without Ethos-U
- **Supporting Libraries**: flatbuffers, gemmlowp, kissfft, ruy
- **ML Eval Kit**: API, logging, math utilities, and keyword spotting algorithms

## Models

### MicroNet Keyword Spotting Model

The layer includes multiple variants of the MicroNet keyword spotting model optimized for
different hardware configurations:

#### 1. **Ethos-U55 (128 MACs)** - `kws_micronet_m_vela_H128.tflite.c`

- Optimized for devices with Arm Ethos-U55 NPU (128 MACs per cycle)
- Uses Vela compiler optimizations
- Best suited for Cortex-M processors with Ethos-U55

#### 2. **Ethos-U55 (256 MACs)** - `kws_micronet_m_vela_H256.tflite.c`

- Optimized for devices with Arm Ethos-U55 NPU (256 MACs per cycle)
- Uses Vela compiler optimizations
- Higher performance variant for Cortex-M with Ethos-U55

#### 3. **Ethos-U65 (256 MACs)** - `kws_micronet_m_vela_Y256.tflite.c`

- Optimized for devices with Arm Ethos-U65 NPU (256 MACs per cycle)
- Uses Vela compiler optimizations
- Higher performance variant for Cortex-M processors with Ethos-U65

#### 4. **Ethos-U85 (256 MACs)** - `kws_micronet_m_vela_Z256.tflite.c`

- Optimized for devices with Arm Ethos-U85 NPU (256 MACs per cycle)
- Uses Vela compiler optimizations
- Latest generation NPU support

#### 5. **CPU-Only** - `kws_micronet_m.tflite.c`

- Suitable for Cortex-M devices without Ethos-U NPU
- Uses CMSIS-NN optimized kernels

### Model Capabilities

All model variants provide:

- **Input**: Audio features (MFCC - Mel-frequency cepstral coefficients)
- **Detection Task**: Real-time keyword spotting and recognition
- **Output**: Classification scores for predefined keywords

## Usage

### Target Type Specification

The ML layer needs properly specified build context to automatically select the appropriate model variant.

Based on the target hardware configuration the target-type shall use the naming convention:

```yml
<name>-<NPU family>-<NPU MACs>
# Where:
# - <name> can be arbitrary name
# - <NPU family> is the Ethos-U NPU family name (U55, U65, U85), if omitted Ethos-U NPU won't be used
# - <NPU MACs> is the number of NPU MACs (128, 256), if omitted 256 MACs is assumed
```

Examples:

```yml
target-types:
  - type: myDevice-U55-128 # Specifies Ethos-U55 with 128 MACs
    ...
  - type: myBoard-U65      # Specifies Ethos-U65 with 256 MACs
    ...
```
