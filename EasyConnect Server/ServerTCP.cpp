#include "ServerTCP.h"
#include <iostream>
#include <cstring>
#include "../Common/DiscoveryCommon.h"
#include "../Common/CryptoConfig.h"
#include "../Common/NetUtils.h"
#include "../Common/AES.h"

bool ServerTCP::initSockets()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return (err == 0);
#else
    return true;
#endif
}

void ServerTCP::cleanupSockets()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void ServerTCP::closeSocket(CrossSocket s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

int ServerTCP::getLastError()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

// --------------------
// Konstruktor/Destruktor
// --------------------
ServerTCP::ServerTCP()
{
    if (!initSockets()) {
        std::cerr << "[ServerTCP] initSockets() failed.\n";
        return;
    }

#ifdef _WIN32
    m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
#else
    m_listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
#endif

    if (m_listenSocket == INVALID_SOCKET) {
        std::cerr << "[ServerTCP] socket() failed: err=" << getLastError() << "\n";
        return;
    }

    sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[ServerTCP] bind() failed: err=" << getLastError() << "\n";
        closeSocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        return;
    }

    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[ServerTCP] listen() failed: err=" << getLastError() << "\n";
        closeSocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        return;
    }
}

ServerTCP::~ServerTCP()
{
    if (m_listenSocket != INVALID_SOCKET) {
        closeSocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }
    if (m_clientSocket != INVALID_SOCKET) {
        closeSocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }
    cleanupSockets();
}

// --------------------
// acceptClient
// --------------------
bool ServerTCP::acceptClient()
{
    if (m_listenSocket == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in clientAddr{};
#ifdef _WIN32
    int clientLen = sizeof(clientAddr);
#else
    socklen_t clientLen = sizeof(clientAddr);
#endif

    m_clientSocket = accept(m_listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
    if (m_clientSocket == INVALID_SOCKET) {
        std::cerr << "[ServerTCP] accept() failed: err=" << getLastError() << "\n";
        return false;
    }

    char ipStr[INET_ADDRSTRLEN];
#ifdef _WIN32
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
#else
    // Auf Linux/Unix inet_ntop ist ebenfalls verfügbar
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
#endif

    std::cout << "[ServerTCP] Client connected from " << ipStr
        << ":" << ntohs(clientAddr.sin_port) << "\n";

    return true;
}

// --------------------
// runEchoLoop
// --------------------
void ServerTCP::runEchoLoop()
{
    if (m_clientSocket == INVALID_SOCKET) {
        std::cerr << "[ServerTCP] No client socket.\n";
        return;
    }

    AES aes(256);

    while (true) {
        // 1) Verschlüsseltes Paket empfangen -> Klartext
        auto plaintextVec = recvPacket(m_clientSocket, aes, AES_KEY_256);
        if (plaintextVec.empty()) {
            std::cout << "[ServerTCP] Client disconnected or error.\n";
            break;
        }

        // In std::string wandeln
        std::string received(reinterpret_cast<char*>(plaintextVec.data()), plaintextVec.size());
        std::cout << "[ServerTCP] Received: " << received << "\n";

        // 2) Antwort
        std::string response = "Hello from server: " + received;

        // 3) Verschlüsselt senden
        bool ok = sendPacket(
            m_clientSocket,
            reinterpret_cast<const unsigned char*>(response.data()),
            response.size(),
            aes,
            AES_KEY_256
        );
        if (!ok) {
            std::cerr << "[ServerTCP] sendPacket failed.\n";
            break;
        }
    }
}
