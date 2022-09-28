# GitCon - E-Gitarren zu MIDI Converter
## Ausgangslage
Digitale Instrumente sind in der modernen Musik Produktion aufgrund ihrer Versatilität weit verbreitet. Das Projekt macht es möglich, diese Digitalen Instrumente auch mit einer E-Gitarre spielen zu können. Hierfür werden die Analogen Signal in MIDI-Noten umgewandelt. 

## Untersuchungsanliegen der individuellen Themenstellungen (Diplomarbeit/Abschlussarbeit-Teilthemen)
Daniel Bräumann übernimmt die Entwicklung des Gehäuses und den teil der Hardware, welche für die Aufbereitung des analogen Signals notwendig ist bevor es digital Verarbeitet werden kann. Dies umfasst sämtliche Filter und Verstärker.

Simon Grundner übernimmt die Entwicklung des Embedded Systems und die Implementierung der Anbindung an eine Digital Audio Workstation. Dazu zählen auch der Entwurf, Entwicklung, Fertigung, Inbetriebnahme und Testung der Schaltungen.

Laurenz Hölzl übernimmt die Firmware, welche die für Digitale Signalverarbeitung auf dem Microprozessor notwendig ist.

## Zeilsetzung
Es soll ermöglicht werden eine E-Gitarre als MIDI Device zu verwenden. 
Das Gerät soll einzelne Noten und Akkorde zuverlässig umwandeln.
Die Latenz soll möglichst gering sein.

## Geplantes Ergebnis der individuellen Themenstellungen
Ein leistungssarker Mikroprozessor soll eine zuverlässige FFT des Gitarrensignals durchführen.
Die ausgelesenen Frequenzen werden im Mikrocontroller in MIDI-Noten umgewandelt und anschließend über den USB-Port ausgegeben.
Am PC erkennt eine Digital Audio Workstation das MIDI-Device und ist bereit, die Daten zu verarbeiten.
