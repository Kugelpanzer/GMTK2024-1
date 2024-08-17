#include "Environment_manager.hpp"

#include <Hobgoblin/HGExcept.hpp>

#include "Collisions.hpp"

void EnvironmentManager::setToHeadlessHostMode() {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==true, headless==true);
    HG_HARD_ASSERT(_mode == Mode::UNINITIALIZED);
    _mode = Mode::HEADLESS_HOST;

    _collisionDispatcher.emplace();
    _space.emplace();
    InitColliders(*_collisionDispatcher, *_space);
}

void EnvironmentManager::setToClientMode() {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==false, headless==false);
    HG_HARD_ASSERT(_mode == Mode::UNINITIALIZED);
    _mode = Mode::CLIENT;
}

EnvironmentManager::Mode EnvironmentManager::getMode() const {
    return _mode;
}

NeverNull<cpSpace*> EnvironmentManager::getSpace() {
    HG_ASSERT(_mode == Mode::HEADLESS_HOST && _space.has_value());
    return *_space;
}
