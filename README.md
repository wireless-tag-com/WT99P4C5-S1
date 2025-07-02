# WT99P4C5-S1 Example Project

## Project Overview

This is a sample project based on the WT99P4C5_S1 development board, featuring a smartphone-like user interface built with the ESP-Brookesia UI framework. The project integrates various application functions including audio/video playback, camera, games, calculator, and supports computer vision features such as face detection and pedestrian detection.

## Key Features

- 🎯 **Smartphone-style UI Interface** - Built on ESP-Brookesia framework
- 📱 **Multiple Applications** - Calculator, music player, video player, 2048 game, camera app
- 🤖 **AI Vision Features** - Face detection and pedestrian detection
- 🖥️ **High-definition Display** - Supports MIPI DSI interface display
- 🎵 **Audio Processing** - Supports MP3 decoding and audio playback
- 🎬 **Video Processing** - Supports H.264 video decoding (currently supports MJPEG format only)
- 💾 **Multi-storage Support** - SPIFFS file system + SD card storage
- 🌐 **Network Connectivity** - WiFi and Ethernet support
- 📷 **Camera Support** - 1280x960 resolution camera

## Environment Setup

### Hardware Requirements

- **Development Board**: WT99P4C5-S1 development board
- **Display**: MIPI DSI interface display (ek79007)
- **Storage**: SD card (optional)
- **Camera**: OV5647
- **Speaker**: 3W

>[!NOTE]
>**Video Playback Instructions**
>- Save MJPEG format videos to the SD card and insert it into the SD card slot
>- Currently only supports MJPEG format videos
>- After inserting the SD card, the video player APP will automatically appear on the interface

>[!TIP]
>**Video Format Conversion Method**
>
>1. Install ffmpeg:
>```bash
>sudo apt update
>sudo apt install ffmpeg
>```
>
>2. Use ffmpeg for video conversion:
>```bash
>ffmpeg -i YOUR_INPUT_FILE_NAME.mp4 -vcodec mjpeg -q:v 2 -vf "scale=1024:600" -acodec copy YOUR_OUTPUT_FILE_NAME.mjpeg
>```

### Software Environment Setup

#### 1. Install ESP-IDF

