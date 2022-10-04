# GitCon - E-Gitarren zu MIDI Converter 
## Ausgangslage
Digitale Instrumente sind in der modernen Musik-Produktion aufgrund ihrer Versatilität weit verbreitet. Das Projekt macht es möglich, diese digitalen Instrumente auch mit einer E-Gitarre zu spielen. Hierfür werden die analogen Signale in MIDI-Noten umgewandelt und via USB an eine DAW übertragen.
## Projektteam (Arbeitsaufwand)

| Name            | Individuelle Themenstellung                                       |
| --------------- | ----------------------------------------------------------------- |
| Simon Grundner  | Entwicklung der Hardware Frontend-Platine für den Microcontroller | 
| Daniel Bräumann | Entwurf der Analogen Signalverarbeitungskette                     |
| Laurenz Hölzl   | Implementierung der Digitalen Signalverarbeitung in die Firmware  |

## Untersuchungsanliegen der Individuellen Themenstellung
**Daniel Bräumann** Implementiert den Teil der Hardware, welcher für die Aufbereitung des Gitarrensignals zuständig ist. Dies umfasst sämtliche Filterstufen und Verstärker. Außerdem widmet er sich der Gehäusekonstruktion.

**Simon Grundner** übernimmt die Entwicklung des Mainboards und die Implementierung der Anbindung an eine Digital Audio Workstation. Dazu zählen auch Entwurf, Entwicklung, Inbetriebnahme und Testung der Schaltungen.

**Laurenz Hölzl** entwickelt die Firmware, welche für die Digitale Signalverarbeitung auf dem Microprozessor notwendig ist.

## Zielsetzung
Das Vorliegende Projekt ermöglicht die Verwendung, einer E-Gitarre als MIDI-Device.
Das Gerät soll einzelne Noten und Akkorde, mit möglichst geringer Latenz, zuverlässig in das MIDI-Format umwandeln.

## Geplantes Ergebnis der Prüfungskandidaten
**D B** Entwurf und Implementierung der analogen Signalverarbeitungskette bestehend aus Tiefpassfilter und Vorverstärker

**L H** Entwurf der Software, um mittels eines FFT Algorithmus die Musiknoten zu ermitteln 

**S G** Die umgewandelte Note soll anschließend im MIDI Format via USART and ein USB-Gateway ansteuern, welches die Verbindung zum PC herstellt


---