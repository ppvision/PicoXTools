# @file
#
# @brief Sets CMake target_sources and target_include_directories
#
# --- The Clear BSD License ---
add_library(sx126x_driver INTERFACE)
target_include_directories(sx126x_driver INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_sources(sx126x_driver INTERFACE
    lr_fhss_mac.c
    sx126x_lr_fhss.c
    sx126x.c
)

