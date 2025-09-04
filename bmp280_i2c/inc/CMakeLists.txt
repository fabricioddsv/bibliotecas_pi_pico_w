add_library(bmp280 STATIC bmp280.c)
target_include_directories(bmp280 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

# Add any user requested libraries
target_link_libraries(bmp280
    pico_stdlib    
    hardware_i2c
)