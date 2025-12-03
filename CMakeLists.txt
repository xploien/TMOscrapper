cmake_minimum_required(VERSION 3.15)
project(TMOscrapper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Directorio de salida
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============================================
# BUSCAR DEPENDENCIAS
# ============================================

# Buscar libcurl
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CURL REQUIRED libcurl)
endif()

# Buscar libzip con pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP REQUIRED libzip)

# Si pkg-config falla, buscar manualmente
if(NOT LIBZIP_FOUND)
  find_path(LIBZIP_INCLUDE_DIR zip.h)
  find_library(LIBZIP_LIBRARY NAMES zip libzip)

  if(LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARY)
    set(LIBZIP_FOUND TRUE)
    set(LIBZIP_LIBRARIES ${LIBZIP_LIBRARY})
    set(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR})
    message(STATUS "✓ LIBZIP found manually at ${LIBZIP_LIBRARY}")
  else()
    message(FATAL_ERROR "libzip not found. Install: sudo apt-get install libzip-dev")
  endif()
endif()

# Buscar threads (pthread)
find_package(Threads REQUIRED)

# ============================================
# ARCHIVOS FUENTE
# ============================================

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/MyApi.cpp"
    "${CMAKE_SOURCE_DIR}/src/Tools.cpp"
    "${CMAKE_SOURCE_DIR}/src/MangaData.cpp"
    "${CMAKE_SOURCE_DIR}/src/Downloader.cpp"
)

# ============================================
# CREAR EJECUTABLE
# ============================================

add_executable(tmo ${SOURCES})

# ============================================
# CONFIGURAR INCLUDES Y LINKEAR LIBRERÍAS
# ============================================

# Incluir directorios de headers
target_include_directories(tmo PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${LIBZIP_INCLUDE_DIRS}
)

# Linkear todas las librerías
target_link_libraries(tmo PRIVATE
    ${CURL_LIBRARIES}
    ${LIBZIP_LIBRARIES}
    Threads::Threads
)

# Método alternativo si el anterior falla (descomenta si es necesario):
# target_link_libraries(tmo PRIVATE curl zip pthread)

# ============================================
# CLANG-TIDY (OPCIONAL)
# ============================================

find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
if(NOT CLANG_TIDY_COMMAND)
  message(WARNING "clang-tidy not found. Code analysis will be skipped.")
else()
  set_target_properties(tmo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

# ============================================
# INFORMACIÓN DE DEBUG
# ============================================

message(STATUS "===========================================")
message(STATUS "  Configuración de TMOscrapper")
message(STATUS "===========================================")

if(CURL_FOUND)
  message(STATUS "✓ CURL found")
  message(STATUS "  Version: ${CURL_VERSION_STRING}")
  message(STATUS "  Include: ${CURL_INCLUDE_DIRS}")
  message(STATUS "  Library: ${CURL_LIBRARIES}")
endif()

if(LIBZIP_FOUND)
  message(STATUS "✓ LIBZIP found")
  message(STATUS "  Include: ${LIBZIP_INCLUDE_DIRS}")
  message(STATUS "  Library: ${LIBZIP_LIBRARIES}")
else()
  message(FATAL_ERROR "✗ LIBZIP not found!")
endif()

if(Threads_FOUND)
  message(STATUS "✓ Threads (pthread) found")
endif()

message(STATUS "===========================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "===========================================")
