#include "Main_menu_manager.hpp"

class MainMenuManager::Impl {
public:
};

MainMenuManager::MainMenuManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : spe::NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "MainMenuManager"}
    , _impl{std::make_unique<Impl>()} {}

MainMenuManager::~MainMenuManager() = default;

void MainMenuManager::_eventDrawGUI() {}
