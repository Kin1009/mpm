# fxSDK project, build with `fxsdk build-cg` to use the right toolchain.
cmake_minimum_required(VERSION 3.15)
project(MpmLoader LANGUAGES C ASM)

set(SOURCES
  addins.c
  casiowin.S
  fxlibc_hal.c
  loader.c
  logo.c
  main.c
  start.c
  util.c)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/casiowin.inc"
  COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/casiowin_gen.py"
                 "${CMAKE_CURRENT_SOURCE_DIR}/casiowin.yaml"
                 "${CMAKE_CURRENT_BINARY_DIR}/casiowin.inc"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/casiowin_gen.py"
          "${CMAKE_CURRENT_SOURCE_DIR}/casiowin.yaml")
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/casiowin.S"
  PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/casiowin.inc")

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/logo.inc"
  COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/logo_gen.py"
                 "${CMAKE_CURRENT_SOURCE_DIR}/logo.png"
                 "${CMAKE_CURRENT_BINARY_DIR}/logo.inc"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/logo_gen.py"
          "${CMAKE_CURRENT_SOURCE_DIR}/logo.png")
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/logo.c"
  PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/logo.inc")

add_executable(loader ${SOURCES})
set_target_properties(loader PROPERTIES OUTPUT_NAME "mpm.bin")
target_compile_options(loader PRIVATE -Wall -Wextra -Os -g -flto=auto)
target_link_options(loader PRIVATE -T "${CMAKE_CURRENT_SOURCE_DIR}/loader-cg.x")

# We link with what's likely a gint-oriented build of fxlibc, with no default
# HAL definitions. We provide a few in fxlibc_hal.c.
target_link_libraries(loader -lc)

add_custom_command(TARGET loader POST_BUILD
  COMMAND echo -ne '\\x1b[36;1mmpm.bin size: \\x1b[0m'
  COMMAND stat -c %s "${CMAKE_CURRENT_BINARY_DIR}/mpm.bin")
