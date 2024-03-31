# src/CMakeLists.txt

add_library(peregrine SHARED
    status_code.cc
    system_mock.cc
)
target_link_libraries(peregrine spdlog::spdlog)