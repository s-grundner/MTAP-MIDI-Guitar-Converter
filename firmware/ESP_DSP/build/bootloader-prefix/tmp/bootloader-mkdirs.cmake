# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/laurenz/esp/esp-idf/components/bootloader/subproject"
  "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader"
  "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader-prefix"
  "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader-prefix/tmp"
  "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader-prefix/src/bootloader-stamp"
  "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader-prefix/src"
  "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
