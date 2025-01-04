# Im Ordner EasyConnect Client/
cmake_minimum_required(VERSION 3.10)

# Executable anlegen:
add_executable(easyconnect_client
    main.cpp
    ServerDiscovery.cpp
    ClientTCP.cpp
)

# Include-Pfade setzen, damit #include "../Common/..." funktioniert
# oder du passt die Includes an, je nachdem.
target_include_directories(easyconnect_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        # Wir möchten auch die Common-Includes erreichbar machen:
        ${CMAKE_CURRENT_LIST_DIR}/../Common
)

# Link zu der Common-Library
target_link_libraries(easyconnect_client
    PRIVATE
        easyconnect_common
)

# Unter Windows evtl. ws2_32 nochmal
if(WIN32)
    target_link_libraries(easyconnect_client PRIVATE ws2_32)
endif()
elseif(UNIX)
    # Falls du pthread oder andere Libs brauchst:
    # target_link_libraries(easyconnect_client
    #     PRIVATE
    #         pthread
    # )
endif()