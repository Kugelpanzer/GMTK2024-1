// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include "Main_gameplay_manager.hpp"

#include "Player_controls.hpp"

#include <Hobgoblin/Logging.hpp>

namespace {
constexpr auto LOG_ID = "MultiplayerFoundation";
} // namespace

RN_DEFINE_RPC(SetGlobalStateBufferingLength, RN_ARGS(unsigned, aNewLength)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [=](RN_ClientInterface& aClient) {
            const auto rc = spe::RPCReceiverContext(aClient);
            rc.gameContext.getComponent<MNetworking>().setStateBufferingLength(aNewLength);
            rc.gameContext.getComponent<MInput>().setStateBufferingLength(aNewLength);
            HG_LOG_INFO(LOG_ID, "Global state buffering set to {} frames.", aNewLength);
        });
    RN_NODE_IN_HANDLER().callIfServer(
        [](RN_ServerInterface&) {
            throw RN_IllegalMessage();
        });
}

MainGameplayManager::MainGameplayManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "GameplayManager"}
{
    auto& netMgr = ccomp<MNetworking>();
    netMgr.addEventListener(this);
    stateBufferingLength = netMgr.getStateBufferingLength();
}

MainGameplayManager::~MainGameplayManager() {
    ccomp<MNetworking>().removeEventListener(this);
}

void MainGameplayManager::_eventUpdate1() {
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

    if (!ctx().isPrivileged()) {
        auto& client = ccomp<MNetworking>().getClient();
        if (client.getServerConnector().getStatus() == RN_ConnectorStatus::Connected) {
            const auto input = ccomp<MWindow>().getInput();
            PlayerControls controls{
                input.checkPressed(hg::in::PK_A),
                input.checkPressed(hg::in::PK_D),
                input.checkPressed(hg::in::PK_W),
                input.checkPressed(hg::in::PK_S),
                input.checkPressed(hg::in::PK_SPACE, spe::WindowFrameInputView::Mode::Edge)
            };

            spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};
            wrapper.setSignalValue<bool>(CTRLNAME_LEFT,  controls.left);
            wrapper.setSignalValue<bool>(CTRLNAME_RIGHT, controls.right);
            wrapper.setSignalValue<bool>(CTRLNAME_UP,    controls.up);
            wrapper.setSignalValue<bool>(CTRLNAME_DOWN,  controls.down);
            wrapper.triggerEvent(CTRLNAME_JUMP, controls.jump);
        }
    }
}

void MainGameplayManager::_eventDrawGUI() {
    // Do nothing
}

void MainGameplayManager::_eventPostUpdate() {
    const auto input = ccomp<MWindow>().getInput();
    if (input.checkPressed(hg::in::PK_F9, spe::WindowFrameInputView::Mode::Direct)) {
        // Stopping the context will delete:
        // - All objects owned by the QAO runtime (in undefined order)
        // - Then, all ContextComponents owned by the context (in reverse order of insertion)
        ctx().stop();
    }

    printBandwidthUsageCountdown -= 1;
    if (printBandwidthUsageCountdown == 0) {
        printBandwidthUsageCountdown = 120;
        auto& netMgr = ccomp<MNetworking>();
        const auto telemetry = netMgr.getTelemetry(120);
        HG_LOG_INFO(
            LOG_ID,
            "Bandwidth usage in the last 120 frame(s): {:6.2f}kB UP, {:6.2f}kB DOWN.",
            static_cast<double>(telemetry.uploadByteCount) / 1024.0,
            static_cast<double>(telemetry.downloadByteCount) / 1024.0
        );
    }
}

void MainGameplayManager::onNetworkingEvent(const hg::RN_Event& aEvent) {
    if (ccomp<MNetworking>().isClient()) {
        // CLIENT
        aEvent.visit(
            [this](const RN_Event::Connected& ev) {
                HG_LOG_INFO(LOG_ID, "Client lobby uploading local info to server.");
                ccomp<MLobbyBackend>().uploadLocalInfo();
            }
        );
    }
    else {
        // HOST
        aEvent.visit(
            [this](const RN_Event::Connected& ev) {
            },
            [](const RN_Event::Disconnected& ev) {
                // TODO Remove player avatar
            }
        );
    }
}

// clang-format on
