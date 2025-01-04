#include <ctime>
#include <cstdlib>
#include "ServerDiscovery.h"
#include "ClientTCP.h"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // 1) Discovery
    ServerDiscovery discovery;
    std::string serverIp = discovery.waitForServer();
    if (serverIp.empty()) {
        return 0;
    }

    // 2) TCP-Verbindung
    ClientTCP client(serverIp);
    client.runConsoleLoop();

    return 0;
}
