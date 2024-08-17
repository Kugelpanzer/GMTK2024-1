// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#pragma once

#include "Engine.h"

#include <string>

class LobbyFrontendManagerInterface
    : public spe::ContextComponent
{
public:
    enum class Mode {
        Uninitialized,
        HeadlessHost,
        Client,
    };

    ~LobbyFrontendManagerInterface() override = default;

    virtual void setToHeadlessHostMode() = 0;
    //virtual void setToHostMode() = 0;
    virtual void setToClientMode(const std::string& aName, const std::string& aUniqueId) = 0;
    
    virtual Mode getMode() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("LobbyFrontendManagerInterface");
};

// clang-format on
