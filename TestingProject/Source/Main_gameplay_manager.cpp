#include "Main_gameplay_manager.hpp"

#include "Character.hpp"
#include "Player_controls.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

RN_DEFINE_RPC(SetGlobalStateBufferingLength, RN_ARGS(unsigned, aNewLength)) {
    RN_NODE_IN_HANDLER().callIfClient([=](RN_ClientInterface& aClient) {
        const auto rc = spe::RPCReceiverContext(aClient);
        rc.gameContext.getComponent<MNetworking>().setStateBufferingLength(aNewLength);
        rc.gameContext.getComponent<MInput>().setStateBufferingLength(aNewLength);
        HG_LOG_INFO(LOG_ID, "Global state buffering set to {} frames.", aNewLength);
    });
    RN_NODE_IN_HANDLER().callIfServer([](RN_ServerInterface&) {
        throw RN_IllegalMessage();
    });
}

MainGameplayManager::MainGameplayManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "GameplayManager"} {
    auto& netMgr = ccomp<MNetworking>();
    netMgr.addEventListener(this);
    stateBufferingLength = netMgr.getStateBufferingLength();
}

MainGameplayManager::~MainGameplayManager() {
    ccomp<MNetworking>().removeEventListener(this);
}

void MainGameplayManager::setToHostMode(hg::PZInteger aPlayerCount) {
    HG_VALIDATE_PRECONDITION(_mode == Mode::UNINITIALIZED);
    _mode = Mode::HOST;
    // _startGame(aPlayerCount);
}

void MainGameplayManager::setToClientMode() {
    HG_VALIDATE_PRECONDITION(_mode == Mode::UNINITIALIZED);
    _mode = Mode::CLIENT;

    auto& views = ccomp<MWindow>().getViewController();
    views.getView(0).setSize({1920.f, 1080.f});
    views.getView(0).setViewport({0.f, 0.f, 1.f, 1.f});
    views.getView(0).setCenter({0.f, 0.f});
}

MainGameplayManager::Mode MainGameplayManager::getMode() const {
    return _mode;
}

void MainGameplayManager::_startGame(hg::PZInteger aPlayerCount) {
    for (hg::PZInteger i = 0; i < aPlayerCount; i += 1) {
        if (i == 0) {
            continue; // player 0 is the host, doesn't need a character
        }
        auto* obj = QAO_PCreate<CharacterObject>(ctx().getQAORuntime(),
                                                 ccomp<MNetworking>().getRegistryId(),
                                                 spe::SYNC_ID_NEW);
        obj->init(i, i * 300.0, 300.0);
    }
}

void MainGameplayManager::_eventUpdate1() {
#if 0
    if (ctx().isPrivileged()) {
        auto& winMgr = ccomp<MWindow>();

        const int MAX_BUFFERING_LENGTH = 10;
        bool sync = false;

        if (winMgr.getInput().checkPressed(hg::in::PK_I,
                                           spe::WindowFrameInputView::Mode::Direct)) {
            stateBufferingLength = (stateBufferingLength + 1) % (MAX_BUFFERING_LENGTH + 1);
            sync = true;
        }
        if (winMgr.getInput().checkPressed(hg::in::PK_U,
                                           spe::WindowFrameInputView::Mode::Direct)) {
            stateBufferingLength = (stateBufferingLength + MAX_BUFFERING_LENGTH) % (MAX_BUFFERING_LENGTH + 1);
            sync = true;
        }

        if (sync) {
            HG_LOG_INFO(LOG_ID, "Global state buffering set to {} frames.", stateBufferingLength);
            Compose_SetGlobalStateBufferingLength(
                ccomp<MNetworking>().getNode(),
                RN_COMPOSE_FOR_ALL,
                stateBufferingLength
            );
        }

        return;
    }
#endif
    if (!ctx().isPrivileged()) {
        auto& client = ccomp<MNetworking>().getClient();
        // If connected, upload input
        if (client.getServerConnector().getStatus() == RN_ConnectorStatus::Connected) {
            const auto input = ccomp<MWindow>().getInput();

            const bool left  = input.checkPressed(hg::in::PK_A);
            const bool right = input.checkPressed(hg::in::PK_D);
            const bool up    = input.checkPressed(hg::in::PK_W);
            const bool down  = input.checkPressed(hg::in::PK_S);
            const bool jump =
                input.checkPressed(hg::in::PK_SPACE, spe::WindowFrameInputView::Mode::Edge);

            spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};
            wrapper.setSignalValue<bool>(CTRL_ID_LEFT, left);
            wrapper.setSignalValue<bool>(CTRL_ID_RIGHT, right);
            wrapper.setSignalValue<bool>(CTRL_ID_UP, up);
            wrapper.setSignalValue<bool>(CTRL_ID_DOWN, down);
            wrapper.triggerEvent(CTRL_ID_JUMP, jump);
        }
    }
}

void MainGameplayManager::_eventDrawGUI() {
    // Do nothing
    hg::gr::CircleShape circle{32.f};
    circle.setFillColor(hg::gr::COLOR_RED);
    circle.setPosition({60.f, 60.f});
    ccomp<MWindow>().getCanvas().draw(circle);
}

void MainGameplayManager::_eventPostUpdate() {
    const auto input = ccomp<MWindow>().getInput();
    if (input.checkPressed(hg::in::PK_F9, spe::WindowFrameInputView::Mode::Direct)) {
        // Stopping the context will delete:
        // - All objects owned by the QAO runtime (in undefined order)
        // - Then, all ContextComponents owned by the context (in reverse order of insertion)
        ctx().stop();
    }
}

void MainGameplayManager::onNetworkingEvent(const hg::RN_Event& aEvent) {
    if (ccomp<MNetworking>().isClient()) {
        // CLIENT
        aEvent.visit([this](const RN_Event::Connected& ev) {
            HG_LOG_INFO(LOG_ID, "Client lobby uploading local info to server.");
            ccomp<MLobbyBackend>().uploadLocalInfo();
        });
    } else {
        // HOST
        aEvent.visit([this](const RN_Event::Connected& ev) {},
                     [](const RN_Event::Disconnected& ev) {
                         // TODO Remove player avatar
                     });
    }
}
