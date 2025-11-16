#ifndef SPELL_HAND_H
#define SPELL_HAND_H

#include "Spell.h"
#include <vector>
#include <memory> // Для std::unique_ptr

namespace game {

class SpellHand {
public:
    SpellHand(size_t max_size);
    SpellHand(const SpellHand& other); // Конструктор копирования
    SpellHand& operator=(const SpellHand& other); // Оператор присваивания копированием
    SpellHand(SpellHand&& other) noexcept; // Перемещающий конструктор
    SpellHand& operator=(SpellHand&& other) noexcept; // Перемещающий оператор присваивания

    bool AddSpell(std::unique_ptr<Spell> spell);
    std::unique_ptr<Spell> UseSpell(int index); // Возвращаем unique_ptr для передачи владения
    const std::vector<std::unique_ptr<Spell>>& GetSpells() const;
    size_t GetMaxSize() const;

private:
    std::vector<std::unique_ptr<Spell>> spells_;
    size_t max_size_;
};

} // namespace game

#endif // SPELL_HAND_H
