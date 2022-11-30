[![HTL Salzburg](https://img.shields.io/badge/HTBLuVA-Elektronik%20und%20Technische%20Informatik-8a2be2)](http://www.htl-salzburg.ac.at/startseite.html)
[![Built with: platformio](https://img.shields.io/badge/built%20with-platformio-orange)](https://platformio.org/)
[![Drawn with: Diagrams](https://img.shields.io/badge/drawn%20with-diagrams.net-ff8c00)](https://app.diagrams.net/)

# MTAP-MIDI-Guitar-Converter [DE]
<picture>
 <source media="(prefers-color-scheme: dark)" srcset="https://github.com/s-grundner/MTAP-MIDI-Guitar-Converter/blob/main/documentation/images/LOGO-dark.png?raw=true" align="right" width="450px" height="160px"/>
 <img alt="Logo" src="https://github.com/s-grundner/MTAP-MIDI-Guitar-Converter/blob/main/documentation/images/LOGO-light.png?raw=true" align="right" width="450px" height="160px"/>
</picture>

## Kurzberschreibung
Das vorliegende Projekt ermöglicht die Verwendung einer E-Gitarre als MIDI-Device. Das Gerät soll einzelne Noten und Akkorde zuverlässig mit möglichst geringer Latenz in das MIDI-Format umwandeln. Die MIDI-Signale werden an die USB-Schnittstelle eines PCs übertragen, von welchem die Signale beliebig interpretiert werden können.

## Vorhaben, Lösungsweg und Innovation
Virtuelle Instrumente sind in der modernen Musik-Produktion aufgrund ihrer Vielseitigkeit weit verbreitet. Das Projekt, ein Audiodaten-Verarbeitungssystem, macht es möglich, diese virtuellen Instrumente auch mit einer E-Gitarre zu spielen. Hierfür wird das analoge Audiosignal direkt von der Ausgangsbuchse der Gitarre abgegriffen, in MIDI-Noten umgewandelt und via USB an eine Digital Audio Workstation (DAW) übertragen.

Die Ein- und Ausgänge des Systems umfassen eine Micro-USB-Buchse und zwei Mono-Audio Buchsen. Die analogen Signale werden zunächst an einer der Buchsen abgegriffen, digitalisiert und von einem leistungsstarken 32-Bit Prozessor analysiert. Die Daten, die sich aus der Analyse ergeben, werden anschließend in das MIDI Format konvertiert und über die USB-Schnittstelle ausgegeben. An der anderen Buchse wird das Gitarrensignal unverändert wiedergegeben, um eine Weiterverwendung zu ermöglichen.

Der Vorteil des MIDI-Formats ist, dass es der Standard vieler Musikproduktionsumgebungen ist. In der DAW wird das MIDI-Signal als Note auf einem Klavier interpretiert, welches dann in beliebige virtuelle Instrumente eingespeist werden kann und somit einen benutzerdefinierten Ton abspielt. Außerdem hat das Abgreifen der unveränderten Gitarrenschwingung den Zweck, dass parallel zur Analyse eine Aufnahme des Rohsignals erfolgen kann.

## Projektstatus
Das Projekt befindet sich derzeit in der frühen Entwicklungsphase. Funktionalitäten sind noch in Planung
Planungsvorgänge können unter dem Reiter *[Projects](https://github.com/s-grundner/MTAP-MIDI-Guitar-Converter/projects?query=is%3Aopen)* eingesehen werden.

## Mitwirkende
[Simon Grundner](https://github.com/s-grundner)

[Laurenz Hölzl](https://github.com/Laurenz03)

[Daniel Bräumann](https://github.com/Hexewillfred)

## Repository Struktur
- [**Lastenheft**](documentation/Lastenheft.md)
- [**Pflichtenheft**](documentation/Pflichtenheft.md)
- [**Hardware DOCS**](documentation/hw-docs)
- [**Software DOCS**](documentation/sw-docs)
