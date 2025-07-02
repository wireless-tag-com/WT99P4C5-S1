# WT99P4C5-S1 示例项目

## 项目简介

这是一个基于WT99P4C5_S1的示例项目，采用ESP-Brookesia UI框架展示类似智能手机的用户界面。该项目集成了多种应用程序功能，包括音视频播放、摄像头、游戏、计算器等，并支持人脸检测和行人检测等计算机视觉功能。

## 主要特性

- 🎯 **智能手机式UI界面** - 基于ESP-Brookesia框架
- 📱 **多种应用程序** - 计算器、音乐播放器、视频播放器、2048游戏、摄像头应用
- 🤖 **AI视觉功能** - 人脸检测、行人检测
- 🖥️ **高清显示** - 支持MIPI DSI接口显示屏
- 🎵 **音频处理** - 支持MP3解码和音频播放
- 🎬 **视频处理** - 支持H.264视频解码(目前仅支持mjpeg格式)
- 💾 **多存储支持** - SPIFFS文件系统 + SD卡存储
- 🌐 **网络连接** - WIFI,以太网支持
- 📷 **摄像头支持** - 1280x960分辨率摄像头

## 环境搭建

### 硬件要求

- **开发板**: WT99P4C5-S1开发板
- **显示屏**: MIPI DSI接口显示屏(ek79007)
- **存储**: SD卡（可选）
- **摄像头**: OV5647
- **喇叭**: 3W

>[!NOTE]
>**视频播放说明**
>- 需要将 MJPEG 格式的视频保存到 SD 卡上，并将 SD 卡插入 SD 卡槽
>- 目前仅支持 MJPEG 格式的视频
>- 插入 SD 卡后，视频播放 APP 将自动出现在界面上

>[!TIP]
>**视频格式转换方法**
>
>1. 安装 ffmpeg：
>```bash
>sudo apt update
>sudo apt install ffmpeg
>```
>
>2. 使用 ffmpeg 进行视频转换：
>```bash
>ffmpeg -i YOUR_INPUT_FILE_NAME.mp4 -vcodec mjpeg -q:v 2 -vf "scale=1024:600" -acodec copy YOUR_OUTPUT_FILE_NAME.mjpeg
>```


### 软件环境搭建

#### 1. 安装ESP-IDF

请按照官方文档安装ESP-IDF v5.5(commit:cbe9388f45dd8f33fc560c9727d429e8e107d476)或最新版本：
- [ESP-IDF 入门指南](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html)

#### 2. 克隆项目

```bash
git clone <项目仓库地址>
cd phone_wt99p4c5_s1_board
```

#### 3. 配置环境变量

```bash
. ${IDF_PATH}/export.sh
```

#### 4. 安装项目依赖

```bash
# 安装管理的组件依赖
idf.py reconfigure
```

#### 5. 编译和烧录

```bash
# 配置项目
idf.py menuconfig

# 编译项目
idf.py build

# 烧录到设备
idf.py flash

# 查看串口输出
idf.py monitor
```

## 工程目录介绍

```
phone_wt99p4c5_s1_board/
├── main/                           # 主程序源码
│   ├── main.cpp                    # 程序入口点
│   ├── CMakeLists.txt              # 主程序构建配置
│   └── idf_component.yml           # 组件依赖配置
├── components/                     # 自定义组件
│   ├── apps/                       # 应用程序组件
│   │   ├── calculator/             # 计算器应用
│   │   ├── camera/                 # 摄像头应用
│   │   ├── game_2048/              # 2048游戏
│   │   ├── music_player/           # 音乐播放器
│   │   ├── setting/                # 设置应用
│   │   └── video_player/           # 视频播放器
│   ├── human_face_detect/          # 人脸检测组件
│   ├── pedestrian_detect/          # 行人检测组件
│   ├── wt99p4c5_s1_board/          # 开发板支持包(BSP)
│   └── bsp_extra/                  # 额外的BSP功能
├── spiffs/                         # SPIFFS文件系统数据
│   ├── music/                      # 音乐文件
│   └── 2048/                       # 2048游戏资源
├── mp4/                            # 视频文件目录
├── CMakeLists.txt                  # 顶层构建配置
├── sdkconfig.defaults              # 默认SDK配置
├── partitions.csv                  # 分区表配置
└── README.md                       # 项目说明文档
```

### 核心组件说明

#### 1. 主程序 (`main/`)
- **main.cpp**: 程序入口，初始化系统、显示、存储、网络等模块，并启动各个应用程序

