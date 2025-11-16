#include "DamageSpell.h"

namespace game {

DamageSpell::DamageSpell(const std::string& name, const std::string& description, int effectValue)
    : Spell(name, description, SpellType::Damage, effectValue) {}

std::unique_ptr<Spell> DamageSpell::clone() const {
    return std::make_unique<DamageSpell>(*this);
}

} // namespace game
