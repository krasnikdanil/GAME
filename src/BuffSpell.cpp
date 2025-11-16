#include "BuffSpell.h"

namespace game {

BuffSpell::BuffSpell(const std::string& name, const std::string& description, int effectValue)
    : Spell(name, description, SpellType::Buff, effectValue) {}

std::unique_ptr<Spell> BuffSpell::clone() const {
    return std::make_unique<BuffSpell>(*this);
}

} // namespace game
