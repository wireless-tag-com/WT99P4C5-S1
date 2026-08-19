# WT99P4C5-S1 示例项目

[英文版本](./README.md)

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

## 视频播放器功能

### 支持的视频格式

>[!NOTE]
>**视频播放说明**
>- 需要将 MJPEG 格式的视频保存到 SD 卡上，并将 SD 卡插入 SD 卡槽
>- 目前仅支持 MJPEG 格式的视频
>- 插入 SD 卡后，视频播放 APP 将自动出现在界面上

### 视频格式转换

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

### 视频播放器使用方法

1. **准备视频文件**
   - 使用上述方法将视频文件转换为 MJPEG 格式
   - 确保分辨率调整为 1024x600 以获得最佳显示效果

2. **设置SD卡**
   - 将格式化的 SD 卡插入 SD 卡槽
   - 将转换后的 MJPEG 文件复制到 SD 卡中

3. **启动视频播放器**
   - 视频播放器应用会自动读取 SD 卡
   - 从读取的文件列表中选择视频进行播放

## 环境搭建

### 硬件要求

- **开发板**: WT99P4C5-S1开发板

![WT99P4C5-S1](./docs/WT99P4C5-S1.png#pic_center)

- **显示屏**: MIPI DSI接口显示屏(ek79007)
- **存储**: SD卡（可选）
- **摄像头**: OV5647
- **喇叭**: 3W


### 软件环境搭建

#### 1. 安装ESP-IDF

请按照官方文档安装ESP-IDF v5.5.5(commit b774170ff46c393eeb5e495ea37936038d3f4f4f)：
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

## WIFI/BLE使用

### 说明

- ESP32P4自身是不支持WIFI，因此使用WIFI则需要加一个支持WIFI/BLE的协处理器；
- 当前开发板搭配的是ESP32C5，使用SDIO通信。

### 使用

#### ESP32P4工程配置

##### 1. 配置esp-hosted的slave芯片类型
打开menuconfig进入对应的配置选项，配置esp32c5作为slave

```bash
(Top) → Component config → Wi-Fi Remote → choose slave target
```

>[!NOTE] 需要注意引脚配置

##### 2. 配置esp32p4芯片版本

先观察开发板的模组型号，分为：N16R32、N16R32X两种
###### 2.1 N16R32
![N16R32](./docs/N16R32_model.png#pic_center)

需要在menuconfig里面配置如下：
```bash
(Top) → Component config → Hardware Settings → Chip revision
```
![WT0132P4_A1_N16R32](./docs/WT0132P4_A1_N16R32.png#pic_center)

###### 2.2 N16R32X
![N16R32](./docs/N16R32X_model.png#pic_center)

需要在menuconfig里面配置如下：
```bash
(Top) → Component config → Hardware Settings → Chip revision
```
![WT0132P4_A1_N16R32X](./docs/WT0132P4_A1_N16R32X.png#pic_center)

#### 克隆esp32c5工程

```bash
git clone https://github.com/espressif/esp-hosted-mcu.git
cd esp-hosted-mcu/slave
```

#### 编译和烧录

```bash
# 设置目标芯片
idf.py set-target esp32c5

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

## 日志输出

- ESP32P4

```bash
ESP-ROM:esp32p4-eco2-20240710
Build:Jul 10 2024
rst:0x1 (POWERON),boot:0xf (SPI_FAST_FLASH_BOOT)
SPI mode:DIO, clock div:1
load:0x4ff33ce0,len:0x17a4
load:0x4ff29ed0,len:0xf28
--- 0x4ff29ed0: esp_bootloader_get_description at /home/ferry/esp/idf55/components/esp_bootloader_format/esp_bootloader_desc.c:39

load:0x4ff2cbd0,len:0x3454
--- 0x4ff2cbd0: esp_flash_encryption_enabled at /home/ferry/esp/idf55/components/bootloader_support/src/flash_encrypt.c:89

entry 0x4ff29eda
--- 0x4ff29eda: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

I (25) boot: ESP-IDF v5.5-beta1-204-gcbe9388f45 2nd stage bootloader
I (26) boot: compile time Jul  4 2025 14:31:51
I (26) boot: Multicore bootloader
I (29) boot: chip revision: v1.0
I (30) boot: efuse block revision: v0.3
I (34) qio_mode: Enabling default flash chip QIO
I (38) boot.esp32p4: SPI Speed      : 80MHz
I (42) boot.esp32p4: SPI Mode       : QIO
I (46) boot.esp32p4: SPI Flash Size : 16MB
I (50) boot: Enabling RNG early entropy source...
I (54) boot: Partition Table:
I (57) boot: ## Label            Usage          Type ST Offset   Length
I (63) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (69) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (76) boot:  2 factory          factory app      00 00 00010000 00900000
I (82) boot:  3 storage          Unknown data     01 82 00910000 00400000
I (90) boot: End of partition table
I (93) esp_image: segment 0: paddr=00010020 vaddr=481d0020 size=405ecch (4218572) map
I (745) esp_image: segment 1: paddr=00415ef4 vaddr=30100000 size=00088h (   136) load
I (747) esp_image: segment 2: paddr=00415f84 vaddr=4ff00000 size=0a094h ( 41108) load
I (758) esp_image: segment 3: paddr=00420020 vaddr=48000020 size=1c1160h (1839456) map
I (1040) esp_image: segment 4: paddr=005e1188 vaddr=4ff0a094 size=138e8h ( 80104) load
I (1056) esp_image: segment 5: paddr=005f4a78 vaddr=4ff1d980 size=03d98h ( 15768) load
I (1061) esp_image: segment 6: paddr=005f8818 vaddr=50108080 size=00020h (    32) load
I (1069) boot: Loaded app from partition at offset 0x10000
I (1069) boot: Disabling RNG early entropy source...
I (1082) hex_psram: vendor id    : 0x0d (AP)
I (1082) hex_psram: Latency      : 0x01 (Fixed)
I (1082) hex_psram: DriveStr.    : 0x00 (25 Ohm)
I (1083) hex_psram: dev id       : 0x03 (generation 4)
I (1088) hex_psram: density      : 0x07 (256 Mbit)
I (1093) hex_psram: good-die     : 0x06 (Pass)
I (1097) hex_psram: SRF          : 0x02 (Slow Refresh)
I (1102) hex_psram: BurstType    : 0x00 ( Wrap)
I (1106) hex_psram: BurstLen     : 0x03 (2048 Byte)
I (1111) hex_psram: BitMode      : 0x01 (X16 Mode)
I (1115) hex_psram: Readlatency  : 0x04 (14 cycles@Fixed)
I (1120) hex_psram: DriveStrength: 0x00 (1/1)
I (1125) MSPI DQS: tuning success, best phase id is 0
I (1297) MSPI DQS: tuning success, best delayline id is 17
I esp_psram: Found 32MB PSRAM device
I esp_psram: Speed: 200MHz
I (1498) mmu_psram: .rodata xip on psram
I (1587) mmu_psram: .text xip on psram
I (1587) hex_psram: psram CS IO is dedicated
I (1588) cpu_start: Multicore app
I (1971) esp_psram: SPI SRAM memory test OK
I (1980) cpu_start: Pro cpu start user code
I (1980) cpu_start: cpu freq: 360000000 Hz
I (1980) app_init: Application information:
I (1980) app_init: Project name:     phone_p4_function_ev_board
I (1986) app_init: App version:      bcd94a6-dirty
I (1990) app_init: Compile time:     Jul  4 2025 17:55:56
I (1996) app_init: ELF file SHA256:  7e8b8b868...
I (2000) app_init: ESP-IDF:          v5.5-beta1-204-gcbe9388f45
I (2006) efuse_init: Min chip rev:     v0.1
I (2010) efuse_init: Max chip rev:     v1.99 
I (2014) efuse_init: Chip rev:         v1.0
I (2018) heap_init: Initializing. RAM available for dynamic allocation:
I (2024) heap_init: At 4FF256B0 len 00015910 (86 KiB): RAM
I (2029) heap_init: At 4FF3AFC0 len 00004BF0 (18 KiB): RAM
I (2034) heap_init: At 4FF40000 len 00040000 (256 KiB): RAM
I (2040) heap_init: At 501080A0 len 00007F60 (31 KiB): RTCRAM
I (2045) heap_init: At 30100088 len 00001F78 (7 KiB): TCM
I (2050) esp_psram: Adding pool of 26752K of PSRAM memory to heap allocator
I (2057) esp_psram: Adding pool of 59K of PSRAM memory gap generated due to end address alignment of irom to the heap allocator
I (2068) esp_psram: Adding pool of 40K of PSRAM memory gap generated due to end address alignment of drom to the heap allocator
I (2080) spi_flash: detected chip: generic
I (2083) spi_flash: flash io: qio
I (2086) host_init: ESP Hosted : Host chip_ip[18]
I (2100) H_API: ESP-Hosted starting. Hosted_Tasks: prio:23, stack: 5120 RPC_task_stack: 5120
sdio_mempool_create free:27610324 min-free:27610324 lfb-def:27262976 lfb-8bit:27262976

I (2107) H_API: ** add_esp_wifi_remote_channels **
I (2111) transport: Add ESP-Hosted channel IF[1]: S[0] Tx[0x4800d28c] Rx[0x4801cb3a]
--- 0x4800d28c: transport_drv_sta_tx at /home/ferry/workpro/phone_wt99p4c5_s1_board/managed_components/espressif__esp_hosted/host/drivers/transport/transport_drv.c:219
--- 0x4801cb3a: esp_wifi_remote_channel_rx at /home/ferry/workpro/phone_wt99p4c5_s1_board/managed_components/espressif__esp_wifi_remote/esp_wifi_remote_net.c:19

I (2119) transport: Add ESP-Hosted channel IF[2]: S[0] Tx[0x4800d1d2] Rx[0x4801cb3a]
--- 0x4800d1d2: transport_drv_ap_tx at /home/ferry/workpro/phone_wt99p4c5_s1_board/managed_components/espressif__esp_hosted/host/drivers/transport/transport_drv.c:249
--- 0x4801cb3a: esp_wifi_remote_channel_rx at /home/ferry/workpro/phone_wt99p4c5_s1_board/managed_components/espressif__esp_wifi_remote/esp_wifi_remote_net.c:19

I (2127) main_task: Started on CPU0
I (2130) main_task: Calling app_main()
I (2218) WT99P4C5_S1_BOARD: Partition size: total: 3848081, used: 3217067
I (2218) app_main: SPIFFS mount successfully
W (2218) ldo: The voltage value 0 is out of the recommended range [500, 2700]
W (2224) WT99P4C5_S1_BOARD: Warning: Long filenames on SD card are disabled in menuconfig!
I (2232) sdmmc_periph: sdmmc_host_init: SDMMC host already initialized, skipping init flow
I (2407) app_main: SD card mount successfully
W (2407) i2c.master: Please check pull-up resistances whether be connected properly. Otherwise unexpected behavior would happen. For more detailed information, please read docs
W (2416) i2s_common: dma frame num is adjusted to 256 to align the dma buffer with 64, bufsize = 512
W (2425) i2s_common: dma frame num is adjusted to 256 to align the dma buffer with 64, bufsize = 512
I (2438) ES8311: Work in Slave mode
I (2445) ES8311: Work in Slave mode
I (2448) I2S_IF: channel mode 0 bits:16/16 channel:2 mask:3
I (2448) I2S_IF: STD Mode 1 bits:16/16 channel:2 sample_rate:16000 mask:3
I (2452) I2S_IF: channel mode 0 bits:16/16 channel:2 mask:3
I (2457) I2S_IF: STD Mode 0 bits:16/16 channel:2 sample_rate:16000 mask:3
I (2478) Adev_Codec: Open codec device OK
I (2478) I2S_IF: channel mode 0 bits:16/16 channel:2 mask:3
I (2478) I2S_IF: STD Mode 0 bits:16/16 channel:2 sample_rate:16000 mask:3
I (2496) Adev_Codec: Open codec device OK
I (2496) app_main: Codec init successfully
I (2537) esp_eth.netif.netif_glue: 30:ed:a0:e0:ca:b1
I (2537) esp_eth.netif.netif_glue: ethernet attached to netif
I (6538) WT99P4C5_S1_BOARD: Ethernet Started
I (6538) app_main: Ethernet init successfully
I (6538) LVGL: Starting LVGL task
I (6609) GT911: TouchPad_ID:0x39,0x31,0x31
I (6609) GT911: TouchPad_Config_Version:70
W (6609) ledc: GPIO 20 is not usable, maybe conflict with others
I (6611) WT99P4C5_S1_BOARD: MIPI DSI PHY Powered on
I (6617) WT99P4C5_S1_BOARD: Install MIPI DSI LCD control panel
I (6621) WT99P4C5_S1_BOARD: Install EK79007 LCD control panel
I (6627) ek79007: version: 1.0.2
I (6783) WT99P4C5_S1_BOARD: Display initialized
I (6783) WT99P4C5_S1_BOARD: Display resolution 1024x600
E (6784) lcd_panel: esp_lcd_panel_swap_xy(50): swap_xy is not supported by this panel
I (6788) WT99P4C5_S1_BOARD: Setting LCD backlight: 100%
I (6793) app_main: Display ESP-Brookesia phone demo
[WARN] [esp_brookesia_core.cpp:46](getDisplaySize): Display is not set, use default display
[INFO] [esp_brookesia_core.cpp:204](beginCore): Library version: 0.4.1
[WARN] [esp_brookesia_phone_manager.cpp:72](begin): No touch device is set, try to use default touch device
[WARN] [esp_brookesia_phone_manager.cpp:76](begin): Using default touch device(@0x0x485dedac)
I (6895) file_iterator: File : BGM 1.mp3
I (6901) file_iterator: File : For Elise.mp3
I (6907) file_iterator: File : Something.mp3
I (6913) file_iterator: File : Waka Waka.mp3
I (6923) file_iterator: File : BGM 2.mp3
I (6924) EUI_Setting: Load ble_en: 0
I (6924) EUI_Setting: Load brightness: 95
I (6924) EUI_Setting: Load volume: 60
I (6926) EUI_Setting: Load wifi_en: 0
I (6929) bsp_extra_board: Setting volume: 60
I (6933) WT99P4C5_S1_BOARD: Setting LCD backlight: 95%
I (6939) transport: Attempt connection with slave: retry[0]
I (6943) transport: Reset slave using GPIO[54]
I (6947) os_wrapper_esp: GPIO [54] configured
I (7023) file_iterator: File : normal.mp3
I (7024) file_iterator: File : weak.mp3
I (7024) file_iterator: File : good.mp3
I (7024) file_iterator: File : excellent.mp3
I (7027) Game2048: Load score: 0
I (7032) ov5647: Detected Camera sensor PID=0x5647
I (7097) app_video: version: 0.8.0
I (7097) app_video: driver:  MIPI-CSI
I (7097) app_video: card:    MIPI-CSI
I (7097) app_video: bus:     esp32p4:MIPI-CSI
I (7100) app_video: width=1280 height=960
I (7120) app_camera_pipeline: new elements[0]:0x49198d04, internal:1
I (7136) app_camera_pipeline: new elements[1]:0x493f8d08, internal:1
I (7152) app_camera_pipeline: new elements[2]:0x49658d0c, internal:1
I (7168) app_camera_pipeline: new elements[3]:0x498b8d10, internal:1
I (7168) app_camera_pipeline: new pipeline 0x481c252c, elem_num:4
I (7169) app_camera_pipeline: new elements[0]:0x481c263c, internal:1
I (7175) app_camera_pipeline: new elements[1]:0x481c2690, internal:1
I (7181) app_camera_pipeline: new elements[2]:0x481c26e4, internal:1
I (7187) app_camera_pipeline: new elements[3]:0x481c2738, internal:1
I (7193) app_camera_pipeline: new pipeline 0x481c25b4, elem_num:4
I (7244) MEM:    Biggest /     Free /    Total
          SRAM : [136 / 180 / 400] KB
         PSRAM : [4992 / 5074 / 26851] KB
I (8627) sdio_wrapper: SDIO master: Slot 1, Data-Lines: 4-bit Freq(KHz)[40000 KHz]
I (8627) sdio_wrapper: GPIOs: CLK[18] CMD[19] D0[14] D1[15] D2[16] D3[17] Slave_Reset[54]
I (8627) H_SDIO_DRV: Starting SDIO process rx task
I (8631) sdio_wrapper: Queues: Tx[20] Rx[20] SDIO-Rx-Mode[1]
Name: 
Type: SDIO
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 0MB
CSD: ver=1, sector_size=0, capacity=0 read_bl_len=0
SCR: sd_spec=0, bus_width=0
TUPLE: DEVICE, size: 3: D9 01 FF 
TUPLE: MANFID, size: 4
  MANF: 0092, CARD: 6666
TUPLE: FUNCID, size: 2: 0C 00 
TUPLE: FUNCE, size: 4: 00 00 02 32 
TUPLE: CONFIG, size: 5: 01 01 00 02 07 
TUPLE: CFTABLE_ENTRY, size: 8
  INDX: C1, Intface: 1, Default: 1, Conf-Entry-Num: 1
  IF: 41
  FS: 30, misc: 0, mem_space: 1, irq: 1, io_space: 0, timing: 0, power: 0
  IR: 30, mask: 1,   IRQ: FF FF
  LEN: FFFF
TUPLE: END
I (8714) sdio_wrapper: Function 0 Blocksize: 512
I (8718) sdio_wrapper: Function 1 Blocksize: 512
I (8722) H_SDIO_DRV: SDIO Host operating in STREAMING MODE
I (8727) H_SDIO_DRV: generate slave intr
I (8739) transport: Received INIT event from ESP32 peripheral
I (8739) transport: EVENT: 12
I (8739) transport: EVENT: 11
I (8742) transport: capabilities: 0x1
I (8745) transport: Features supported are:
I (8749) transport:      * WLAN
I (8752) transport: EVENT: 13
I (8754) transport: ESP board type is : 23 

I (8759) transport: Base transport is set-up

I (8763) transport: Slave chip Id[12]
I (8766) hci_stub_drv: Host BT Support: Disabled
I (8771) H_SDIO_DRV: Received INIT event
I (8820) rpc_wrap: --- ESP Event: Slave ESP Init ---
I (10485) EUI_Setting: wifi_init done
I (12244) MEM:    Biggest /     Free /    Total
          SRAM : [136 / 177 / 400] KB
         PSRAM : [4992 / 5074 / 26851] KB
I (17244) MEM:    Biggest /     Free /    Total
          SRAM : [136 / 177 / 400] KB
         PSRAM : [4992 / 5074 / 26851] KB
I (22244) MEM:    Biggest /     Free /    Total
          SRAM : [136 / 177 / 400] KB
         PSRAM : [4992 / 5074 / 26851] KB
```

- ESP32C5
```bash
ESP-ROM:esp32c5-eco2-20250121
Build:Jan 21 2025
rst:0x1 (POWERON),boot:0x38 (SPI_FAST_FLASH_BOOT)
SPI mode:DIO, clock div:1
load:0x408556b0,len:0x5f8
load:0x4084bba0,len:0xb1c
--- 0x4084bba0: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

load:0x4084e5a0,len:0x2bd4
--- 0x4084e5a0: esp_flash_encryption_enabled at /home/ferry/esp/idf55/components/bootloader_support/src/flash_encrypt.c:89

entry 0x4084bba0
--- 0x4084bba0: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

I (39) cpu_start: Unicore app
W (42) clk: esp_perip_clk_init() has not been implemented yet
I (47) cpu_start: Pro cpu start user code
I (47) cpu_start: cpu freq: 240000000 Hz
I (48) app_init: Application information:
I (48) app_init: Project name:     network_adapter
I (52) app_init: App version:      0c4aa5f
I (55) app_init: Compile time:     Jul  4 2025 18:20:35
I (60) app_init: ELF file SHA256:  0802c74bb...
I (65) app_init: ESP-IDF:          v5.5-beta1-204-gcbe9388f45
I (70) efuse_init: Min chip rev:     v1.0
I (74) efuse_init: Max chip rev:     v1.99 
I (78) efuse_init: Chip rev:         v1.0
I (82) heap_init: Initializing. RAM available for dynamic allocation:
I (88) heap_init: At 4082B500 len 000310A0 (196 KiB): RAM
I (93) heap_init: At 4085C5A0 len 00002F58 (11 KiB): RAM
I (98) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
I (104) spi_flash: detected chip: generic
I (107) spi_flash: flash io: dio
W (110) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header.
I (122) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (128) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (135) coexist: coex firmware version: 7b9a184
I (153) coexist: coexist rom version 78e5c6e42
I (154) main_task: Started on CPU0
I (154) main_task: Calling app_main()
I (154) fg_mcu_slave: *********************************************************************
I (161) fg_mcu_slave:                 ESP-Hosted-MCU Slave FW version :: 2.0.13                        
I (170) fg_mcu_slave:                 Transport used :: SDIO only                     
I (178) fg_mcu_slave: *********************************************************************
I (186) fg_mcu_slave: Supported features are:
I (190) fg_mcu_slave: - WLAN over SDIO
I (193) h_bt: - BT/BLE
I (195) h_bt:    - HCI Over SDIO
I (198) h_bt:    - BLE only
I (201) fg_mcu_slave: capabilities: 0xd
I (204) fg_mcu_slave: Supported extended features are:
I (209) h_bt: - BT/BLE (extended)
I (212) fg_mcu_slave: extended capabilities: 0x0
I (231) h_bt: ESP Bluetooth MAC addr: 30:ed:a0:e4:10:1a
I (231) BLE_INIT: Using main XTAL as clock source
I (231) BLE_INIT: ble controller commit:[35fe65f]
W (236) BLE_INIT: BLE modem sleep is enabled
I (237) BLE_INIT: Bluetooth MAC: 30:ed:a0:e4:10:1a
I (242) phy_init: phy_version 102,171bf417,Jun 12 2025,15:57:12
I (808) phy: libbtbb version: 09fb4d6, Jun 12 2025, 15:57:24
I (809) SDIO_SLAVE: Using SDIO interface
I (809) SDIO_SLAVE: sdio_init: sending mode: SDIO_SLAVE_SEND_STREAM
I (812) SDIO_SLAVE: sdio_init: ESP32 SDIO TxQ[20] timing[0]

ESP-ROM:esp32c5-eco2-20250121
Build:Jan 21 2025
rst:0x1 (POWERON),boot:0x38 (SPI_FAST_FLASH_BOOT)
SPI mode:DIO, clock div:1
load:0x408556b0,len:0x5f8
load:0x4084bba0,len:0xb1c
--- 0x4084bba0: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

load:0x4084e5a0,len:0x2bd4
--- 0x4084e5a0: esp_flash_encryption_enabled at /home/ferry/esp/idf55/components/bootloader_support/src/flash_encrypt.c:89

entry 0x4084bba0
--- 0x4084bba0: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

I (35) cpu_start: Unicore app
W (38) clk: esp_perip_clk_init() has not been implemented yet
I (43) cpu_start: Pro cpu start ESP-ROM:esp32c5-eco2-20250121
Build:Jan 21 2025
rst:0x1 (POWERON),boot:0x38 (SPI_FAST_FLASH_BOOT)
SPI mode:DIO, clock div:1
load:0x408556b0,len:0x5f8
load:0x4084bba0,len:0xb1c
--- 0x4084bba0: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

load:0x4084e5a0,len:0x2bd4
--- 0x4084e5a0: esp_flash_encryption_enabled at /home/ferry/esp/idf55/components/bootloader_support/src/flash_encrypt.c:89

entry 0x4084bba0
--- 0x4084bba0: call_start_cpu0 at /home/ferry/esp/idf55/components/bootloader/subproject/main/bootloader_start.c:25

I (35) cpu_start: Unicore app
W (38) clk: esp_perip_clk_init() has not been implemented yet
I (43) cpu_start: Pro cpu start user code
I (43) cpu_start: cpu freq: 240000000 Hz
I (44) app_init: Application information:
I (44) app_init: Project name:     network_adapter
I (48) app_init: App version:      0c4aa5f
I (52) app_init: Compile time:     Jul  4 2025 18:20:35
I (57) app_init: ELF file SHA256:  0802c74bb...
I (61) app_init: ESP-IDF:          v5.5-beta1-204-gcbe9388f45
I (66) efuse_init: Min chip rev:     v1.0
I (70) efuse_init: Max chip rev:     v1.99 
I (74) efuse_init: Chip rev:         v1.0
I (78) heap_init: Initializing. RAM available for dynamic allocation:
I (84) heap_init: At 4082B500 len 000310A0 (196 KiB): RAM
I (89) heap_init: At 4085C5A0 len 00002F58 (11 KiB): RAM
I (94) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
I (100) spi_flash: detected chip: generic
I (103) spi_flash: flash io: dio
W (106) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header.
I (118) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (124) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (131) coexist: coex firmware version: 7b9a184
I (149) coexist: coexist rom version 78e5c6e42
I (150) main_task: Started on CPU0
I (150) main_task: Calling app_main()
I (150) fg_mcu_slave: *********************************************************************
I (157) fg_mcu_slave:                 ESP-Hosted-MCU Slave FW version :: 2.0.13                        
I (166) fg_mcu_slave:                 Transport used :: SDIO only                     
I (174) fg_mcu_slave: *********************************************************************
I (182) fg_mcu_slave: Supported features are:
I (186) fg_mcu_slave: - WLAN over SDIO
I (189) h_bt: - BT/BLE
I (191) h_bt:    - HCI Over SDIO
I (194) h_bt:    - BLE only
I (197) fg_mcu_slave: capabilities: 0xd
I (200) fg_mcu_slave: Supported extended features are:
I (205) h_bt: - BT/BLE (extended)
I (208) fg_mcu_slave: extended capabilities: 0x0
I (227) h_bt: ESP Bluetooth MAC addr: 30:ed:a0:e4:10:1a
I (227) BLE_INIT: Using main XTAL as clock source
I (227) BLE_INIT: ble controller commit:[35fe65f]
W (232) BLE_INIT: BLE modem sleep is enabled
I (233) BLE_INIT: Bluetooth MAC: 30:ed:a0:e4:10:1a
I (238) phy_init: phy_version 102,171bf417,Jun 12 2025,15:57:12
I (802) phy: libbtbb version: 09fb4d6, Jun 12 2025, 15:57:24
I (803) SDIO_SLAVE: Using SDIO interface
I (803) SDIO_SLAVE: sdio_init: sending mode: SDIO_SLAVE_SEND_STREAM
I (806) SDIO_SLAVE: sdio_init: ESP32 SDIO TxQ[20] timing[0]

I (1676) fg_mcu_slave: Start Data Path
I (1683) fg_mcu_slave: Initial set up done
I (1683) slave_ctrl: event ESPInit
mem_dump free:82116 min-free:82064 lfb-dma:65536 lfb-def:65536 lfb-8bit:65536
I (1732) fg_mcu_slave: Slave init_config received from host
I (1732) fg_mcu_slave: Host capabilities: 44
I (1732) fg_mcu_slave: ESP<-Host high data throttle threshold [80%]
I (1736) fg_mcu_slave: ESP<-Host low data throttle threshold [60%]
I (2497) slave_ctrl: Resp_MSGId for req[0x116] is [0x216], uid 1
I (2497) slave_ctrl: Received Req [0x116]
I (2497) pp: pp rom version: 78a72e9d5
I (2499) net80211: net80211 rom version: 78a72e9d5
I (2505) wifi:wifi driver task: 4084ee70, prio:23, stack:6656, core=0
I (2518) wifi:wifi firmware version: 2dcd4f5
I (2518) wifi:wifi certification version: v7.0
I (2518) wifi:config NVS flash: enabled
I (2521) wifi:config nano formatting: disabled
I (2525) wifi:mac_version:HAL_MAC_ESP32AX_752MP_ECO2,ut_version:N, band mode:0x3
I (2533) wifi:Init data frame dynamic rx buffer num: 32
I (2537) wifi:Init static rx mgmt buffer num: 5
I (2541) wifi:Init management short buffer num: 32
I (2546) wifi:Init dynamic tx buffer num: 32
I (2550) wifi:Init static tx FG buffer num: 2
I (2554) wifi:Init static rx buffer size: 1700 (rxctrl:64, csi:512)
I (2560) wifi:Init static rx buffer num: 10
I (2564) wifi:Init dynamic rx buffer num: 32
I (2569) wifi_init: rx ba win: 6
I (2571) wifi_init: accept mbox: 6
I (2574) wifi_init: tcpip mbox: 32
I (2577) wifi_init: udp mbox: 6
I (2580) wifi_init: tcp mbox: 6
I (2583) wifi_init: tcp tx win: 5760
I (2586) wifi_init: tcp rx win: 5760
I (2589) wifi_init: tcp mss: 1440
I (2592) wifi_init: WiFi IRAM OP enabled
I (2596) wifi_init: WiFi RX IRAM OP enabled
I (2600) wifi_init: WiFi SLP IRAM OP enabled
I (2606) slave_ctrl: Resp_MSGId for req[0x104] is [0x204], uid 2
I (2610) slave_ctrl: Received Req [0x104]
I (2615) slave_ctrl: Resp_MSGId for req[0x118] is [0x218], uid 3
I (2619) slave_ctrl: Received Req [0x118]
W (2625) wifi:WDEV_RXCCK_DELAY:960
W (2626) wifi:WDEV_RXOFDM_DELAY:264
W (2629) wifi:WDEV_RX_11G_OFDM_DELAY:265
W (2633) wifi:WDEV_TXCCK_DELAY:630
W (2636) wifi:WDEV_TXOFDM_DELAY:94
W (2639) wifi:ACK_TAB0   :0x   90a0b, QAM16:0x9 (24Mbps), QPSK:0xa (12Mbps), BPSK:0xb (6Mbps)
W (2648) wifi:CTS_TAB0   :0x   90a0b, QAM16:0x9 (24Mbps), QPSK:0xa (12Mbps), BPSK:0xb (6Mbps)
W (2656) wifi:WDEVBEAMFORMCONF:0x61d7120, HE_BF_RPT_RA_SET_OPT:1
W (2661) wifi:WDEVVHTBEAMFORMCONF: 0x61d7120, WDEV_VHT_BEAMFORMEE_ENA: 1, WDEV_VHT_NG_SEL: 0
W (2670) wifi:(agc)0x600a7128:0xd21f0c20, min.avgNF:0xce->0xd2(dB), RCalCount:0x1f0, min.RRssi:0xc20(-62.00)
W (2679) wifi:MODEM_SYSCON_WIFI_BB_CFG_REG(0x600a9c18):0x10003802
W (2685) wifi:(phy)rate:0x0(  LP-1Mbps), pwr:20, txing:20
W (2690) wifi:(phy)rate:0x1(  LP-2Mbps), pwr:20, txing:20
W (2695) wifi:(phy)rate:0x2(LP-5.5Mbps), pwr:20, txing:20
W (2700) wifi:(phy)rate:0x3( LP-11Mbps), pwr:20, txing:20
W (2705) wifi:(phy)rate:0x5(  SP-2Mbps), pwr:20, txing:20
W (2711) wifi:(phy)rate:0x6(SP-5.5Mbps), pwr:20, txing:20
W (2716) wifi:(phy)rate:0x7( SP-11Mbps), pwr:20, txing:20
W (2721) wifi:(phy)rate:0x8(    48Mbps), pwr:17, txing:17
W (2726) wifi:(phy)rate:0x9(    24Mbps), pwr:19, txing:19
W (2731) wifi:(phy)rate:0xa(    12Mbps), pwr:19, txing:19
W (2736) wifi:(phy)rate:0xb(     6Mbps), pwr:19, txing:19
W (2741) wifi:(phy)rate:0xc(    54Mbps), pwr:17, txing:17
W (2746) wifi:(phy)rate:0xd(    36Mbps), pwr:19, txing:19
W (2752) wifi:(phy)rate:0xe(    18Mbps), pwr:19, txing:19
W (2757) wifi:(phy)rate:0xf(     9Mbps), pwr:19, txing:19
W (2762) wifi:(phy)rate:0x10, mcs:0x0, pwr(bw20:19, bw40:18), txing:19, HE pwr(bw20:19), txing:19
W (2770) wifi:(phy)rate:0x11, mcs:0x1, pwr(bw20:19, bw40:18), txing:19, HE pwr(bw20:19), txing:19
W (2779) wifi:(phy)rate:0x12, mcs:0x2, pwr(bw20:18, bw40:17), txing:18, HE pwr(bw20:18), txing:18
W (2788) wifi:(phy)rate:0x13, mcs:0x3, pwr(bw20:18, bw40:17), txing:18, HE pwr(bw20:18), txing:18
W (2796) wifi:(phy)rate:0x14, mcs:0x4, pwr(bw20:17, bw40:16), txing:17, HE pwr(bw20:17), txing:17
W (2805) wifi:(phy)rate:0x15, mcs:0x5, pwr(bw20:17, bw40:16), txing:17, HE pwr(bw20:17), txing:17
W (2813) wifi:(phy)rate:0x16, mcs:0x6, pwr(bw20:17, bw40:16), txing:17, HE pwr(bw20:17), txing:17
W (2822) wifi:(phy)rate:0x17, mcs:0x7, pwr(bw20:17, bw40:16), txing:17, HE pwr(bw20:17), txing:17
W (2831) wifi:(phy)rate:0x18, mcs:0x8, pwr(bw20:19, bw40:16), txing:19, HE pwr(bw20:16), txing:16
W (2839) wifi:(phy)rate:0x19, mcs:0x9, pwr(bw20:18, bw40:16), txing:18, HE pwr(bw20:15), txing:15
W (2848) wifi:(hal)co_hosted_bss:0, max_indicator:0, bitmask:0xff, mBSSIDsEnable:0
I (2855) wifi:11ax coex: WDEVAX_PTI0(0x55777555), WDEVAX_PTI1(0x00003377).

I (2862) wifi:mode : sta (30:ed:a0:e4:10:18)
I (2866) wifi:enable tsf
W (2868) wifi:(BB)enable busy check(0x18), disable idle check(0xaa)
I (2875) slave_ctrl: Sending Wi-Fi event [43]
I (2878) slave_ctrl: Sending Wi-Fi event [2]
I (2883) slave_ctrl: Resp_MSGId for req[0x101] is [0x201], uid 4
I (2888) slave_ctrl: Received Req [0x101]
I (2892) slave_ctrl: mac [30:ed:a0:e4:10:18]
mem_dump free:40644 min-free:40288 lfb-dma:23552 lfb-def:23552 lfb-8bit:23552
mem_dump free:40644 min-free:40288 lfb-dma:23552 lfb-def:23552 lfb-8bit:23552
mem_dump free:40644 min-free:40288 lfb-dma:23552 lfb-def:23552 lfb-8bit:23552
```

---
