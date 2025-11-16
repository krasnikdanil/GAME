#include "Spell.h"
#include <iostream>

namespace game {

Spell::Spell(const std::string& name, const std::string& description, SpellType type, int effectValue)
    : name_(name), description_(description), type_(type), effectValue_(effectValue) {}

const std::string& Spell::GetName() const {
    return name_;
}

const std::string& Spell::GetDescription() const {
    return description_;
}

SpellType Spell::GetType() const {
    return type_;
}

int Spell::GetEffectValue() const {
    return effectValue_;
}


} // namespace game
