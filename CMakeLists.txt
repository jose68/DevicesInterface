cmake_minimum_required(VERSION 3.10)
project(devicesinterface VERSION 1.0 LANGUAGES CXX)

# 1. Configuration C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Recherche de Java et JNI
find_package(JNI REQUIRED)

# 3. Définition des dossiers d'en-têtes
include_directories(
    include 
    ${JNI_INCLUDE_DIRS}
)

# 4. Préparation des options de liaison STATIQUE pour MinGW
# On utilise une liste (;) pour que CMake passe chaque argument séparément au compilateur
set(STATIC_LINK_FLAGS 
    -static 
    -static-libgcc 
    -static-libstdc++ 
    -Wl,-Bstatic 
    -lwinpthread 
    -Wl,-Bdynamic
)

# 5. La DLL (devicesinterface) - C'est elle qui sera utilisée par Java
add_library(devicesinterface SHARED 
    src/ZebraPrinter.cpp 
    src/DeviceNativeBridge.cpp
)

# Configuration de la DLL
set_target_properties(devicesinterface PROPERTIES 
    PREFIX "" 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Application des flags statiques + alias stdcall pour la compatibilité JNI
target_link_options(devicesinterface PRIVATE 
    ${STATIC_LINK_FLAGS} 
    -Wl,--add-stdcall-alias
)

# Liens vers les bibliothèques système Windows (toujours dynamiques car natives au système)
target_link_libraries(devicesinterface PRIVATE 
    setupapi 
    cfgmgr32
)

# 6. L'exécutable de test (zebra_test.exe)
add_executable(zebra_test 
    tests/ZebraPrinterTest.cpp 
    src/ZebraPrinter.cpp
)

set_target_properties(zebra_test PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Application des flags statiques pour l'exécutable
target_link_options(zebra_test PRIVATE 
    ${STATIC_LINK_FLAGS}
)

target_link_libraries(zebra_test PRIVATE 
    setupapi 
    cfgmgr32
)