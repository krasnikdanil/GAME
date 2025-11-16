#include "AoEDamageSpell.h"

namespace game {

AoEDamageSpell::AoEDamageSpell(const std::string& name, const std::string& description, int effectValue)
    : Spell(name, description, SpellType::AoEDamage, effectValue) {}

std::unique_ptr<Spell> AoEDamageSpell::clone() const {
    return std::make_unique<AoEDamageSpell>(*this);
}

} // namespace game
