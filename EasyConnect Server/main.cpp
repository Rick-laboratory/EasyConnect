#include <ctime>
#include <cstdlib>
#include "ClientDiscovery.h"
#include "ServerTCP.h"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // 1) ClientDiscovery: Per UDP-Broadcast Client finden
    ClientDiscovery discovery;
    bool found = discovery.broadcastAndAwaitResponse();
    if (!found) {
        return 0;
    }

    // 2) TCP-Server
    ServerTCP server;
    if (!server.acceptClient()) {
        return 0;
    }

    // 3) EchoLoop (AES-verschlüsselt)
    server.runEchoLoop();

    return 0;
}
