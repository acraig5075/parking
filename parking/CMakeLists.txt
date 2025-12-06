add_library(parking STATIC
    Core.cpp
    ParkingLayout.cpp
    parking.cpp
    pch.cpp
)

target_include_directories(parking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(parking PRIVATE pch.h)

target_compile_definitions(parking PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _LIB
)

target_compile_options(parking PRIVATE -Wall -Wextra -pedantic)
