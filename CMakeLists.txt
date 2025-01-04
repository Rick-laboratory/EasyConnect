cmake_minimum_required(VERSION 3.10)
project(EasyConnect LANGUAGES CXX)

# Wir setzen den C++-Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Füge die Unterordner hinzu
# => Dadurch werden jeweils die CMakeLists.txt in diesen Verzeichnissen aufgerufen
add_subdirectory(Common)
add_subdirectory("EasyConnect Client")
add_subdirectory("EasyConnect Server")
