cmake_minimum_required(VERSION 3.20)

project(sokoban)

add_executable(sokoban)

target_sources(sokoban PUBLIC
    src/game.c 
    src/main.c 
    src/menu.c 
    src/renderer.c
)

target_include_directories(sokoban PUBLIC include)

target_link_libraries(sokoban -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf)

set_target_properties(sokoban
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(COPY "${CMAKE_BINARY_DIR}/textures" DESTINATION "${CMAKE_BINARY_DIR}/bin")
file(COPY "${CMAKE_BINARY_DIR}/level" DESTINATION "${CMAKE_BINARY_DIR}/bin")
file(COPY "${CMAKE_BINARY_DIR}/LICENSE" DESTINATION "${CMAKE_BINARY_DIR}/bin")
