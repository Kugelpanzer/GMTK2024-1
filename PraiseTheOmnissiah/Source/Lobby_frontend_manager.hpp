// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#pragma once

#include "Engine.h"
#include "Lobby_frontend_manager_interface.hpp"

#include <memory>

class LobbyFrontendManager
    : public LobbyFrontendManagerInterface
    , public spe::NonstateObject
{
public:
    LobbyFrontendManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);
    ~LobbyFrontendManager() override;

    void setToHeadlessHostMode() override;
    void setToClientMode(const std::string& aName, const std::string& aUniqueId) override;
    Mode getMode() const override;

private:
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;

    void _eventBeginUpdate() override;
    void _eventUpdate1() override;
    void _eventDrawGUI() override;

    friend void ActivateCommand(LobbyFrontendManager& aMgr, int aCommand, void* aArgs);
};

// clang-format on
