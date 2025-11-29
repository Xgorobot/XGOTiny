# RIG-puppy a Quadrupedal robots.

(English | [中文](README.md) ))

# 🐶RIG-puppy Five-Degree-of-Freedom Robot Dog Lulu
A lightweight intelligent robotic dog based on ESP32-S3 + XiaoZhi AI framework | Fully open-source & replicable | Supports voice and visual multimodal interaction
## Videos

- Coming soon

---

## Introduction

- RIG-puppy (Lulu) is a miniature quadruped robot platform designed for education, makers, and embedded AI enthusiasts.
Powered by the ESP32-S3 chip as the main controller, it integrates the domestic open-source AI voice framework “XiaoZhi,” enabling a fully local closed-loop pipeline including voice wake-up, natural language understanding, and action execution.
Unlike common four-DOF robotic dogs, RIG-puppy innovatively uses five bus-style UART servos to build a 5-DOF motion structure. A waist servo is introduced, and UART bus communication greatly simplifies wiring complexity, making the robot structurally clean and allowing richer and more interesting movements.  

- If you have any ideas or suggestions, feel free to submit them via **Issues**.

---

## Hardware Specifications

- MEMS microphone, effective pickup distance 1–2 m, sensitivity -26 dB ± 3 dB 
- 8Ω 2W full-range speaker, frequency response 200 Hz–20 kHz, supports multi-level volume adjustment (0–100%) 
- 1.09-inch round TFT SPI display, resolution 240×240 
- 70 mm 3 V LED meteor-light strip  
- GC0308 camera 
- EM3 bus UART servos
- ICM42670 high-performance 6-axis MEMS IMU

---

## Features

- **Voice Wake-up:** Custom wake word “Xiao Lu Tongxue”  
- **Camera Capture:** Commands like “open the camera” or “take a photo”  
- **Voice Motion Control:** Commands like “move forward,” “go back,” “shake hands”  
- **Dynamic Lighting Effects:** Commands like “turn on lights” or “switch light mode”  

---

## RIG-puppy Encyclopedia

👉 [《RIG-puppy Encyclopedia》](https://www.yuque.com/luwudynamics/pet/oytelbareyl97xgd)

---

## Software

The firmware connects by default to the [xiaozhi.me](https://xiaozhi.me) official server.  
Personal users can register for free access to the **Qwen real-time model**.



