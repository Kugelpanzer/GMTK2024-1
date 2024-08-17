// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include "Player_character_autodiff.hpp"

AutodiffPlayerCharacter::AutodiffPlayerCharacter(QAO_RuntimeRef aRuntimeRef,
                                                 spe::RegistryId aRegId,
                                                 spe::SyncId aSyncId)
    : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, PRIORITY_PLAYERAVATAR,
                   "AutodiffPlayerCharacter", aRegId, aSyncId}
{
    if (isMasterObject()) {
        _getCurrentState().initMirror();
    }
}

AutodiffPlayerCharacter::~AutodiffPlayerCharacter() {
    if (isMasterObject()) {
        doSyncDestroy();
    }
}

void AutodiffPlayerCharacter::init(int aOwningPlayerIndex, float aX, float aY) {
    assert(isMasterObject());

    auto& self = _getCurrentState();
    self.x = aX;
    self.y = aY;
    self.owningPlayerIndex = aOwningPlayerIndex;
}

void AutodiffPlayerCharacter::_eventUpdate1(spe::IfMaster) {
    if (ctx().getGameState().isPaused) return;

    auto& self = _getCurrentState();
    assert(self.owningPlayerIndex >= 0);
    if (const auto clientIndex = ccomp<MLobbyBackend>().playerIdxToClientIdx(self.owningPlayerIndex);
        clientIndex != spe::CLIENT_INDEX_UNKNOWN) {

        spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};

        const bool left = wrapper.getSignalValue<bool>(clientIndex, "left");
        const bool right = wrapper.getSignalValue<bool>(clientIndex, "right");

        self.x += (10.f * ((float)right - (float)left));

        const bool up = wrapper.getSignalValue<bool>(clientIndex, "up");
        const bool down = wrapper.getSignalValue<bool>(clientIndex, "down");

        self.y += (10.f * ((float)down - (float)up));

        wrapper.pollSimpleEvent(clientIndex, "jump",
                                [&]() {
            self.y -= 16.f;
        });
    }
}

void AutodiffPlayerCharacter::_eventPostUpdate(spe::IfMaster) {
    _getCurrentState().commit();
}

void AutodiffPlayerCharacter::_eventDraw1() {
    if (this->isDeactivated()) return;

    #define NUM_COLORS 12
    static const hg::gr::Color COLORS[NUM_COLORS] = {
        hg::gr::COLOR_RED,
        hg::gr::COLOR_GREEN,
        hg::gr::COLOR_YELLOW,
        hg::gr::COLOR_BLUE,
        hg::gr::COLOR_ORANGE,
        hg::gr::COLOR_PURPLE,
        hg::gr::COLOR_TEAL,
        hg::gr::COLOR_BROWN,
        hg::gr::COLOR_FUCHSIA,
        hg::gr::COLOR_GREY,
        hg::gr::COLOR_WHITE,
        hg::gr::COLOR_AQUA,
    };

    const auto& self = _getCurrentState();

    hg::gr::CircleShape circle{20.f};
    circle.setFillColor(COLORS[self.owningPlayerIndex % NUM_COLORS]);
    circle.setPosition({self.x, self.y});
    ccomp<MWindow>().getCanvas().draw(circle);
}

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(AutodiffPlayerCharacter, (CREATE, UPDATE, DESTROY));

void AutodiffPlayerCharacter::_syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(AutodiffPlayerCharacter, aSyncCtrl);
}

void AutodiffPlayerCharacter::_syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(AutodiffPlayerCharacter, aSyncCtrl);
}

void AutodiffPlayerCharacter::_syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(AutodiffPlayerCharacter, aSyncCtrl);
}

// clang-format on
