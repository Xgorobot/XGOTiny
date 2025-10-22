XGO-Tiny  a Quadrupedal robots.

## The First Desktop Pet-Level 5-DOF Robot Dog | 首款桌宠级五自由度机器狗

（中文 | [English](README_en.md) ）

## 视频

👉[噜噜：我的机器人不可能这么搞笑！](https://www.bilibili.com/video/BV1nqa8zHEqR)  
👉[噜噜：国产桌宠赢在哪里？噜噜来啦！](https://www.bilibili.com/video/BV1zctqzsEAa)

## 介绍

- 这是一个由虾哥开源的 ESP32 项目，以 MIT 许可证发布，允许任何人免费使用，或用于商业用途。
- 噜噜采用 ESP32-S3 芯片方案，搭载小智 AI 大脑构建多模态实时交互系统，可兼容 Qwen、DeepSeek 等大模型。
依托优秀的语义理解能力与多语言识别技术，它能实现精准智能对话，轻松满足闲聊、信息查询、动作执行等多样化交互需求。
其独特的五自由度关节设计包含腰部扭动关节，搭配灵活的腿部关节，可完成打招呼、撒娇、求抱抱等一系列协调自然的动作，让互动充满趣味与温度。同时，噜噜集成了麦克风、扬声器、摄像头及流星灯等多元组件，在保障稳定运动的基础上，实现了唤醒词检测、多模态交互融合、持续智能学习及自定义声纹等丰富功能，为用户带来沉浸式的智能桌宠体验。
陆吾智能希望通过这个项目让机器人爱好者学习和了解四组机器人。

- 如果你有任何想法或建议，请随时提出 Issues。

### 硬件参数

- MEMS 麦克风，有效拾音范围 1-2m，灵敏度 - 26dB±3dB
- 8Ω 2W 全频扬声器，频响范围 200Hz-20kHz，支持音量多级调节（0-100%）
- 1.09 英寸 TFT SPI圆形全彩屏幕，分辨率 240*240
- 70mm长3v流星雨流水灯
- GC0308 摄像头
- 消费级舵机

### 实现功能

- 语音唤醒(定制语音唤醒词语"小陆同学")
- 摄像头取景拍照(如:打开摄像头，拍照等)
- 语音控制机器狗运动(如:前进，后退，握手等)
- 激光剑动态灯光(如:打开激光剑，切换灯光模式等)

### 噜噜百科全书

👉 [《噜噜百科全书》](https://www.yuque.com/luwudynamics/pet/oytelbareyl97xgd )

## 软件

固件默认接入 [xiaozhi.me](https://xiaozhi.me) 官方服务器，个人用户注册账号可以免费使用 Qwen 实时模型。

### 开发环境

- Cursor 或 VSCode
- 安装 ESP-IDF 插件，选择 SDK 版本 5.4 或以上
- Linux 比 Windows 更好，编译速度快，也免去驱动问题的困扰
- 本项目使用 Google C++ 代码风格，提交代码时请确保符合规范

### 开发者文档

- [自定义开发板指南](main/boards/README.md) - 学习如何为小智 AI 创建自定义开发板
- [MCP 协议物联网控制用法说明](docs/mcp-usage.md) - 了解如何通过 MCP 协议控制物联网设备
- [MCP 协议交互流程](docs/mcp-protocol.md) - 设备端 MCP 协议的实现方式
- [MQTT + UDP 混合通信协议文档](docs/mqtt-udp.md)
- [一份详细的 WebSocket 通信协议文档](docs/websocket.md)

## 大模型配置

如果你已经拥有一个的小智 AI 聊天机器人设备，并且已接入官方服务器，可以登录 [xiaozhi.me](https://xiaozhi.me) 控制台进行配置。

## 大模型配置
感谢以下B站博主或开发者，排名不分先后
- 项目经理 刘鹏飞LuwuDynamics
- B站博主 Xvsenfeng
- B站博主 香草味的纳西妲喵
- 开发者 蔡乐嫣 Hustle28214
- 开发者 王云馨 KEENNESS19
- 开发者 徐甜 Oral-tian
