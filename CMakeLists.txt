cmake_minimum_required(VERSION 3.25)

project(
  glotter
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(kotki REQUIRED)
find_package(marian-lite REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(
  ICU
  COMPONENTS uc i18n
  REQUIRED)

# Configure mecab for usage
execute_process(
  COMMAND mecab-config --cflags
  OUTPUT_VARIABLE MECAB_CFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND mecab-config --libs
  OUTPUT_VARIABLE MECAB_LIBS
  OUTPUT_STRIP_TRAILING_WHITESPACE)

include(GNUInstallDirs)

set(ASSETS_DIR_NAME
    "assets"
    CACHE STRING "Path to the assets directory")

set(MODELS_REGISTRY_FILE_PATH
    "$ENV{HOME}/.local/share/localtranslate/models/firefox/registry.json"
    CACHE STRING "Path to the translation models registry.json file")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(ASSETS_PATH
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${ASSETS_DIR_NAME}"
      CACHE STRING "Path to the assets directory")
else()
  file(COPY ${ASSETS_DIR_NAME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  set(ASSETS_PATH
      "${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_DIR_NAME}"
      CACHE STRING "Path to the assets directory")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config/config.hpp")

add_executable(${PROJECT_NAME} src/main.cpp src/translation-bridge.cpp)

target_include_directories(
  ${PROJECT_NAME} PRIVATE include external "${CMAKE_CURRENT_BINARY_DIR}/config")
target_compile_options(${PROJECT_NAME} PRIVATE ${MECAB_CFLAGS})
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE nlohmann_json::nlohmann_json
          Threads::Threads
          kotki
          marian-lite
          yaml-cpp
          ICU::uc
          ICU::i18n
          ${MECAB_LIBS})

install(TARGETS ${PROJECT_NAME} COMPONENT Application)
install(
  DIRECTORY ${ASSETS_DIR_NAME}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  COMPONENT Assets)
