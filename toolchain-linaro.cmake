# Toolchain para Linaro GCC 7.4.1 ARM64
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Path al toolchain de Linaro
set(TOOLCHAIN_PREFIX /opt/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu)

# Compiladores
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}/bin/aarch64-linux-gnu-g++)

# Sysroot (donde están las libs del target)
set(CMAKE_SYSROOT ${TOOLCHAIN_PREFIX}/aarch64-linux-gnu/libc)

# Dónde buscar librerías y headers
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/aarch64-linux-gnu)

# Búsqueda de programas: solo en host
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Búsqueda de librerías y headers: solo en target
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Flags adicionales para ARM
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a")

message(STATUS "Cross-compiling for ARM64 with Linaro GCC 7.4.1")
