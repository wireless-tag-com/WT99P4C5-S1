# Install script for directory: /home/abing/Desktop/qm-wt99p4/esp-idf

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/abing/.espressif/tools/riscv32-esp-elf/esp-14.2.0_20241119/riscv32-esp-elf/bin/riscv32-esp-elf-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/riscv/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_driver_gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_pm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/bootloader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esptool_py/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/partition_table/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_app_format/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_bootloader_format/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/app_update/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_partition/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/efuse/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/bootloader_support/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_mm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/spi_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_system/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_common/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_rom/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/hal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/log/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/heap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_security/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_hw_support/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/freertos/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/newlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/pthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/cxx/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/wear_levelling/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/sdmmc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_driver_sdmmc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_ringbuf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_driver_spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_driver_sdspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_driver_uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_driver_usb_serial_jtag/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_vfs_console/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/vfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/console/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_event/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/lwip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_netif_stack/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/esp_netif/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/espressif__tinyusb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/usb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/espressif__esp_tinyusb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/abing/Desktop/qm-wt99p4/esp-idf/examples/peripherals/usb/device/tusb_msc/build/esp-idf/main/cmake_install.cmake")
endif()

