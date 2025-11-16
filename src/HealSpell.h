#ifndef HEAL_SPELL_H
#define HEAL_SPELL_H

#include "Spell.h"
#include <memory>

namespace game {

class HealSpell : public Spell {
public:
    HealSpell(const std::string& name, const std::string& description, int effectValue);
    std::unique_ptr<Spell> clone() const override;
};

} // namespace game

#endif // HEAL_SPELL_H
