#pragma once
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
typedef SOCKET CrossSocket;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

typedef int CrossSocket;
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)
#endif

#include <iostream>
#include <string>

// Hier deine DiscoveryCommon-Konstanten/Strukturen
#include "../Common/DiscoveryCommon.h"

// Diese Klasse: Wartet auf ein Discovery-Paket vom "Server" und schickt es zurück.
// Gibt bei Erfolg die IP zurück, sonst leere Zeichenkette.
class ServerDiscovery
{
public:
    ServerDiscovery();
    ~ServerDiscovery();

    // Wartet auf Discovery-Paket und schickt es direkt zurück.
    // Gibt bei Erfolg die IP des Senders zurück, sonst "".
    std::string waitForServer();

private:
    // Auf Windows ein SOCKET, auf Linux ein int
    CrossSocket m_socket = INVALID_SOCKET;

    // Plattformabhängige Helfer
    bool initSockets();
    void cleanupSockets();
    void closeSocket(CrossSocket s);
    int  getLastError();
};
