#pragma once

#include <string>

// Falls du TCP_PORT in einem gemeinsamen Header definiert hast, nimm das hier rein:
#include "../Common/DiscoveryCommon.h"

// Cross-Platform Forward-Declarations
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
// Ws2_32.lib nicht vergessen beim Linken!
typedef SOCKET CrossSocket;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

typedef int CrossSocket;
#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1
#endif

class ClientTCP
{
public:
    explicit ClientTCP(const std::string& ipAddress);
    ~ClientTCP();

    void runConsoleLoop();

private:
    CrossSocket m_socket = INVALID_SOCKET;

    // Betriebssystemspezifische Helfer
    bool initSockets();
    void cleanupSockets();
    void closeSocket(CrossSocket s);
    int  getLastError();

};
