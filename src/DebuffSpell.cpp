#include "DebuffSpell.h"

namespace game {

DebuffSpell::DebuffSpell(const std::string& name, const std::string& description, int effectValue)
    : Spell(name, description, SpellType::Debuff, effectValue) {}

std::unique_ptr<Spell> DebuffSpell::clone() const {
    return std::make_unique<DebuffSpell>(*this);
}

} // namespace game
