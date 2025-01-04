# EasyConnect

Vernetze **Server** und **Client** im lokalen Netzwerk, ganz ohne IP-Eingabe – und das **verschlüsselt** per AES.

![EasyConnect Demo](https://via.placeholder.com/600x200?text=EasyConnect+Demo+Screenshot)  
*(Platzhalter-Bild – ersetze es gerne mit einem echten Screenshot.)*

## Inhaltsverzeichnis
- [Introduction](#introduction)
- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements & Build](#requirements--build)
- [CMake Usage](#cmake-usage)


## Introduction
**EasyConnect** ermöglicht es, einen **Server** und einen **Client** im **lokalen Netzwerk** automatisch miteinander zu verbinden (Discovery), ohne IP-Adressen händisch einzugeben. Anschließend baut EasyConnect eine **TCP-Verbindung** auf und **verschlüsselt** die Kommunikation via **AES**.

- **Ideal für** Anwender, die keinen IP- oder Netzwerkstress wollen.  
- **Flexibel**: Dank normalem Socket-Datenstrom kann jede Art von Daten übertragen werden.

## Features

1. **UDP-Broadcast Discovery**  
   - Automatisches „Auffinden“ des Gegenübers im LAN  
   - Spart das manuelle Eintippen von IP-Adressen oder Ports

2. **TCP-Verbindung**  
   - Auf der ermittelten IP wird eine Socket-Verbindung aufgebaut  
   - Beliebige Daten oder Protokolle möglich

3. **AES-Verschlüsselung (CBC-Modus, PKCS#7)**  
   - Kein Klartext im Netzwerk sichtbar  
   - Pro Nachricht wird ein zufälliges IV verwendet  
   - 4-Byte-Längenheader vor IV + Ciphertext

4. **Startreihenfolge**  
   - Optional kann Discovery endlos laufen, damit Server & Client in beliebiger Reihenfolge starten  
   - Standardmäßig: Server broadcastet zuerst, Client hört zu
### Wichtige Dateien

- **DiscoveryCommon.h**  
  Konstanten & Strukturen für den UDP-Broadcast (z. B. Ports, Magic Values, Tokens etc.)  

- **ServerDiscovery / ClientDiscovery**  
  Senden & Empfangen von Broadcast-Paketen, regeln die Discovery-Logik (Wie oft senden? Wie lange warten?).

- **ServerTCP / ClientTCP**  
  Bauen die TCP-Verbindung auf und enthalten z. B. `runConsoleLoop` oder `runEchoLoop` für den Datenaustausch.

- **AES.h / AES.cpp**  
  AES-Implementierung (CBC, PKCS#7). Methoden: `EncryptCBC_PKCS7`, `DecryptCBC_PKCS7`, etc.

- **NetUtils.h / NetUtils.cpp**  
  Hilfsfunktionen wie `sendPacket`, `recvPacket`, `generateRandomIV`, `sendAll`, `recvAll`, usw.

- **CryptoConfig.h**  
  Statischer 256-Bit Key (Demo). In echten Szenarien würde man einen Schlüsseltausch nutzen.

- **main_server.cpp / main_client.cpp**  
  Einstiegspunkte für Server & Client, starten die Discovery und danach TCP-Kommunikation.

## Requirements & Build

- **Betriebssystem**: Ursprünglich Windows (WinSock2).  
  - Mit `#ifdef _WIN32` oder Cross-Platform-Socket-Libs läuft es auch auf Linux/macOS.
- **Compiler**: Visual Studio oder ein anderer C++-Compiler (MinGW, Clang, GCC).  
- **Bibliotheken**:  
  - `Ws2_32.lib` (Windows) "muss" verlinkt werden, sollte via pragma schon passieren.  
  - Keine weiteren externen Libs nötig (AES & Discovery selbst implementiert)

### CMake Usage

Für eine **plattformunabhängige** Lösung kannst du das Projekt per **CMake** bauen, z. B.:

```bash
cd YourSolution
mkdir build
cd build
cmake ..
cmake --build .
