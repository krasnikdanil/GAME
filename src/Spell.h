#ifndef SPELL_H
#define SPELL_H

#include <string>
#include <vector>
#include <memory>

namespace game {

enum class SpellType {
    Damage,
    Heal,
    Buff,
    Debuff,
    AoEDamage // Добавляем новый тип заклинания для урона по площади
};

class Spell {
public:
    Spell(const std::string& name, const std::string& description, SpellType type, int effectValue);
    virtual ~Spell() = default; // Виртуальный деструктор для полиморфизма

    const std::string& GetName() const;
    const std::string& GetDescription() const;
    SpellType GetType() const;
    int GetEffectValue() const;

    // Виртуальный метод для клонирования заклинания
    virtual std::unique_ptr<Spell> clone() const = 0;

private:
    std::string name_;
    std::string description_;
    SpellType type_;
    int effectValue_;
};

} // namespace game

#endif // SPELL_H
