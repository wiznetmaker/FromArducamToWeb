# ioLibrary_Driver
add_library(IOLIBRARY_FILES STATIC)

target_sources(IOLIBRARY_FILES PUBLIC
        ${PORT_DIR}/ioLibrary_Driver/src/w5x00_spi.c
        ${PORT_DIR}/ioLibrary_Driver/src/w5x00_gpio_irq.c
        )

if(${BOARD_NAME} STREQUAL W55RP20_EVB_PICO)
pico_generate_pio_header(IOLIBRARY_FILES ${PORT_DIR}/ioLibrary_Driver/src/wiznet_spi_pio.pio)

target_sources(IOLIBRARY_FILES PUBLIC 
        ${PORT_DIR}/ioLibrary_Driver/src/wiznet_spi_pio.c
        )
endif()

target_include_directories(IOLIBRARY_FILES PUBLIC
        ${WIZNET_DIR}/Ethernet
        ${PORT_DIR}/ioLibrary_Driver/inc
        ${PORT_DIR}
        )

target_link_libraries(IOLIBRARY_FILES PUBLIC
        pico_stdlib
        hardware_pio
        hardware_spi
        hardware_dma
        hardware_clocks
        )

# HTTP_SERVER
add_library(HTTPSERVER_FILES STATIC)

target_sources(HTTPSERVER_FILES PUBLIC
        ${PORT_DIR}/http_server/src/httpCamera.c
        ${PORT_DIR}/http_server/src/httpParser.c
        ${PORT_DIR}/http_server/src/httpServer.c
        ${PORT_DIR}/http_server/src/httpUtil.c
        )

target_include_directories(HTTPSERVER_FILES PUBLIC
        ${PORT_DIR}/http_server/inc
        ${PORT_DIR}
        )
        
target_link_libraries(HTTPSERVER_FILES PUBLIC
        IOLIBRARY_FILES
        arducam
        )


# timer
add_library(TIMER_FILES STATIC)

target_sources(TIMER_FILES PUBLIC
        ${PORT_DIR}/timer/timer.c
        )

target_include_directories(TIMER_FILES PUBLIC
        ${PORT_DIR}/timer
        )

target_link_libraries(TIMER_FILES PRIVATE
        pico_stdlib      
        )
