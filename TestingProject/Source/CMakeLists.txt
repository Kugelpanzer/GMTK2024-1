find_package(hobgoblin CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Context_factory.cpp"
    "Lobby_frontend_manager.cpp"
    "Main_menu_manager.cpp"
    "Main.cpp"
 "Character.cpp" "Collisions.cpp")

target_link_libraries(${PROJECT_NAME}
PUBLIC
    hobgoblin::hobgoblin
)
