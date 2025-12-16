cmake_minimum_required(VERSION 3.15)
project(TMOscrapper)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============================================
# BUSCAR DEPENDENCIAS
# ============================================

# CURL
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CURL REQUIRED libcurl)
endif()

find_package(PkgConfig REQUIRED)


# LIBZIP
pkg_check_modules(LIBZIP REQUIRED libzip)
find_package(raylib REQUIRED)


if(NOT LIBZIP_FOUND)
  find_path(LIBZIP_INCLUDE_DIR zip.h)
  find_library(LIBZIP_LIBRARY NAMES zip libzip)
  if(LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARY)
    set(LIBZIP_FOUND TRUE)
    set(LIBZIP_LIBRARIES ${LIBZIP_LIBRARY})
    set(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR})
    message(STATUS "✓ LIBZIP found manually at ${LIBZIP_LIBRARY}")
  else()
    message(FATAL_ERROR "libzip not found. Install: sudo apt-get install libzip-dev (o equivalente)")
  endif()
endif()

find_package(Threads REQUIRED)

# ============================================
# ARCHIVOS FUENTE (ESPECIFICAR MANUALMENTE)
# ============================================
# NO uses GLOB_RECURSE porque incluye archivos no deseados
set(SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/Curl.cpp"
    "${CMAKE_SOURCE_DIR}/src/MyApi.cpp"
    "${CMAKE_SOURCE_DIR}/src/Tools.cpp"
    "${CMAKE_SOURCE_DIR}/src/MangaData.cpp"
    "${CMAKE_SOURCE_DIR}/src/Downloader.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/Clay.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/Gui.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/ImageLoader.cpp"
)

# ============================================
# CREAR EJECUTABLE
# ============================================
add_executable(tmo ${SOURCES})

# ============================================
# CONFIGURAR INCLUDES Y LINKEAR LIBRERÍAS
# ============================================
target_include_directories(tmo PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${LIBZIP_INCLUDE_DIRS}
)

target_compile_options(tmo PRIVATE -Wno-error=narrowing)

target_link_libraries(tmo PRIVATE
    curl
    zip
    pthread
    raylib
)

# ============================================
# CLANG-TIDY (OPCIONAL)
# ============================================
find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
if(NOT CLANG_TIDY_COMMAND)
  message(WARNING "clang-tidy not found. Code analysis will be skipped.")
else()
  set_target_properties(tmo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()
