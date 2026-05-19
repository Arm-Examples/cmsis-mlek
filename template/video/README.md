
## Object detection

This example uses a neural network model that specialises in detecting human faces in images.
The input size for these images is 192x192 (monochrome) and the smallest face that can be
detected is of size 20x20. The output of the application will be co-ordinates for rectangular
bounding boxes for each detection.

The current runtime is [TensorFlow Lite Micro](https://www.keil.arm.com/packs/tensorflow-lite-micro-tensorflow). [ExecuTorch](https://pytorch.org/executorch/) runtime support, including the [Arm Ethos-U backend](https://github.com/pytorch/executorch/tree/main/backends/arm), is planned in follow-up work.


## Related

- [MLEK Pack](https://www.keil.arm.com/packs/MLEK) available on [keil.arm.com/packs](https://www.keil.arm.com/packs).
- [tensorflow-lite-micro Pack](https://www.keil.arm.com/packs/tensorflow-lite-micro-tensorflow) available on [keil.arm.com/packs](https://www.keil.arm.com/packs).
- [ExecuTorch](https://pytorch.org/executorch/) runtime project with the [Arm Ethos-U backend](https://github.com/pytorch/executorch/tree/main/backends/arm).
- [MLEK Examples](https://github.com/Arm-Examples/mlek-examples) a repository with MLEK applications that are configured for evaluation boards and use cases.
- [ML Developers Guide for Cortex-M Processors and Ethos-U NPU](https://developer.arm.com/documentation/109267).
- [Arm Virtual Hardware - FVP](https://github.com/arm-software/avh) repository with documentation.