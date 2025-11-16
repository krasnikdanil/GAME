#ifndef AOE_DAMAGE_SPELL_H
#define AOE_DAMAGE_SPELL_H

#include "Spell.h"
#include <memory>

namespace game {

class AoEDamageSpell : public Spell {
public:
    AoEDamageSpell(const std::string& name, const std::string& description, int effectValue);
    std::unique_ptr<Spell> clone() const override;
};

} // namespace game

#endif // AOE_DAMAGE_SPELL_H
