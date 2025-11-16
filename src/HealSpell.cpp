#include "HealSpell.h"

namespace game {

HealSpell::HealSpell(const std::string& name, const std::string& description, int effectValue)
    : Spell(name, description, SpellType::Heal, effectValue) {}

std::unique_ptr<Spell> HealSpell::clone() const {
    return std::make_unique<HealSpell>(*this);
}

} // namespace game