Please install ESP-IDF v5.5 (commit: cbe9388f45dd8f33fc560c9727d429e8e107d476) or the latest version according to the official documentation:
- [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

#### 2. Clone Project

```bash
git clone <project_repository_url>
cd phone_wt99p4c5_s1_board
```

#### 3. Configure Environment Variables

```bash
. ${IDF_PATH}/export.sh
```

#### 4. Install Project Dependencies

```bash
# Install managed component dependencies
idf.py reconfigure
```

#### 5. Build and Flash

```bash
# Configure project
idf.py menuconfig

# Build project
idf.py build

# Flash to device
idf.py flash

# View serial output
idf.py monitor
```

## Project Directory Structure

```
phone_wt99p4c5_s1_board/
├── main/                           # Main program source code
│   ├── main.cpp                    # Program entry point
│   ├── CMakeLists.txt              # Main program build configuration
│   └── idf_component.yml           # Component dependency configuration
├── components/                     # Custom components
│   ├── apps/                       # Application components
│   │   ├── calculator/             # Calculator application
│   │   ├── camera/                 # Camera application
│   │   ├── game_2048/              # 2048 game
│   │   ├── music_player/           # Music player
│   │   ├── setting/                # Settings application
│   │   └── video_player/           # Video player
│   ├── human_face_detect/          # Face detection component
│   ├── pedestrian_detect/          # Pedestrian detection component
│   ├── wt99p4c5_s1_board/          # Board Support Package (BSP)
│   └── bsp_extra/                  # Additional BSP functions
├── spiffs/                         # SPIFFS file system data
│   ├── music/                      # Music files
│   └── 2048/                       # 2048 game resources
├── mp4/                            # Video file directory
├── CMakeLists.txt                  # Top-level build configuration
├── sdkconfig.defaults              # Default SDK configuration
├── partitions.csv                  # Partition table configuration
└── README.md                       # Project documentation
```

### Core Component Description

#### 1. Main Program (`main/`)
- **main.cpp**: Program entry point, initializes system, display, storage, network modules, and starts various applications

#### 2. Applications (`components/apps/`)
- **calculator/**: Calculator application supporting basic arithmetic operations
- **camera/**: Camera application supporting photo capture and video preview
- **game_2048/**: Classic 2048 number game
- **music_player/**: Music player supporting MP3 format
- **setting/**: System settings application
- **video_player/**: Video player supporting H.264 format

#### 3. AI Vision Components
- **human_face_detect/**: Face detection algorithm implementation
- **pedestrian_detect/**: Pedestrian detection algorithm implementation

#### 4. Hardware Abstraction Layer
- **wt99p4c5_s1_board/**: Board-specific BSP providing hardware initialization and driver interfaces
- **bsp_extra/**: Extended BSP function modules

#### 5. Storage and Resources
- **spiffs/**: Built-in file system storing application resources and configuration files
- **mp4/**: Video file storage directory

## Partition Configuration

>[!INFO]
>**Custom Partition Table Description (`partitions.csv`)**
>- **nvs** (24KB): Non-volatile storage for configuration data
>- **phy_init** (4KB): RF calibration data
>- **factory** (9MB): Application firmware
>- **storage** (4MB): SPIFFS file system

## Development and Debugging

### View Logs
```bash
idf.py monitor
```

>[!INFO]
>**Performance Monitoring Description**
>Built-in memory monitoring function outputs memory usage every 5 seconds:
>- SRAM usage status
>- PSRAM usage status
>- Memory leak warnings

### Common Configuration Options
Configure through `idf.py menuconfig`:
- Display parameter settings
- Camera resolution configuration
- Audio sampling rate settings
- Wi-Fi and Ethernet configuration

## Component Library Version Requirements

### Core Framework Dependencies
| Component Name | Version Requirement | Description |
|----------------|-------------------|-------------|
| **ESP-IDF** | v5.5.0 (commit: cbe9388f45dd8f33fc560c9727d429e8e107d476) | ESP32 development framework |
| **espressif/esp-brookesia** | 0.4.2 | Smartphone-style UI framework |
| **lvgl/lvgl** | 8.4.0 | Lightweight graphics library |
| **espressif/esp_lvgl_port** | 2.6.0 | LVGL porting layer |

### Video Processing Components
| Component Name | Version Requirement | Target Chips | Description |
|----------------|-------------------|--------------|-------------|
| **espressif/esp_video** | 0.8.0~3 | ESP32P4 | Video processing framework |
| **espressif/esp_h264** | 1.1.2 | ESP32S3/P4 | H.264 codec |
| **espressif/esp_jpeg** | 1.3.0 | General | JPEG image processing |
| **espressif/esp_ipa** | 0.2.0 | ESP32P4 | Image processing accelerator |

### Display and Touch Components
| Component Name | Version Requirement | Target Chips | Description |
|----------------|-------------------|--------------|-------------|
| **espressif/esp_lcd_ek79007** | 1.0.2 | ESP32P4 | EK79007 display driver |
| **espressif/esp_lcd_touch** | 1.1.2 | General | Touch screen base driver |
| **esp_lcd_touch_gt911** | 1.1.3 | General | GT911 touch controller |

### Camera Components
| Component Name | Version Requirement | Target Chips | Description |
|----------------|-------------------|--------------|-------------|
| **espressif/esp_cam_sensor** | 0.9.0 | ESP32P4 | Camera sensor driver |
| **espressif/esp_sccb_intf** | 0.0.5 | General | SCCB interface driver |

### Audio Processing Components
| Component Name | Version Requirement | Description |
|----------------|-------------------|-------------|
| **espressif/esp_codec_dev** | 1.2.0 | Audio codec device driver |
| **chmorgan/esp-audio-player** | 1.0.7 | Audio player library |
| **chmorgan/esp-libhelix-mp3** | 1.0.3 | MP3 decoder library |

### AI and Deep Learning Components
| Component Name | Version Requirement | Target Chips | Description |
|----------------|-------------------|--------------|-------------|
| **espressif/esp-dl** | 3.1.0 | ESP32S3/P4 | ESP deep learning inference framework |

### Network and Communication Components
| Component Name | Version Requirement | Target Chips | Description |
|----------------|-------------------|--------------|-------------|
| **espressif/esp_wifi_remote** | 0.14.2 | ESP32P4/H2 | WiFi remote control |
| **espressif/esp_hosted** | 2.0.13 | ESP32P4/H2 | ESP hosted mode |
| **espressif/eppp_link** | 0.3.1 | General | PPP protocol link |
| **esp_serial_slave_link** | 1.1.0~1 | General | Serial slave device link |

### Tools and Utility Components
| Component Name | Version Requirement | Description |
|----------------|-------------------|-------------|
| **espressif/cmake_utilities** | 0.5.3 | CMake build tools |
| **chmorgan/esp-file-iterator** | 1.0.0 | File iterator utility |

### Version Compatibility Notes

>[!IMPORTANT]
>**Critical Version Requirements**
>1. **ESP-IDF Version**: Recommend using v5.5 specific commit version to ensure compatibility of all components
>2. **Target Chips**: Primarily targets ESP32P4 chip, some components also support ESP32S3
>3. **Dependencies**: Some components have interdependencies, please ensure version matching

>[!CAUTION]
>**Update Notice**: When upgrading component versions, please check dependencies and compatibility to avoid incompatibility issues

### Installing Specific Version Components

To install specific versions of components, specify in `idf_component.yml`:

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