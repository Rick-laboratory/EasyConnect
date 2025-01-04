#include "ClientTCP.h"
#include <iostream>
#include <cstring>
#include "../Common/CryptoConfig.h"
#include "../Common/NetUtils.h"
#include "../Common/AES.h"

// ---------------------------
// Implementierung der Hilfsfunktionen
// ---------------------------

bool ClientTCP::initSockets()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return (err == 0);
#else
    // Unter Linux/Unix kein spezielles Init nötig
    return true;
#endif
}

void ClientTCP::cleanupSockets()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void ClientTCP::closeSocket(CrossSocket s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

int ClientTCP::getLastError()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

// ---------------------------
// ClientTCP Konstruktor / Destruktor
// ---------------------------
ClientTCP::ClientTCP(const std::string& ipAddress)
{
    if (!initSockets()) {
        std::cerr << "[ClientTCP] initSockets() failed.\n";
        return;
    }

    // Socket anlegen
#ifdef _WIN32
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
#else
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
#endif

    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[ClientTCP] socket() failed, err=" << getLastError() << "\n";
        return;
    }

    // Adresse vorbereiten
    sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT); // Aus DiscoveryCommon oder eigener define
#ifdef _WIN32
    inet_pton(AF_INET, ipAddress.c_str(), &addr.sin_addr);
#else
    // Auf Linux ist inet_pton auch verfügbar (seit langem).
    inet_pton(AF_INET, ipAddress.c_str(), &addr.sin_addr);
#endif

    // Verbinden
    int ret = connect(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (ret == SOCKET_ERROR) {
        std::cerr << "[ClientTCP] connect() failed: err=" << getLastError() << "\n";
        closeSocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }

    std::cout << "[ClientTCP] Connected to server at " << ipAddress << ":" << TCP_PORT << "\n";
}

ClientTCP::~ClientTCP()
{
    if (m_socket != INVALID_SOCKET) {
        closeSocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    cleanupSockets();
}

// ---------------------------
// runConsoleLoop
// ---------------------------
void ClientTCP::runConsoleLoop()
{
    if (m_socket == INVALID_SOCKET) {
        return;
    }

    AES aes(256);

    while (true) {
        std::cout << "> ";
        std::string userInput;
        if (!std::getline(std::cin, userInput)) {
            break;
        }
        if (userInput.empty()) {
            break;
        }

        // (1) Verschlüsselt Senden
        bool okSend = sendPacket(
            m_socket,
            reinterpret_cast<const unsigned char*>(userInput.data()),
            userInput.size(),
            aes,
            AES_KEY_256);
        if (!okSend) {
            std::cerr << "[ClientTCP] sendPacket failed.\n";
            break;
        }

        // (2) Antwort empfangen
        auto replyVec = recvPacket(m_socket, aes, AES_KEY_256);
        if (replyVec.empty()) {
            std::cout << "[ClientTCP] Server closed or error.\n";
            break;
        }
        std::string replyStr(reinterpret_cast<char*>(replyVec.data()), replyVec.size());
        std::cout << "SERVER> " << replyStr << "\n";
    }
}
