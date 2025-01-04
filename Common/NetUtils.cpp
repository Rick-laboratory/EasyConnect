#include "NetUtils.h"
#include <cstdlib>  // rand()
#include <ctime>
#include <cstring>
#include <iostream>

bool sendAll(SOCKET s, const char* data, int totalBytes)
{
    int sent = 0;
    while (sent < totalBytes) {
        int res = send(s, data + sent, totalBytes - sent, 0);
        if (res == SOCKET_ERROR || res == 0) {
            return false;
        }
        sent += res;
    }
    return true;
}

bool recvAll(SOCKET s, char* data, int totalBytes)
{
    int received = 0;
    while (received < totalBytes) {
        int res = recv(s, data + received, totalBytes - received, 0);
        if (res <= 0) {
            return false;
        }
        received += res;
    }
    return true;
}

/// Erzeugt 16 zufällige Bytes als IV
std::vector<unsigned char> generateRandomIV()
{
    std::vector<unsigned char> iv(16);
    for (int i = 0; i < 16; i++) {
        iv[i] = static_cast<unsigned char>(rand() & 0xFF);
    }
    return iv;
}

bool sendPacket(SOCKET s, const unsigned char* plaintext, size_t ptLen,
    AES& aes, const unsigned char* key)
{
    // 1) Random IV
    auto iv = generateRandomIV();

    // 2) Verschlüsseln
    unsigned int encLen = 0;
    unsigned char* encData = aes.EncryptCBC_PKCS7(plaintext, (unsigned int)ptLen,
        key, iv.data(), encLen);

    // 3) Gesamtlänge = 16 (IV) + Cipher
    uint32_t totalLen = 16 + encLen;
    uint32_t netLen = htonl(totalLen);

    // Sendepuffer = 4 Bytes (length) + 16 (IV) + encData
    std::vector<char> buffer(4 + totalLen);
    // Kopiere length
    std::memcpy(buffer.data(), &netLen, 4);
    // Kopiere IV
    std::memcpy(buffer.data() + 4, iv.data(), 16);
    // Kopiere Cipher
    std::memcpy(buffer.data() + 4 + 16, encData, encLen);

    delete[] encData;

    // 4) Senden
    return sendAll(s, buffer.data(), (int)buffer.size());
}

std::vector<unsigned char> recvPacket(SOCKET s, AES& aes, const unsigned char* key)
{
    // 1) Erst 4 Bytes lesen
    uint32_t netLen = 0;
    if (!recvAll(s, reinterpret_cast<char*>(&netLen), 4)) {
        return {};
    }
    uint32_t totalLen = ntohl(netLen);
    if (totalLen < 16) {
        return {};
    }

    // 2) Lese totalLen Bytes -> (IV + Ciphertext)
    std::vector<char> buffer(totalLen);
    if (!recvAll(s, buffer.data(), totalLen)) {
        return {};
    }

    // 3) IV (16), Rest = Cipher
    unsigned char* iv = reinterpret_cast<unsigned char*>(buffer.data());
    unsigned int encLen = totalLen - 16;
    unsigned char* encData = reinterpret_cast<unsigned char*>(buffer.data() + 16);

    // 4) Entschlüsseln
    unsigned int plainLen = 0;
    unsigned char* plain = aes.DecryptCBC_PKCS7(encData, encLen, key, iv, plainLen);

    // 5) In vector zurück
    std::vector<unsigned char> result(plain, plain + plainLen);
    delete[] plain;
    return result;
}
