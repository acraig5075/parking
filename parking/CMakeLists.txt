add_library(parking STATIC
    Core.cpp
    ParkingImage.cpp
    ParkingLayout.cpp
    WKTParser.cpp
    pch.cpp
)

target_include_directories(parking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_precompile_headers(parking PRIVATE pch.h)

target_compile_definitions(parking PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _LIB
)

target_compile_options(parking PRIVATE -Wall -Wextra -pedantic)
