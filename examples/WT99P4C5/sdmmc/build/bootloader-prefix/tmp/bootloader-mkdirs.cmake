# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/abing/Desktop/qm-wt99p4/esp-idf/components/bootloader/subproject"
  "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader"
  "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix"
  "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix/tmp"
  "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix/src/bootloader-stamp"
  "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix/src"
  "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/abing/Desktop/qm-wt99p4/esp-idf/examples/storage/sd_card/sdmmc/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
