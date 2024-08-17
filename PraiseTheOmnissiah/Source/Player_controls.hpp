// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#pragma once

#include "Engine.h"

struct PlayerControls {
    bool left  = false;
    bool right = false;
    bool up    = false;
    bool down  = false;
    bool jump  = false;
};

constexpr auto CTRLNAME_LEFT  = "left";
constexpr auto CTRLNAME_RIGHT = "right";
constexpr auto CTRLNAME_UP    = "up";
constexpr auto CTRLNAME_DOWN  = "down";
constexpr auto CTRLNAME_JUMP  = "jump";

inline
void SetUpPlayerControlsDefinitions(spe::InputSyncManagerInterface& aInputSyncManager) {
    spe::InputSyncManagerWrapper wrapper{aInputSyncManager};
    wrapper.defineSignal<bool>(CTRLNAME_LEFT,  false);
    wrapper.defineSignal<bool>(CTRLNAME_RIGHT, false);
    wrapper.defineSignal<bool>(CTRLNAME_UP,    false);
    wrapper.defineSignal<bool>(CTRLNAME_DOWN,  false);
    wrapper.defineSimpleEvent(CTRLNAME_JUMP);
}

// clang-format on
