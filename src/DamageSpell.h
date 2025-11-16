#ifndef DAMAGE_SPELL_H
#define DAMAGE_SPELL_H

#include "Spell.h"
#include <memory>

namespace game {

class DamageSpell : public Spell {
public:
    DamageSpell(const std::string& name, const std::string& description, int effectValue);
    std::unique_ptr<Spell> clone() const override;
};

} // namespace game

#endif // DAMAGE_SPELL_H
