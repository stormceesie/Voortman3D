# Voortman3D

## Overview

Voortman3D is a proof of concept for machine simulation software designed to provide real-time visualization of 3D models. Utilizing Vulkan API for high-performance graphics rendering and KTX for texture management, Voortman3D leverages TwinCAT ADS API to interact with PLC variables. This integration allows dynamic control and real-time animation of 3D models based on PLC data. To ensure optimal performance, the majority of calculations are offloaded to the GPU, minimizing the load on the CPU.

## Features

- **Real-Time 3D Visualization**: Display and animate 3D models in real-time based on PLC data.
- **Vulkan API**: High-performance graphics rendering with modern Vulkan API.
- **KTX Texture Management**: Efficient handling and loading of textures using KTX.
- **TwinCAT ADS Integration**: Seamless communication with PLCs for dynamic control of simulations.
- **GPU-Accelerated Computation**: Offload computational tasks to the GPU to reduce CPU load.

## Prerequisites

- **Vulkan SDK**: Ensure the Vulkan SDK is installed on your system.
- **KTX Library**: Required for texture management.
- **TwinCAT ADS SDK**: For communication with PLCs.

- ## Contact
For any questions or feedback, please open an issue on GitHub or contact kegler.florent@gmail.com.
