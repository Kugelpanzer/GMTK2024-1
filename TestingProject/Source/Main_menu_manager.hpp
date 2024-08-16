#pragma once

#include "Engine.hpp"
#include "Main_menu_manager_interface.hpp"

#include <memory>

class MainMenuManager
    : public MainMenuManagerInterface
    , public spe::NonstateObject {
public:
    MainMenuManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);
    ~MainMenuManager() override;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;

    void _eventDrawGUI() override;
};
