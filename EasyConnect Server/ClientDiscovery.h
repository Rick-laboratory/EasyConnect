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
#include "../Common/DiscoveryCommon.h"

// Hier deine MAX_BROADCAST_TRIES, BROADCAST_SLEEP_MS, etc.
// (Falls sie in DiscoveryCommon stehen, ist das okay.)
//
// class ClientDiscovery broadcastet und wartet auf Antwort.
class ClientDiscovery
{
public:
    ClientDiscovery();
    ~ClientDiscovery();

    bool broadcastAndAwaitResponse();

private:
    // Cross-Platform Socket
    CrossSocket m_socket = INVALID_SOCKET;

    // Zieladresse für Broadcast
    sockaddr_in m_addrSend{};

    // Zu sendendes Packet
    DiscoveryPacket m_packetSend{};

    // Helfer
    bool initSockets();
    void cleanupSockets();
    void closeSocket(CrossSocket s);
    int  getLastError();
};
