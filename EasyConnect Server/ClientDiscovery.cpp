#include "ClientDiscovery.h"

ClientDiscovery::ClientDiscovery()
{
    if (!initSockets()) {
        std::cerr << "[ClientDiscovery] WSAStartup/initSockets failed.\n";
        return;
    }

#ifdef _WIN32
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#else
    m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[ClientDiscovery] socket() failed: " << getLastError() << "\n";
        return;
    }

    // Broadcast-Option
    {
        int yes = 1;
        if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST,
            reinterpret_cast<const char*>(&yes),
            sizeof(yes)) == SOCKET_ERROR)
        {
            std::cerr << "[ClientDiscovery] setsockopt(SO_BROADCAST) failed: " << getLastError() << "\n";
            closeSocket(m_socket);
            m_socket = INVALID_SOCKET;
            return;
        }
    }

    // Zieladresse für Broadcast
    ZeroMemory(&m_addrSend, sizeof(m_addrSend));
    m_addrSend.sin_family = AF_INET;
    m_addrSend.sin_port = htons(DISCOVERY_PORT);
    m_addrSend.sin_addr.s_addr = INADDR_BROADCAST;

    // Non-blocking
#ifdef _WIN32
    u_long iMode = 1;
    ioctlsocket(m_socket, FIONBIO, &iMode);
#else
    // Auf Linux z.B. fcntl(...)
    // Da wir "bitte nichts verändern" möglichst wörtlich nehmen,
    // belassen wir es bei diesem Minimalansatz.
#endif

    // Packet vorbereiten
    ZeroMemory(&m_packetSend, sizeof(m_packetSend));
    m_packetSend.magic = DISCOVERY_MAGIC;
    m_packetSend.version = DISCOVERY_VERSION;
    // Simplifiziert random:
    m_packetSend.randomToken = static_cast<uint64_t>(rand() ^ (rand() << 15));
}

ClientDiscovery::~ClientDiscovery()
{
    if (m_socket != INVALID_SOCKET) {
        closeSocket(m_socket);
    }
    cleanupSockets();
}

bool ClientDiscovery::broadcastAndAwaitResponse()
{
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[ClientDiscovery] Invalid socket.\n";
        return false;
    }

    sockaddr_in addrRecv{};
#ifdef _WIN32
    int addrLen = sizeof(addrRecv);
#else
    socklen_t addrLen = sizeof(addrRecv);
#endif

    DiscoveryPacket packetRecv{};

    for (int i = 0; i < MAX_BROADCAST_TRIES; ++i) {
        // Paket senden
        int sentBytes = sendto(
            m_socket,
            reinterpret_cast<const char*>(&m_packetSend),
            sizeof(m_packetSend),
            0,
            reinterpret_cast<sockaddr*>(&m_addrSend),
            sizeof(m_addrSend));
        if (sentBytes == SOCKET_ERROR) {
            std::cerr << "[ClientDiscovery] sendto() failed: " << getLastError() << "\n";
        }

#ifdef _WIN32
        Sleep(BROADCAST_SLEEP_MS);
#else
        usleep(BROADCAST_SLEEP_MS * 1000);
#endif

        // Non-blocking recv
        while (true) {
            int recvBytes = recvfrom(
                m_socket,
                reinterpret_cast<char*>(&packetRecv),
                sizeof(packetRecv),
                0,
                reinterpret_cast<sockaddr*>(&addrRecv),
                &addrLen);

            if (recvBytes == SOCKET_ERROR) {
#ifdef _WIN32
                int err = WSAGetLastError();
                if (err == WSAEWOULDBLOCK) {
#else
                int err = errno;
                if (err == EWOULDBLOCK || err == EAGAIN) {
#endif
                    // Keine Daten -> nächste Schleifenrunde
                    break;
                }
                // Anderer Fehler
                std::cerr << "[ClientDiscovery] recvfrom error: " << err << "\n";
                break;
                }

            if (recvBytes == sizeof(packetRecv)) {
                // Prüfen
                if (packetRecv.magic == DISCOVERY_MAGIC &&
                    packetRecv.version == DISCOVERY_VERSION &&
                    packetRecv.randomToken == m_packetSend.randomToken)
                {
                    char ipStr[INET_ADDRSTRLEN];
#ifdef _WIN32
                    inet_ntop(AF_INET, &addrRecv.sin_addr, ipStr, INET_ADDRSTRLEN);
#else
                    inet_ntop(AF_INET, &addrRecv.sin_addr, ipStr, INET_ADDRSTRLEN);
#endif
                    std::cout << "[ClientDiscovery] Found client at " << ipStr << "\n";
                    return true;
                }
            }
            }
        }
    std::cerr << "[ClientDiscovery] No valid response after " << MAX_BROADCAST_TRIES << " tries.\n";
    return false;
    }

// ------------------------------------------------
// Hilfsfunktionen
// ------------------------------------------------
bool ClientDiscovery::initSockets()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return (err == 0);
#else
    return true;
#endif
}

void ClientDiscovery::cleanupSockets()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void ClientDiscovery::closeSocket(CrossSocket s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

int ClientDiscovery::getLastError()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