#### 2. 应用程序 (`components/apps/`)
- **calculator/**: 计算器应用，支持基本四则运算
- **camera/**: 摄像头应用，支持拍照和视频预览
- **game_2048/**: 经典2048数字游戏
- **music_player/**: 音乐播放器，支持MP3格式
- **setting/**: 系统设置应用
- **video_player/**: 视频播放器，支持H.264格式

#### 3. AI视觉组件
- **human_face_detect/**: 人脸检测算法实现
- **pedestrian_detect/**: 行人检测算法实现

#### 4. 硬件抽象层
- **wt99p4c5_s1_board/**: 开发板专用BSP，提供硬件初始化和驱动接口
- **bsp_extra/**: 扩展的BSP功能模块

#### 5. 存储和资源
- **spiffs/**: 内置文件系统，存储应用资源和配置文件
- **mp4/**: 视频文件存储目录

## 分区配置

>[!INFO]
>**自定义分区表说明 (`partitions.csv`)**
>- **nvs** (24KB): 非易失性存储，用于配置数据
>- **phy_init** (4KB): RF校准数据
>- **factory** (9MB): 应用程序固件
>- **storage** (4MB): SPIFFS文件系统

## 开发调试

### 查看日志
```bash
idf.py monitor
```

>[!INFO]
>**性能监控说明**
>程序内置内存监控功能，每5秒输出一次内存使用情况：
>- SRAM使用状态
>- PSRAM使用状态  
>- 内存泄漏警告

### 常用配置项
通过 `idf.py menuconfig` 可以配置：
- 显示屏参数设置
- 摄像头分辨率配置
- 音频采样率设置
- Wi-Fi和以太网配置

## 组件库版本要求

### 核心框架依赖
| 组件名称 | 版本要求 | 描述 |
|---------|---------|------|
| **ESP-IDF** | v5.5.0 (commit: cbe9388f45dd8f33fc560c9727d429e8e107d476) | ESP32开发框架 |
| **espressif/esp-brookesia** | 0.4.2 | 智能手机式UI框架 |
| **lvgl/lvgl** | 8.4.0 | 轻量级图形库 |
| **espressif/esp_lvgl_port** | 2.6.0 | LVGL移植层 |

### 视频处理组件
| 组件名称 | 版本要求 | 目标芯片 | 描述 |
|---------|---------|----------|------|
| **espressif/esp_video** | 0.8.0~3 | ESP32P4 | 视频处理框架 |
| **espressif/esp_h264** | 1.1.2 | ESP32S3/P4 | H.264编解码器 |
| **espressif/esp_jpeg** | 1.3.0 | 通用 | JPEG图像处理 |
| **espressif/esp_ipa** | 0.2.0 | ESP32P4 | 图像处理加速器 |

### 显示屏和触摸组件
| 组件名称 | 版本要求 | 目标芯片 | 描述 |
|---------|---------|----------|------|
| **espressif/esp_lcd_ek79007** | 1.0.2 | ESP32P4 | EK79007显示屏驱动 |
| **espressif/esp_lcd_touch** | 1.1.2 | 通用 | 触摸屏基础驱动 |
| **esp_lcd_touch_gt911** | 1.1.3 | 通用 | GT911触摸控制器 |

### 摄像头组件
| 组件名称 | 版本要求 | 目标芯片 | 描述 |
|---------|---------|----------|------|
| **espressif/esp_cam_sensor** | 0.9.0 | ESP32P4 | 摄像头传感器驱动 |
| **espressif/esp_sccb_intf** | 0.0.5 | 通用 | SCCB接口驱动 |

### 音频处理组件
| 组件名称 | 版本要求 | 描述 |
|---------|---------|------|
| **espressif/esp_codec_dev** | 1.2.0 | 音频编解码器设备驱动 |
| **chmorgan/esp-audio-player** | 1.0.7 | 音频播放器库 |
| **chmorgan/esp-libhelix-mp3** | 1.0.3 | MP3解码器库 |

### AI和深度学习组件
| 组件名称 | 版本要求 | 目标芯片 | 描述 |
|---------|---------|----------|------|
| **espressif/esp-dl** | 3.1.0 | ESP32S3/P4 | ESP深度学习推理框架 |

### 网络和通信组件
| 组件名称 | 版本要求 | 目标芯片 | 描述 |
|---------|---------|----------|------|
| **espressif/esp_wifi_remote** | 0.14.2 | ESP32P4/H2 | WiFi远程控制 |
| **espressif/esp_hosted** | 2.0.13 | ESP32P4/H2 | ESP托管模式 |
| **espressif/eppp_link** | 0.3.1 | 通用 | PPP协议链接 |
| **esp_serial_slave_link** | 1.1.0~1 | 通用 | 串行从设备链接 |

### 工具和实用组件
| 组件名称 | 版本要求 | 描述 |
|---------|---------|------|
| **espressif/cmake_utilities** | 0.5.3 | CMake构建工具 |
| **chmorgan/esp-file-iterator** | 1.0.0 | 文件迭代器工具 |

### 版本兼容性说明

>[!IMPORTANT]
>**关键版本要求**
>1. **ESP-IDF版本**: 建议使用v5.5特定commit版本，以确保所有组件的兼容性
>2. **目标芯片**: 主要针对ESP32P4芯片，部分组件也支持ESP32S3
>3. **依赖关系**: 某些组件存在相互依赖关系，请确保版本匹配

>[!CAUTION]
>**更新注意**: 升级组件版本时，请检查依赖关系和兼容性，避免出现不兼容问题

### 安装指定版本组件

如需安装特定版本的组件，可以在`idf_component.yml`中指定：

```yaml
dependencies:
  espressif/esp-brookesia:
    version: "0.4.2"
  espressif/esp_video:
    version: "0.8.0~3"
    rules:
      - if: "target == esp32p4"
```

---
