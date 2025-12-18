cmake_minimum_required(VERSION 3.5)
project(ModernMessageQueue)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags de compilación (agregar pthread)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread")

# Detectar target (host o ARM)
if(CMAKE_CROSSCOMPILING)
    message(STATUS "=== CROSS-COMPILATION MODE ===")
    message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "Toolchain: ${CMAKE_CXX_COMPILER}")
    set(TARGET_SUFFIX "_arm64")
else()
    message(STATUS "=== NATIVE COMPILATION MODE ===")
    message(STATUS "Host: ${CMAKE_SYSTEM_PROCESSOR}")
    set(TARGET_SUFFIX "")
endif()

# Directorios
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Headers
include_directories(${SRC_DIR})

# Ejecutable: Productor
add_executable(productor${TARGET_SUFFIX}
    ${SRC_DIR}/productor.cpp
)

# Ejecutable: Consumidor (con logger_thread)
add_executable(consumidor${TARGET_SUFFIX}
    ${SRC_DIR}/consumidor.cpp
    ${SRC_DIR}/logger_thread.cpp
)

# Librerías necesarias
# rt: POSIX message queues y realtime
# pthread: threads (std::thread, std::mutex, std::atomic)
target_link_libraries(productor${TARGET_SUFFIX} rt pthread)
target_link_libraries(consumidor${TARGET_SUFFIX} rt pthread)

# Info de build
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Limpieza total
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing all build artifacts"
)

# Target para limpiar logs
add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/*.log
    COMMENT "Removing log files"
)

# Resumen
message(STATUS "")
message(STATUS "=== BUILD CONFIGURATION ===")
message(STATUS "Executables:")
message(STATUS "  - productor${TARGET_SUFFIX}")
message(STATUS "  - consumidor${TARGET_SUFFIX} (with logger thread)")
message(STATUS "")
message(STATUS "Source files:")
message(STATUS "  - ${SRC_DIR}/productor.cpp")
message(STATUS "  - ${SRC_DIR}/consumidor.cpp")
message(STATUS "  - ${SRC_DIR}/logger_thread.cpp")
message(STATUS "")
message(STATUS "Headers:")
message(STATUS "  - MessageQueue.hpp (template)")
message(STATUS "  - RingBuffer.hpp")
message(STATUS "  - TagData.hpp")
message(STATUS "  - logger_thread.hpp")
message(STATUS "")
message(STATUS "To build:")
message(STATUS "  make -j$(nproc)")
message(STATUS "")
message(STATUS "To clean logs:")
message(STATUS "  make clean-logs")
message(STATUS "")
