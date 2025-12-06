add_executable(console console.cpp)

target_link_libraries(console PRIVATE parking)

target_compile_definitions(console PRIVATE
    _CONSOLE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(console PRIVATE -Wall -Wextra -pedantic)
