#include "SpellHand.h"
#include <iostream> // Для отладочного вывода

namespace game {

SpellHand::SpellHand(size_t max_size) : max_size_(max_size) {}

// Конструктор копирования
SpellHand::SpellHand(const SpellHand& other) : max_size_(other.max_size_) {
    for (const auto& spell_ptr : other.spells_) {
        spells_.push_back(spell_ptr->clone());
    }
}

// Оператор присваивания копированием
SpellHand& SpellHand::operator=(const SpellHand& other) {
    if (this != &other) {
        spells_.clear();
        max_size_ = other.max_size_;
        for (const auto& spell_ptr : other.spells_) {
            spells_.push_back(spell_ptr->clone());
        }
    }
    return *this;
}

// Перемещающий конструктор
SpellHand::SpellHand(SpellHand&& other) noexcept
    : spells_(std::move(other.spells_)), max_size_(other.max_size_) {
    other.max_size_ = 0;
}

// Перемещающий оператор присваивания
SpellHand& SpellHand::operator=(SpellHand&& other) noexcept {
    if (this != &other) {
        spells_ = std::move(other.spells_);
        max_size_ = other.max_size_;
        other.max_size_ = 0;
    }
    return *this;
}

bool SpellHand::AddSpell(std::unique_ptr<Spell> spell) {
    if (spells_.size() < max_size_) {
        spells_.push_back(std::move(spell));
        return true;
    }
    std::cout << "Spell hand is full. Cannot add more spells." << std::endl;
    return false;
}

std::unique_ptr<Spell> SpellHand::UseSpell(int index) {
    if (index >= 0 && static_cast<size_t>(index) < spells_.size()) {
        std::unique_ptr<Spell> spell = std::move(spells_[index]);
        spells_.erase(spells_.begin() + index);
        return spell;
    }
    return nullptr; // Некорректный индекс
}

const std::vector<std::unique_ptr<Spell>>& SpellHand::GetSpells() const {
    return spells_;
}

size_t SpellHand::GetMaxSize() const {
    return max_size_;
}

} // namespace game
