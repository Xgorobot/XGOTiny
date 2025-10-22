XGO-Tiny  a Quadrupedal robots.

## The First Desktop Pet-Level 5-DOF Robot Dog | 首款桌宠级五自由度机器狗

（中文 | [English](README_en.md) ）

## 视频

👉[噜噜：我的机器人不可能这么搞笑！](https://www.bilibili.com/video/BV1nqa8zHEqR)  
👉[噜噜：国产桌宠赢在哪里？噜噜来啦！](https://www.bilibili.com/video/BV1zctqzsEAa)

## 介绍

- 这是一个由虾哥开源的 ESP32 项目，以 MIT 许可证发布，允许任何人免费使用，或用于商业用途。
- XGO-tiny(噜噜) 是一款面向教育、创客和嵌入式AI爱好者的微型四足机器人平台。它以 ESP32-S3 单芯片为主控核心，深度集成国产开源 AI 语音框架——“小智”（XiaoZhi），实现了从语音唤醒、自然语言理解到动作执行的全链路本地化闭环。不同于其他四自由度机器狗，XGO-tiny 创新性的使用 5个总线式串口舵机 构成五自由度（5DOF）运动结构，引入了机器人腰部舵机，通过 UART 总线通信，大幅简化布线复杂度，使得机器人结构简洁，动作更加丰富有趣。
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


