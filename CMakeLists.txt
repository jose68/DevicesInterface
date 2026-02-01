cmake_minimum_required(VERSION 3.10)
project(devicesinterface VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Recherche de Java et JNI
find_package(JNI REQUIRED)

# 2. Dossier des headers
include_directories(include ${JNI_INCLUDE_DIRS})

# 3. La DLL (devicesinterface)
add_library(devicesinterface SHARED 
    src/ZebraPrinter.cpp 
    src/DeviceNativeBridge.cpp
)

# On force le nom sans "lib" et on redirige tout vers /bin
set_target_properties(devicesinterface PROPERTIES 
    PREFIX "" 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(devicesinterface PRIVATE setupapi)

# 4. L'exécutable de test
add_executable(zebra_test 
    tests/ZebraPrinterTest.cpp 
    src/ZebraPrinter.cpp
)

set_target_properties(zebra_test PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(zebra_test PRIVATE setupapi)