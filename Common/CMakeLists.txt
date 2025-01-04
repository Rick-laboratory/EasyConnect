# Im Ordner Common/
cmake_minimum_required(VERSION 3.10)

# Wir erstellen eine statische Bibliothek (könnte auch STATIC, SHARED etc. sein)
add_library(easyconnect_common
    AES.cpp
    NetUtils.cpp
    # Falls du weitere cpp-Dateien hast, hier angeben
    # (CryptoConfig.h, DiscoveryCommon.h sind Header und brauchen hier nicht explizit)
)

target_include_directories(easyconnect_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}  # Damit AES.h, NetUtils.h etc. gefunden werden
)

# Unter Windows musst du Ws2_32.lib verlinken, wenn NetUtils.cpp WinSock nutzt
if(WIN32)
    target_link_libraries(easyconnect_common ws2_32)
endif()
