# Overview

The **Machine Learning Evaluation Kit (MLEK)** pack contains [CMSIS Reference Applications](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/) and templates for Edge AI development with embedded systems. These applications implement data preprocessing, memory management, and neural network inference pipelines that are optimized for Cortex-M and Ethos-U platforms.

Key Features:

- **Rapid Prototyping**: Get working ML applications running quickly with minimal setup.
- **Algorithm Development**: Use example code as design patterns for custom ML algorithm implementation.
- **Model Integration**: Easily swap in custom TensorFlow Lite models with minimal code changes.
- **Performance Validation**: Test and optimize ML performance on target hardware or simulation.
- **Hardware Evaluation**: Compare performance across different Corstone platforms and configurations.

MLEK covers three main application domains:

- [Audio](todo-link-to-docu) processing with audio classification and keyword spotting.
- [Video](todo-link-to-docu) processing with image classification and object detection.
- [Generic](todo-link-to-docu) inference for flexible ML model deployment.

Each reference applications is a *csolution project* which supports deployment to physical hardware or Arm Virtual Hardware (AVH-FVP) for simulation.

![MLEK Reference Application Architecture](./images/MLEK-Architecture.png)

A board layer (`*.clayer.yml`) implements the drivers for the physical interfaces. The API interfaces required by the different applications is shown in the table below.

| Required API Interfaces     | Description     |
|:----------------------------|:----------------|
| **Audio Processing**        |                 |
| CMSIS_VSTREAM_AUDIO_IN      | [CMSIS-Driver vStream](https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html) configured for Audio input. |
| **Video Processing**        |                 |
| CMSIS_VSTREAM_VIDEO_IN      | [CMSIS-Driver vStream](https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html) configured for Video input. |
| CMSIS_VSTREAM_VIDEO_OUT     | [CMSIS-Driver vStream](https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html) configured for Video output. |
| **Generic Inference Runner**|                 |
| CMSIS_VSTREAM_AUDIO_IN      | [CMSIS-Driver vStream](https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html configured for Audio input. |

## Platform Support

The templates support via _target names_ multiple [Arm Cortex-M IP Subsystems](https://www.arm.com/products/silicon-ip-subsystems#Products). 

| IP Subsystem | Description  |
|:-------------|:-------------|
| Corstone-300 | Cortex-M55 optional with Ethos-U55 or Ethos-U65 |
| Corstone-310 | Cortex-M85 optional with Ethos-U55 |
| Corstone-315 | Cortex-M85 optional with Ethos-U65 |
| Corstone-320 | Cortex-M85 optional with Ethos-U85 |

Adding a postfix to the _target name_ in the `*.csolution.yml` project file configures the neural network inference pipeline for Ethos-U. Without this prefix only the Cortex-M system is used as shown in the diagram below.

| Postfix      | Description |
|:-------------|:------------|
| _none_       | Cortex-M system only, no Ethos-U NPU |
| `-U55`       | Cortex-M system + Ethos-U55 NPU |
| `-U65`       | Cortex-M system + Ethos-U65 NPU |
| `-U85`       | Cortex-M system + Ethos-U85 NPU |

![Neural Network Inference Pipeline](./images/System-Topology.png)
