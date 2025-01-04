# Im Ordner EasyConnect Server/
cmake_minimum_required(VERSION 3.10)

add_executable(easyconnect_server
    main.cpp
    ClientDiscovery.cpp
    ServerTCP.cpp
    # ggf. weitere .cpp
)

target_include_directories(easyconnect_server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../Common
)

target_link_libraries(easyconnect_server
    PRIVATE
        easyconnect_common
)

if(WIN32)
    target_link_libraries(easyconnect_server PRIVATE ws2_32)
endif()
elseif(UNIX)
    # Falls du pthread oder andere Libs brauchst:
    # target_link_libraries(easyconnect_server
    #     PRIVATE
    #         pthread
    # )
endif()