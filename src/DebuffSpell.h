#ifndef DEBUFF_SPELL_H
#define DEBUFF_SPELL_H

#include "Spell.h"
#include <memory>

namespace game {

class DebuffSpell : public Spell {
public:
    DebuffSpell(const std::string& name, const std::string& description, int effectValue);
    std::unique_ptr<Spell> clone() const override;
};

} // namespace game

#endif // DEBUFF_SPELL_H
