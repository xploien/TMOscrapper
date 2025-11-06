cmake_minimum_required(VERSION 3.15)
project(TMOscrapper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Directorio de salida
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Buscar libcurl
find_package(CURL REQUIRED)

# Si find_package no funciona, intenta con pkg-config
if(NOT CURL_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CURL REQUIRED libcurl)
endif()

# Archivos fuente
file(GLOB_RECURSE SOURCES
    # "${CMAKE_SOURCE_DIR}/parse.cpp"
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/MyApi.cpp"
    "${CMAKE_SOURCE_DIR}/src/Tools.cpp"
    "${CMAKE_SOURCE_DIR}/src/MangaData.cpp"
)

# Crear ejecutable
add_executable(tmo ${SOURCES})

# Linkear libcurl
target_link_libraries(tmo PRIVATE CURL::libcurl)

# Si el método anterior no funciona, usa esto:
# target_link_libraries(tmo PRIVATE ${CURL_LIBRARIES})
# target_include_directories(tmo PRIVATE ${CURL_INCLUDE_DIRS})

# Clang-tidy (opcional)
find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
if(NOT CLANG_TIDY_COMMAND)
  message(WARNING "clang-tidy not found. Code analysis will be skipped.")
else()
  set_target_properties(tmo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

# Mostrar información de curl (útil para debug)
if(CURL_FOUND)
  message(STATUS "CURL found: ${CURL_VERSION_STRING}")
  message(STATUS "CURL include dirs: ${CURL_INCLUDE_DIRS}")
  message(STATUS "CURL libraries: ${CURL_LIBRARIES}")
endif()
