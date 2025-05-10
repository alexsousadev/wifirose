# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# WiFi credentials
set(WIFI_SSID "Nome da Rede")
set(WIFI_PASSWORD "Senha da Rede")

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(wifi_rose C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(wifi_rose wifi_rose.c src/joystick/joystick.c src/tcp_utils/tcp_utils.c src/send_joystick/send_joystick.c src/wifi_connect/wifi_connect.c src/server_html/send_html.c)

pico_set_program_name(wifi_rose "wifi_rose")
pico_set_program_version(wifi_rose "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(wifi_rose 0)
pico_enable_stdio_usb(wifi_rose 1)

# Add the standard library to the build
target_link_libraries(wifi_rose
        pico_stdlib hardware_adc)

# Add the standard include files to the build
target_include_directories(wifi_rose PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add WiFi credentials as compile definitions
target_compile_definitions(wifi_rose PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

# Add any user requested libraries
target_link_libraries(wifi_rose 
        pico_cyw43_arch_lwip_threadsafe_background
        )

pico_add_extra_outputs(wifi_rose)

