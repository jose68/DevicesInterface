cmake_minimum_required(VERSION 3.10)
project(devicesinterface VERSION 1.0 LANGUAGES CXX)

# Configuration C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Recherche de Java et JNI (Indispensable pour le pont Java)
find_package(JNI REQUIRED)

# 2. Dossier des en-têtes (headers)
include_directories(include ${JNI_INCLUDE_DIRS})

# 3. La DLL (devicesinterface)
add_library(devicesinterface SHARED 
    src/ZebraPrinter.cpp 
    src/DeviceNativeBridge.cpp
)

# Configuration de la DLL : nom propre et dossier de sortie
set_target_properties(devicesinterface PROPERTIES 
    PREFIX "" 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- OPTIONS DE LIAISON STATIQUE (C'est ici que ça se passe) ---
# On force l'inclusion de libstdc++ et libgcc directement dans la DLL
target_link_options(devicesinterface PRIVATE "-static-libgcc" "-static-libstdc++" "-static")
target_link_libraries(devicesinterface PRIVATE setupapi)

# 4. L'exécutable de test (autonome lui aussi)
add_executable(zebra_test 
    tests/ZebraPrinterTest.cpp 
    src/ZebraPrinter.cpp
)

set_target_properties(zebra_test PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_options(zebra_test PRIVATE "-static-libgcc" "-static-libstdc++" "-static")
target_link_libraries(zebra_test PRIVATE setupapi)