cmake_minimum_required(VERSION 3.10)
project(devicesinterface VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)
include_directories(include ${JNI_INCLUDE_DIRS})

# --- CONFIGURATION FORCEE DU LINKER ---
# On dit explicitement à CMake d'ajouter ces flags à la toute fin de la commande
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static -Wl,--add-stdcall-alias")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

# 1. La DLL
add_library(devicesinterface SHARED 
    src/ZebraPrinter.cpp 
    src/Scale.cpp
    src/DeviceNativeBridge.cpp
)

target_link_libraries(devicesinterface PRIVATE 
    setupapi 
    cfgmgr32 
    winspool
)

set_target_properties(devicesinterface PROPERTIES 
    PREFIX "" 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 2. Test Zebra
add_executable(zebra_test tests/ZebraPrinterTest.cpp src/ZebraPrinter.cpp)
target_link_libraries(zebra_test PRIVATE setupapi cfgmgr32 winspool)
set_target_properties(zebra_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 3. Test Balance
add_executable(scale_test tests/ScaleTest.cpp src/Scale.cpp)
target_link_libraries(scale_test PRIVATE setupapi cfgmgr32)
set_target_properties(scale_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")