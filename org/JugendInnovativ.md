# Kurzberschreibung
Das vorliegende Projekt ermöglicht die Verwendung einer E-Gitarre als MIDI-Device. Das Gerät soll einzelne Noten und Akkorde zuverlässig mit möglichst geringer Latenz in das MIDI-Format umwandeln. Die MIDI-Signale werden an die USB-Schnittstelle eines PCs übertragen, von welchem die Signale beliebig interpretiert werden können.

# Vorhaben, Lösungsweg und Innovation
Virtuelle Instrumente sind in der modernen Musik-Produktion aufgrund ihrer Vielseitigkeit weit verbreitet. Das Projekt, ein Audiodaten-Verarbeitungssystem, macht es möglich, diese virtuellen Instrumente auch mit einer E-Gitarre zu spielen. Hierfür wird das analoge Audiosignal direkt von der Ausgangsbuchse der Gitarre abgegriffen, in MIDI-Noten umgewandelt und via USB an eine Digital Audio Workstation (DAW) übertragen.

Die Ein- und Ausgänge des Systems umfassen eine Micro-USB-Buchse und zwei Mono-Audio Buchsen. Die analogen Signale werden zunächst an einer der Buchsen abgegriffen, digitalisiert und von einem leistungsstarken 32-Bit Prozessor analysiert. Die Daten, die sich aus der Analyse ergeben, werden anschließend in das MIDI Format konvertiert und über die USB-Schnittstelle ausgegeben. An der anderen Buchse wird das Gitarrensignal unverändert wiedergegeben, um eine Weiterverwendung zu ermöglichen.

Der Vorteil des MIDI-Formats ist, dass es der Standard vieler Musikproduktionsumgebungen ist. In der DAW wird das MIDI-Signal als Note auf einem Klavier interpretiert, welches dann in beliebige virtuelle Instrumente eingespeist werden kann und somit einen benutzerdefinierten Ton abspielt. Außerdem hat das Abgreifen der unveränderten Gitarrenschwingung den Zweck, dass parallel zur Analyse eine Aufnahme des Rohsignals erfolgen kann.



