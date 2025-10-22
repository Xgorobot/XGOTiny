# XGO-Tiny a Quadrupedal robots.

(English | [中文](README.md) ))

# The First Desktop Pet-Level 5-DOF Robot Dog

## Videos

👉 [LULU: My Robot Can’t Possibly Be This Funny!](https://www.bilibili.com/video/BV1nqa8zHEqR)  
👉 [LULU: Why Domestic Desktop Pets Are Winning!](https://www.bilibili.com/video/BV1zctqzsEAa)

---

## Introduction

- This is an open-source ESP32 project by **Xia Ge**, released under the **MIT License**, allowing free personal and commercial use.  

- **LULU** is built on the **ESP32-S3** platform and powered by the **Xiaozhi AI Engine**, enabling a multimodal real-time interactive system compatible with large models such as **Qwen** and **DeepSeek**.  

With advanced semantic understanding and multilingual recognition capabilities, LULU can engage in natural conversations, perform tasks, and handle information queries with ease.  

Its **five degrees of freedom design**, including a waist rotation joint and flexible leg mechanisms, allows lifelike movements such as waving, cuddling, and playful gestures — creating a warm and engaging interaction experience.  

LULU integrates a **microphone, speaker, camera, and RGB meteor light**, supporting wake-word detection, multimodal fusion, continuous learning, and personalized voiceprint functions — offering users an immersive smart desktop pet experience.  

**Luwu Dynamics** aims to help robotics enthusiasts learn and explore quadruped robot systems through this project.  

- If you have any ideas or suggestions, feel free to submit them via **Issues**.

---

## Hardware Specifications

- MEMS Microphone – Effective pickup range: 1–2 m, Sensitivity: -26 dB ±3 dB  
- 8Ω 2W Full-range Speaker – Frequency response: 200 Hz–20 kHz, Adjustable volume (0–100%)  
- 1.09” Round TFT SPI Display – 240×240 resolution  
- 70 mm 3V Meteor Flow Light Strip  
- GC0308 Camera Module  
- Consumer-grade Servos  

---

## Features

- **Voice Wake-up:** Custom wake word “Xiao Lu Tongxue”  
- **Camera Capture:** Commands like “open the camera” or “take a photo”  
- **Voice Motion Control:** Commands like “move forward,” “go back,” “shake hands”  
- **Dynamic Lighting Effects:** Commands like “turn on lights” or “switch light mode”  

---

## LULU Encyclopedia

👉 [《LULU Encyclopedia》](https://www.yuque.com/luwudynamics/pet/oytelbareyl97xgd)

---

## Software

The firmware connects by default to the [xiaozhi.me](https://xiaozhi.me) official server.  
Personal users can register for free access to the **Qwen real-time model**.

---

### Development Environment

- **Editor:** Cursor or VSCode  
- **ESP-IDF:** Version 5.4 or above  
- **Recommended OS:** Linux (faster compilation, fewer driver issues)  
- **Code Style:** Google C++ Style Guide — please follow it before committing code  

---

### Developer Documentation

- [Custom Board Guide](main/boards/README.md) – Learn how to create your own AI board for Xiaozhi  
- [MCP Protocol for IoT Control](docs/mcp-usage.md) – How to control IoT devices via MCP  
- [MCP Interaction Flow](docs/mcp-protocol.md) – Implementation guide for device-side MCP protocol  
- [MQTT + UDP Hybrid Communication Protocol](docs/mqtt-udp.md)  
- [Detailed WebSocket Communication Protocol](docs/websocket.md)

---

## AI Model Configuration

If you already have a **Xiaozhi AI robot device** connected to the official server,  
you can log in to the [xiaozhi.me](https://xiaozhi.me) console for model configuration.

---

## Acknowledgements

Special thanks to the following contributors or Bilibili Creator (in no particular order):

- Project Manager: **Liu Pengfei (LuwuDynamics)**  
- Bilibili Creator: **Xvsenfeng**  
- Bilibili Creator: **Vanilla Nahida Meow**  
- Developer: **Cai Leyan (Hustle28214)**  
- Developer: **Wang Yunxin (KEENNESS19)**  
- Developer: **Xu Tian (Oral-tian)**

