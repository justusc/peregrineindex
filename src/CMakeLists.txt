# src/CMakeLists.txt

add_library(peregrine SHARED
    status_code.cc
    system.cc
)
target_link_libraries(peregrine spdlog::spdlog)