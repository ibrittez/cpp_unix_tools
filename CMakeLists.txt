cmake_minimum_required(VERSION 3.5)
project(rfid_app LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# Detectar cross-compilación
if(CMAKE_CROSSCOMPILING)
    message(STATUS "=== CROSS-COMPILATION MODE ===")
    set(TARGET_SUFFIX "_arm64")
else()
    message(STATUS "=== NATIVE COMPILATION MODE ===")
    set(TARGET_SUFFIX "")
endif()

# Paths
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(COMMON_DIR ${SRC_DIR}/common)
set(LOGGER_DIR ${SRC_DIR}/logger)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes globales (simple y efectivo)
include_directories(
    ${SRC_DIR}
    ${COMMON_DIR}
    ${LOGGER_DIR}
)

# ======================
# PRODUCTOR
# ======================
add_executable(productor${TARGET_SUFFIX}
    ${SRC_DIR}/productor.cpp
)

target_link_libraries(productor${TARGET_SUFFIX}
    rt
    pthread
)

# ======================
# CONSUMIDOR
# ======================
add_executable(consumidor${TARGET_SUFFIX}
    ${SRC_DIR}/consumidor.cpp
)

target_link_libraries(consumidor${TARGET_SUFFIX}
    rt
    pthread
)

# ======================
# LOGGER (proceso logger standalone)
# ======================
add_executable(logger${TARGET_SUFFIX}
    ${SRC_DIR}/logger.cpp
)

target_link_libraries(logger${TARGET_SUFFIX}
    pthread
)

# ======================
# APP (launcher)
# ======================
add_executable(app${TARGET_SUFFIX}
    ${SRC_DIR}/app.cpp
)

target_link_libraries(app${TARGET_SUFFIX}
    pthread
)

# ======================
# INFO
# ======================
message(STATUS "")
message(STATUS "=== BUILD CONFIGURATION ===")
message(STATUS "Executables:")
message(STATUS "  - productor${TARGET_SUFFIX}")
message(STATUS "  - consumidor${TARGET_SUFFIX}")
message(STATUS "  - logger${TARGET_SUFFIX}")
message(STATUS "  - app${TARGET_SUFFIX}")
message(STATUS "")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")

# ======================
# CLEAN TARGETS
# ======================
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing all build artifacts"
)

add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/*.log
    COMMENT "Removing log files"
)
