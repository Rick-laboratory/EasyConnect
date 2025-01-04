EasyConnect ermöglicht es, einen Server und einen Client im gleichen lokalen Netzwerk (LAN) auf einfache Weise miteinander zu „paaren“, ohne manuell IP-Adressen eingeben zu müssen. Anschließend kommunizieren beide über eine verschlüsselte TCP-Verbindung per AES.

Features
UDP-Broadcast Discovery

Der Server (oder Client, je nach Konfiguration) broadcastet eine kurze Binärnachricht im LAN.
Das Gegenstück lauscht und antwortet. So wird die IP des Servers automatisch gefunden.
Dadurch muss niemand eine IP-Adresse oder einen Port von Hand eintippen.
TCP-Verbindung

Sobald die Discovery abgeschlossen ist, wird eine TCP-Verbindung zum ermittelten Partner aufgebaut.
Die Kommunikation erfolgt dann als normaler Socket-Datenstrom.
AES-Verschlüsselung

Die TCP-Nutzdaten werden via AES (CBC-Modus, PKCS#7-Padding) verschlüsselt.
Jeder gesendeten Nachricht wird ein zufälliges IV hinzugefügt (pro Nachricht neu).
Es wird immer zuerst eine 4-Byte-Längeninformation gesendet, dann IV + Ciphertext.
Auf diese Weise sind alle Klartextdaten im LAN verborgen („Abhören“ zeigt nur zufällige Hexbytes).
Startreihenfolge

Optional lassen sich die Discovery-Schleifen so gestalten, dass es egal ist, ob zuerst der Client oder der Server startet (z. B. durch Endlosschleifen oder ausreichend hohe Timeout/Versuchs-Anzahl).
Projektstruktur
Ein Beispiel, wie man das Projekt aufteilen kann:
YourSolution/
  ├─ EasyConnectServer/
  │    ├─ main_server.cpp
  │    ├─ ServerDiscovery.h / .cpp
  │    ├─ ServerTCP.h / .cpp
  │    ├─ DiscoveryCommon.h
  │    ├─ AES.h / .cpp
  │    ├─ CryptoConfig.h
  │    ├─ NetUtils.h / .cpp
  │    └─ ...
  └─ EasyConnectClient/
       ├─ main_client.cpp
       ├─ ClientDiscovery.h / .cpp
       ├─ ClientTCP.h / .cpp
       ├─ DiscoveryCommon.h
       ├─ AES.h / .cpp
       ├─ CryptoConfig.h
       ├─ NetUtils.h / .cpp
       └─ ...
Wichtige Dateien im Überblick
DiscoveryCommon.h
Enthält Konstanten/Strukturen für den UDP-Broadcast (Ports, Magic Values etc.).

ServerDiscovery / ClientDiscovery

Senden bzw. Empfangen von Broadcast-Paketen mit zufälligen Tokens.
Hier wird geregelt, wie lange gesendet oder gewartet wird.
ServerTCP / ClientTCP

Baut die TCP-Verbindung auf.
Enthält die „EchoLoop“ oder „ConsoleLoop“, in der Daten über den Socket gesendet/empfangen werden.
AES.h / AES.cpp

Implementierung von AES (CBC-Modus) mit PKCS#7-Padding.
Bietet Methoden wie EncryptCBC_PKCS7 und DecryptCBC_PKCS7.
NetUtils.h / NetUtils.cpp

Hilfsfunktionen für das Senden/Empfangen „mit Längen-Header und IV“ (sendPacket / recvPacket).
Enthält außerdem kleine Helfer wie sendAll/recvAll, generateRandomIV, etc.
CryptoConfig.h

Statischer 256-Bit-AES-Key (nur für Demo!).
In einer echten Anwendung würde man hier z. B. einen Schlüsseltausch oder ein anderes Verfahren nutzen.
main_server.cpp / main_client.cpp

Einstiegsfunktionen (jeweils main()).
Starten die Discovery und anschließend die TCP-Kommunikation.
Voraussetzungen
Betriebssystem: Windows (wegen WinSock2-Aufrufen).
Compiler: Visual Studio oder ein anderer C++-Compiler mit Windows-SDK.
Bibliotheken:
Ws2_32.lib muss beim Linker eingebunden sein (z. B. in Visual Studio: Projekt-Eigenschaften > Linker > Eingabe).
Sonst keine externen Libraries erforderlich.
Kompilierung und Ausführung
Solution öffnen: Lade das Projekt in Visual Studio (oder ein anderes IDE/Buildsystem).
Build:
Stelle sicher, dass beide Projekte (Server und Client) in einer Debug oder Release Konfiguration kompiliert werden.
Achte darauf, dass Ws2_32.lib verlinkt wird.
Start:
Server-Projekt starten (z. B. main_server.exe).
Der Server beginnt ggf. zu broadcasten (Discovery) und wartet auf eine Antwort.
Sobald ein Client gefunden ist, lauscht er auf TCP-Verbindungen.
Client-Projekt starten (z. B. main_client.exe).
Der Client hört auf Broadcasts (oder sendet sie, je nach Config) und verbindet sich danach per TCP.
Anmerkung zur Startreihenfolge
In der Grundversion mit MAX_BROADCAST_TRIES kann es sein, dass du den Server zuerst starten solltest (damit er broadcastet), bevor der Client überhaupt antworten kann.
Wenn du den Mechanismus so anpasst, dass beide Seiten dauerhaft warten/lauschen bzw. dauerhaft broadcasten, ist die Reihenfolge egal – auf Kosten möglicher Dauerschleifen im LAN.
Kurzer Test
Server:

Starten.
Sobald er schreibt „Discovery done“ (o.ä.), öffnet er den TCP-Port.
Client:

Starten.
Sieht den Broadcast, antwortet, baut TCP-Verbindung auf.
Du kannst in der Konsole Befehle eintippen, die verschlüsselt an den Server geschickt werden.
Der Server empfängt sie, entschlüsselt sie, zeigt sie an und sendet (verschlüsselt) eine Antwort zurück. Der Client entschlüsselt und druckt die Antwort.

Wie überprüfe ich, ob die Daten verschlüsselt sind?
Wireshark-Mitschnitt:
Schau dir die TCP-Pakete (Port 9008) an, sie sollten reine Hex-Blöcke sein, aus denen kein Klartext herauszulesen ist.
Das UDP-Discovery ist binär, aber nicht verschlüsselt. Du siehst ggf. „Zufallswerte“ (randomToken), die nur wie Verschlüsselung aussehen, aber in Wirklichkeit unverschlüsseltes Binärformat sind.
To-Do / Mögliche Erweiterungen
Multi-Client-Support: Aktuell akzeptiert der Server nur einen Client und geht dann in die Echo-Schleife. Für mehrere Clients ist Threading oder Async-IO notwendig.
Integritätsschutz: Ein HMAC oder GCM-Modus würde Manipulationen (Bitflips) erkennen.
Schlüsselaustausch: Der Key in CryptoConfig.h ist statisch. Ein echtes System bräuchte z. B. Diffie-Hellman oder TLS.
Konfiguration: Statt harter Ports (9009, 9008) wäre eine Konfigurationsdatei oder Parameter nützlich.
