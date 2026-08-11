## Keyword spotting

This example can detect up to twelve keywords in the input audio stream. The
[audio file used](./resources/sample_audio.wav) contains the keyword "down" being spoken.

More details about the input for this example can be found [here](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ml-embedded-evaluation-kit/+/refs/heads/main/docs/use_cases/kws.md#preprocessing-and-feature-extraction).

The current runtime is [TensorFlow Lite Micro](https://www.keil.arm.com/packs/tensorflow-lite-micro-tensorflow). [ExecuTorch](https://pytorch.org/executorch/) runtime support, including the [Arm Ethos-U backend](https://github.com/pytorch/executorch/tree/main/backends/arm), is planned in follow-up work.


## Related

- [MLEK Pack](https://www.keil.arm.com/packs/cmsis-mlek-arm) available on [keil.arm.com/packs](https://www.keil.arm.com/packs).
- [tensorflow-lite-micro Pack](https://www.keil.arm.com/packs/tensorflow-lite-micro-tensorflow) available on [keil.arm.com/packs](https://www.keil.arm.com/packs).
- [ExecuTorch](https://pytorch.org/executorch/) runtime project with the [Arm Ethos-U backend](https://github.com/pytorch/executorch/tree/main/backends/arm).
- [MLEK Examples](https://github.com/Arm-Examples/mlek-examples) a repository with MLEK applications that are configured for evaluation boards and use cases.
- [ML Developers Guide for Cortex-M Processors and Ethos-U NPU](https://developer.arm.com/documentation/109267).
- [Arm Virtual Hardware - FVP](https://github.com/arm-software/avh) repository with documentation.
