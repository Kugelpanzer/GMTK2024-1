#include "Character.hpp"

#include "Player_controls.hpp"

#include <Hobgoblin/HGExcept.hpp>

CharacterObject::CharacterObject(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId)
    : SyncObjSuper{aRuntimeRef,
                   SPEMPE_TYPEID_SELF,
                   PRIORITY_PLAYERAVATAR,
                   "CharacterObject",
                   aRegId,
                   aSyncId}
    , _unibody{[this]() {
                   return _initColDelegate();
               },
               [this]() {
                   return hg::alvin::Body::createKinematic();
               },
               [this]() {
                   static constexpr cpFloat WIDTH  = 256.0;
                   static constexpr cpFloat HEIGHT = 128.0;
                   return hg::alvin::Shape::createBox(_unibody.body, WIDTH, HEIGHT);
               }} {
    if (isMasterObject()) {
        _getCurrentState().initMirror(); // To get autodiff optimization working
        _unibody.bindDelegate(*this);
    }
}

CharacterObject::~CharacterObject() {
    if (isMasterObject()) {
        doSyncDestroy();
    }
}

void CharacterObject::init(int aOwningPlayerIndex, float aX, float aY) {
    HG_HARD_ASSERT(isMasterObject());

    auto& self             = _getCurrentState();
    self.x                 = aX;
    self.y                 = aY;
    self.owningPlayerIndex = aOwningPlayerIndex;

    cpBodySetPosition(_unibody, cpv(aX, aY));
}

void CharacterObject::_eventUpdate1(spe::IfMaster) {
    if (ctx().getGameState().isPaused)
        return;

    auto& self = _getCurrentState();
    HG_HARD_ASSERT(self.owningPlayerIndex >= 0);

    auto& lobbyBackend = ccomp<MLobbyBackend>();
    if (const auto clientIndex = lobbyBackend.playerIdxToClientIdx(self.owningPlayerIndex);
        clientIndex != spe::CLIENT_INDEX_UNKNOWN) {

        spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};

        const auto left  = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_LEFT);
        const auto right = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_RIGHT);

        self.x += (10.f * ((float)right - (float)left));

        const auto up   = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_UP);
        const auto down = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_DOWN);

        self.y += (10.f * ((float)down - (float)up));

        wrapper.pollSimpleEvent(clientIndex, CTRL_ID_JUMP, [&]() {
            self.y -= 16.f;
        });
    }
}

void CharacterObject::_eventPostUpdate(spe::IfMaster) {
    _getCurrentState().commit();
}

void CharacterObject::_eventDraw1() {
    if (this->isDeactivated())
        return;

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

    hg::gr::CircleShape circle{32.f};
    circle.setFillColor(COLORS[self.owningPlayerIndex % NUM_COLORS]);
    circle.setOrigin({32.f, 32.f});
    circle.setPosition({self.x, self.y});
    ccomp<MWindow>().getCanvas().draw(circle);

    auto& lobbyBackend = ccomp<MLobbyBackend>();
    if (self.owningPlayerIndex > 0) {
        const auto&  name = lobbyBackend.getLockedInPlayerInfo(self.owningPlayerIndex).name;
        hg::gr::Text text{hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TITILLIUM_REGULAR),
                          name,
                          30};
        text.setFillColor(hg::gr::COLOR_WHITE);
        text.setOutlineColor(hg::gr::COLOR_BLACK);
        text.setOutlineThickness(2.f);
        const auto& localBounds = text.getLocalBounds();
        text.setOrigin({localBounds.w / 2.f, localBounds.h / 2.f});
        text.setPosition({self.x, self.y - 32.f});
        ccomp<MWindow>().getCanvas().draw(text);
    }
}

hg::alvin::CollisionDelegate CharacterObject::_initColDelegate() {
    auto builder = hg::alvin::CollisionDelegateBuilder{};

    builder.addInteraction<LootInterface>(
        hg::alvin::COLLISION_POST_SOLVE,
        [this](LootInterface& aHealth, const hg::alvin::CollisionData& aCollisionData) {
            // DO INTERACTION
        });
    return builder.finalize();
}

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(CharacterObject, (CREATE, UPDATE, DESTROY));

void CharacterObject::_syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(CharacterObject, aSyncCtrl);
}

void CharacterObject::_syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(CharacterObject, aSyncCtrl);
}

void CharacterObject::_syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(CharacterObject, aSyncCtrl);
}
