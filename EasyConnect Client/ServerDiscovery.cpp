#include "ServerDiscovery.h"

ServerDiscovery::ServerDiscovery()
{
    if (!initSockets()) {
        std::cerr << "[ServerDiscovery] WSAStartup/initSockets failed.\n";
        return;
    }

#ifdef _WIN32
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
#else
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
#endif
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[ServerDiscovery] socket() failed, error=" << getLastError() << "\n";
        return;
    }

    // Binden an DISCOVERY_PORT + any IP
    sockaddr_in bindAddr{};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(DISCOVERY_PORT);
    bindAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_socket, reinterpret_cast<sockaddr*>(&bindAddr), sizeof(bindAddr)) == SOCKET_ERROR) {
        std::cerr << "[ServerDiscovery] bind() failed, error=" << getLastError() << "\n";
#ifdef _WIN32
        closesocket(m_socket);
#else
        close(m_socket);
#endif
        m_socket = INVALID_SOCKET;
        return;
    }
}

ServerDiscovery::~ServerDiscovery()
{
    if (m_socket != INVALID_SOCKET) {
        closeSocket(m_socket);
    }
    cleanupSockets();
}

std::string ServerDiscovery::waitForServer()
{
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[ServerDiscovery] Invalid socket.\n";
        return std::string();
    }

    sockaddr_in senderAddr{};
#ifdef _WIN32
    int senderLen = sizeof(senderAddr);
#else
    socklen_t senderLen = sizeof(senderAddr);
#endif

    DiscoveryPacket packetRecv{};

    // Hier: "wartet auf ein Discovery-Paket vom Server"
    // Schickt es direkt zurück, wenn es ankommt.
    // Gibt die IP zurück, sonst "".

    std::cout << "[ServerDiscovery] Waiting for Discovery-Packet...\n";

    while (true) {
        // recvfrom blockiert – du könntest optional Non-Blocking oder Timeouts nutzen
        // Hier machen wir's synchron, damit "nichts verändert" wird.
        int bytes = recvfrom(
            m_socket,
            reinterpret_cast<char*>(&packetRecv),
            sizeof(packetRecv),
            0,
            reinterpret_cast<sockaddr*>(&senderAddr),
            &senderLen);

        if (bytes == SOCKET_ERROR) {
            std::cerr << "[ServerDiscovery] recvfrom() error=" << getLastError() << "\n";
            return std::string();
        }
        if (bytes == sizeof(packetRecv)) {
            // "Schickt es direkt zurück" – wir senden den gleichen Packetinhalt an den Absender
            int sent = sendto(
                m_socket,
                reinterpret_cast<const char*>(&packetRecv),
                sizeof(packetRecv),
                0,
                reinterpret_cast<sockaddr*>(&senderAddr),
                senderLen);
            if (sent == SOCKET_ERROR) {
                std::cerr << "[ServerDiscovery] sendto() error=" << getLastError() << "\n";
                return std::string();
            }

            // IP aus senderAddr auslesen
            char ipStr[INET_ADDRSTRLEN];
#ifdef _WIN32
            inet_ntop(AF_INET, &senderAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
#else
            inet_ntop(AF_INET, &senderAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
#endif
            std::cout << "[ServerDiscovery] Received & replied to " << ipStr << "\n";

            // Erfolgreich -> IP zurück
            return std::string(ipStr);
        }
    }
    // Nie erreicht
    return std::string();
}

// ---------------------------------------------------
// Plattformabhängige Helfer
// ---------------------------------------------------
bool ServerDiscovery::initSockets()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return (err == 0);
#else
    return true;
#endif
}

void ServerDiscovery::cleanupSockets()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void ServerDiscovery::closeSocket(CrossSocket s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

int ServerDiscovery::getLastError()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
