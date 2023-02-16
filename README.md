[![HTL Salzburg](https://img.shields.io/badge/HTBLuVA-Elektronik%20und%20Technische%20Informatik-8a2be2)](http://www.htl-salzburg.ac.at/startseite.html)
[![Built with: platformio](https://img.shields.io/badge/built%20with-platformio-orange)](https://platformio.org/)
[![Drawn with: Diagrams](https://img.shields.io/badge/drawn%20with-diagrams.net-ff8c00)](https://app.diagrams.net/)

# MTAP-MIDI-Guitar-Converter [DE]
<picture>
 <source media="(prefers-color-scheme: dark)" srcset="/documentation/images/MTAP-logo-weiss.png" align="right" width="450px"/>
 <img alt="Logo" src="/documentation/images/MTAP-logo-schwarz.png" align="right" width="450px"/>
</picture>

## Kurzberschreibung
Das vorliegende Projekt ermöglicht die Verwendung einer E-Gitarre als MIDI-Device. Das Gerät soll einzelne Noten und Akkorde zuverlässig mit möglichst geringer Latenz in das MIDI-Format umwandeln. Die MIDI-Signale werden an die USB-Schnittstelle eines PCs übertragen, von welchem die Signale beliebig interpretiert werden können.

## Vorhaben, Lösungsweg und Innovation
Virtuelle Instrumente sind in der modernen Musik-Produktion aufgrund ihrer Vielseitigkeit weit verbreitet. Das Projekt, ein Audiodaten-Verarbeitungssystem, macht es möglich, diese virtuellen Instrumente auch mit einer E-Gitarre zu spielen. Hierfür wird das analoge Audiosignal direkt von der Ausgangsbuchse der Gitarre abgegriffen, in MIDI-Noten umgewandelt und via USB an eine Digital Audio Workstation (DAW) übertragen.

Die Ein- und Ausgänge des Systems umfassen eine Micro-USB-Buchse und zwei Mono-Audio Buchsen. Die analogen Signale werden zunächst an einer der Buchsen abgegriffen, digitalisiert und von einem leistungsstarken 32-Bit Prozessor analysiert. Die Daten, die sich aus der Analyse ergeben, werden anschließend in das MIDI Format konvertiert und über die USB-Schnittstelle ausgegeben. An der anderen Buchse wird das Gitarrensignal unverändert wiedergegeben, um eine Weiterverwendung zu ermöglichen.

Der Vorteil des MIDI-Formats ist, dass es der Standard vieler Musikproduktionsumgebungen ist. In der DAW wird das MIDI-Signal als Note auf einem Klavier interpretiert, welches dann in beliebige virtuelle Instrumente eingespeist werden kann und somit einen benutzerdefinierten Ton abspielt. Außerdem hat das Abgreifen der unveränderten Gitarrenschwingung den Zweck, dass parallel zur Analyse eine Aufnahme des Rohsignals erfolgen kann.

## Projektstatus

Das Projekt befindet sich derzeit in der Entwicklungsphase. Funktionalitäten sind noch in Planung.
Planungsvorgänge können unter dem Reiter *[Projects](https://github.com/s-grundner/MTAP-MIDI-Guitar-Converter/projects?query=is%3Aopen)* eingesehen werden.

- Der Schaltplan sowie die Platine sind bereits fertig.
- Simulationen für das Analogue-Frontend stimmen
- Die FFT eines künstlich generierten Signals ist bereits möglich
- Frequenz zu Noten Konversion möglich
- Die hardwaretechnische Realisierung des AFE Prototypen ist bereits Vorhanden
- Das Grundgerüst der Codebase ist bereits ausgearbeitet
- Die Firmware läuft bereits auf einem RTOS, in welchem die priorisierung der einzelen Tasks gehandled wird, sowie eine stabile Kommunikation zwischen den beiden Kernen am ESP32
- Benötigte Bibliotheken sind ausprogrammiert bzw implementiert
- (nicht kontinuierliche) FFT von Gitarrensamples sind möglich

### 3D Renderung der Platine

![](/documentation/images/MTAP-Embedded.gif)

| Top-Seite | Bot-Seite |
|-|-|
|![](/documentation/images/MTAP-Embedded.png)|![](/documentation/images/MTAP-Embedded-bot.png)|

### Assemblierter Prototyp

| <img src="/documentation/images/PCB-prot-iso-outline.png" width="400"/> | <img src="/documentation/images/PCB-prot-outline.png" width="200"/> |
|-|-|

## Mitwirkende
### Betreuer
[Prof. Dipl.-Ing. Siegbert Schrempf](https://www.linkedin.com/in/siegbert-schrempf-aa1189a)
### Entwickler
- [Simon Grundner](https://github.com/s-grundner)
- [Laurenz Hölzl](https://github.com/Laurenz03)
- [Daniel Bräumann](https://github.com/Hexewillfred)

![Poster](/documentation/images/plakat.png)
