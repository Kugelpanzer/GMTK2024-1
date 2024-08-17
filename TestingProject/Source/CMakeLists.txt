find_package(hobgoblin CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Character.cpp"
    "Collisions.cpp"
    "Context_factory.cpp"
    "Environment_manager.cpp"
    "Lobby_frontend_manager.cpp"
    "Main_gameplay_manager.cpp"
    "Main_menu_manager.cpp"
    "Main.cpp"
 )

target_link_libraries(${PROJECT_NAME}
PUBLIC
    hobgoblin::hobgoblin
)
