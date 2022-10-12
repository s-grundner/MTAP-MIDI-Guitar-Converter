[![HTL Salzburg](https://img.shields.io/badge/HTBLuVA-Elektronik%20und%20Technische%20Informatik-8a2be2)](http://www.htl-salzburg.ac.at/startseite.html)
[![Built with: platformio](https://img.shields.io/badge/built%20with-platformio-orange)](https://platformio.org/)
[![Drawn with: Diagrams](https://img.shields.io/badge/drawn%20with-diagrams.net-ff8c00)](https://app.diagrams.net/)


# MTAP-MIDI-Guitar-Converter 
Das MIDI-GC ist ein DSP Device mit welchem es möglich ist, Analoge signale eines Gitarren Pickups in MIDI Signale/Noten umzwandeln und an einen PC via USB zu senden. Die MIDI Signale können an einer Digital Audio Workstation verarbeitet werden.
Es gibt eine Line IN für das Gitarren Signal, sowie eine USB Buchse mit der die Digitalen Signale an den PC übertragen werden und Das MIDI-GC mit 5V versorgt wird.
Der Line In kann außerdem unverändert an einer 6.3mm Buchse abgegriffen werden, falls man das Gitarren Signal noch weiter verwenden will.

(evtl Mod-Bands)

## Technische Details und Überlegungen zur Umsetzung
- Vor der DSP:
	- Filtern auf Frequenzbereich des Gitarrensignals
	- Vorverstärker
- DSP: 16/24bit ADC mit samplerate 44.1/48/192 kHz

# Arbeitsaufteilung

## Firmware

- Digital Audio input

- FFT

- MIDI Conversion

	-> [MIDI Standards](https://mitxela.com/other/ca33.pdf)
	
	-> [MIDI Data Table](https://www.midi.org/specifications-old/item/table-2-expanded-messages-list-status-bytes)
	
- MIDI Transmission via USART

## Software
[MIDI to Serial](https://projectgus.github.io/hairless-midiserial/)

[MIDI loopback](https://mitxela.com/other/ca33.pdf)

## Hardware
### Embedded
### Analog

# FFT
[arduinoFFT](https://github.com/kosme/arduinoFFT)

[FFT lib](https://github.com/yash-sanghvi/ESP32/blob/master/FFT_on_ESP32_Arduino/FFT.h)

# Quellen
[Repository](https://github.com/s-grundner/MTAP-MIDI-Expression-Pedal)
