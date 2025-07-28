# MLEK Reference Application Templates

<!-- markdownlint-disable MD013 -->
<!-- markdownlint-disable MD036 -->


## Video Processing Templates

These templates focus on real-time audio processing:

- **Keyword Spotting (KWS)**: Demonstrates wake word detection and voice command recognition
- **Audio User Algorithm Template**: Provides a foundation for custom audio ML processing applications

### Required API Interfaces

For hardware deployment, the Board-Layer should provide the following API interfaces:

| Required API Interface | Description |
|:----------------------|:------------|
| **CMSIS_VIO** | Virtual I/O interface for LEDs, buttons, and basic I/O |
| **CMSIS_VSTREAM_VIDEO_IN** | Virtual Video Input / Camera Interface |
| **CMSIS_VSTREAM_VIDEO_OUT** | Virtual Video Output / Display |
| **STDOUT, STDERR** | Standard output for printf debugging and logging |

These interfaces ideally are supplied by the vendor of your evaluation board. For custom hardware, details on the implementation are 
found in the [CMSIS-Driver Manual](https://arm-software.github.io/CMSIS_6/latest/Driver/group__vstream__interface__gr.html)

## Object Detection Application

This example uses a neural network model that specialises in detecting human faces in images.
The input size for these images is 192x192 (monochrome) and the smallest face that can be detected is of size 20x20. The output of the application will be co-ordinates for rectangular bounding boxes for each detection.


### Build Types
The Object Detection example defines four build types that control debug information and the video source:

| Build Type | Description |
|------------|-------------|
| **Debug-Live_Stream** | Capture frames from a camera in real time. Debug information enabled. |
| **Release-Live_Stream** | Capture frames from a camera in real time. With optimizations for performance. |
| **Debug-Data_Array** |  Built-in image data for regression testing with debug information. |
| **Release-Data_Array** | Built-in image data with release optimizations. |

Use the Debug build types during development and the Release build types for performance measurements. Switch between *Live_Stream* and *Data_Array* depending on whether you want real-time video or a fixed sample. On Arm Virtual Hardware Targets, the Live_Stream is utilizing the VSI interface

## Video User Algorithm Template

Todo

## Working with MLEK Templates

See Target Configuration chapters on how to deploy the reference applications to a specific hardware or simulation target. 