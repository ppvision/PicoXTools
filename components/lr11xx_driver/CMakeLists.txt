# @file
#
# @brief Sets CMake target_sources and target_include_directories
#
# --- The Clear BSD License ---
add_library(lr11xx_driver INTERFACE)
target_include_directories(lr11xx_driver INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_sources(lr11xx_driver INTERFACE
  lr11xx_bootloader.c
  lr11xx_crypto_engine.c
  lr11xx_driver_version.c
  lr11xx_gnss.c
  lr11xx_lr_fhss.c
  lr11xx_radio.c
  lr11xx_radio_timings.c
  lr11xx_regmem.c
  lr11xx_system.c
  lr11xx_wifi.c
)

