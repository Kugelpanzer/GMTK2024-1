#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Math.hpp>

#include <iostream>

enum EntityCategories {
    CAT_CHARACTER = 0x01,
    CAT_TERRAIN  = 0x02,
    CAT_LOOT  = 0x04,

};
enum EntityIds {
    EID_CHARACTER,
    EID_TERRAIN,
    EID_LOOT
};

class CharacterInterface : public alvin::EntityBase {
    public:
        using EntitySuperclass = alvin::EntityBase;

        static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_CHARACTER;

        static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_CHARACTER;
        static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_CHARACTER | CAT_TERRAIN | CAT_LOOT;
};

class TerrainInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_TERRAIN;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_TERRAIN;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_CHARACTER;
};
class LootInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_LOOT;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_LOOT;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_CHARACTER;
};