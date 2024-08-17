# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

find_package(hobgoblin CONFIG REQUIRED)

add_executable("multiplayer-foundation"
    "_Main.cpp"
    "Lobby_frontend_manager.cpp"
    "Main_gameplay_manager.cpp"
    "Player_character_autodiff.cpp"
    "Player_character_autodiff_alternating.cpp"
    "Player_character_alternating.cpp"
    "Player_character_basic.cpp"
)

target_link_libraries("multiplayer-foundation"
PUBLIC
    hobgoblin::hobgoblin
)
