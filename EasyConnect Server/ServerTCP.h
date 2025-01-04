#pragma once

#include <string>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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

class ServerTCP
{
public:
    ServerTCP();
    ~ServerTCP();

    bool acceptClient();
    void runEchoLoop();

private:
    CrossSocket m_listenSocket = INVALID_SOCKET;
    CrossSocket m_clientSocket = INVALID_SOCKET;

    // Betriebssystemspezifische Helfer
    bool initSockets();
    void cleanupSockets();
    void closeSocket(CrossSocket s);
    int  getLastError();
};
