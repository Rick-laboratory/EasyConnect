#pragma once
#include <vector>
#include <winsock2.h>
#include "AES.h"

// sendAll/recvAll: Helfer, um genau n Bytes zu senden/empfangen
bool sendAll(SOCKET s, const char* data, int totalBytes);
bool recvAll(SOCKET s, char* data, int totalBytes);

// sendPacket/recvPacket:
// - sendPacket: 
//    1) random IV erzeugen
//    2) CBC-PKCS7 verschlüsseln
//    3) 4 Byte length + IV (16) + ciphertext
// - recvPacket:
//    1) 4 Byte length lesen
//    2) so viele Bytes in Buffer holen
//    3) IV abtrennen, Rest = Ciphertext
//    4) CBC-PKCS7 entschlüsseln
std::vector<unsigned char> generateRandomIV();

bool sendPacket(SOCKET s, const unsigned char* plaintext, size_t ptLen,
    AES& aes, const unsigned char* key);

std::vector<unsigned char> recvPacket(SOCKET s, AES& aes, const unsigned char* key);
