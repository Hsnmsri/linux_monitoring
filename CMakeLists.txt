cmake_minimum_required(VERSION 3.10)
project(LinuxMonitoring)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(
    src/library
    src/library/log
    src/library/settings
    src/library/cpu
    src/library/memory
    src/library/telegram
    src/library/app
    /usr/local/include # For external libraries
)

# Find required packages
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

# Add source files
set(SOURCES
    src/library/log/Log.cpp
    src/library/settings/Settings.cpp
    src/library/cpu/CpuMonitor.cpp
    src/library/memory/MemoryMonitor.cpp
    src/library/telegram/TelegramMonitor.cpp
    src/library/app/App.cpp
    src/main.cpp
)

# Create the executable
add_executable(LinuxMonitoring ${SOURCES})

# Link libraries
target_link_libraries(LinuxMonitoring
    pthread
    curl
    TgBot
    Boost::boost
    OpenSSL::SSL
    OpenSSL::Crypto
)
