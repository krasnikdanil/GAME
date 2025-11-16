#ifndef BUFF_SPELL_H
#define BUFF_SPELL_H

#include "Spell.h"
#include <memory>

namespace game {

class BuffSpell : public Spell {
public:
    BuffSpell(const std::string& name, const std::string& description, int effectValue);
    std::unique_ptr<Spell> clone() const override;
};

} // namespace game

#endif // BUFF_SPELL_H
