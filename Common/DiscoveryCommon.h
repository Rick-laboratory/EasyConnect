#pragma once
#include <cstdint>

// Gleicher Discovery-Port auf Server und Client
static const int DISCOVERY_PORT = 9009;
// Wir nehmen für TCP einen anderen Port als für den Broadcast
static const int TCP_PORT = 9008;

// "Magic" zur eindeutigen Kennzeichnung unseres Broadcast-Protokolls
static const uint32_t DISCOVERY_MAGIC = 0xB00B5;

// Einfache Protokoll-Version
static const uint32_t DISCOVERY_VERSION = 1;

// UDP-Broadcast wird zyklisch wiederholt
static const int MAX_BROADCAST_TRIES = 100;
static const int BROADCAST_SLEEP_MS = 100; // Wartezeit zwischen den Broadcasts

#pragma pack(push, 1)
struct DiscoveryPacket {
    uint32_t magic;       // Muss DISCOVERY_MAGIC sein
    uint32_t version;     // DISCOVERY_VERSION
    uint64_t randomToken; // Zufällig generiert
};
#pragma pack(pop)
