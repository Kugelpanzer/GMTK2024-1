#pragma once

#include "Engine.hpp"

#include "Environment_manager_interface.hpp"

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <memory>
#include <optional>

class EnvironmentManager
    : public EnvironmentManagerInterface
    , public spe::NonstateObject {
public:
    EnvironmentManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);
    ~EnvironmentManager() override;

    void setToHeadlessHostMode() override;
    void setToClientMode() override;
    Mode getMode() const override;

    NeverNull<cpSpace*> getSpace() override;

private:
    Mode _mode = Mode::UNINITIALIZED;

    std::optional<hg::alvin::MainCollisionDispatcher> _collisionDispatcher;
    std::optional<hg::alvin::Space>                   _space;

    // TODO: grid of cells (mountain)

    void _eventBeginUpdate() override;
    void _eventUpdate1() override;
    void _eventDrawGUI() override;
};
