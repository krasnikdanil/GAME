#include "Player.h"
#include "Spell.h"

namespace game {

// Конструктор по умолчанию — делегирует основной
Player::Player() : Player(10, 4, 2) {}

// Основной конструктор
Player::Player(int hp, int melee, int ranged)
    : health_(hp), melee_damage_(melee), ranged_damage_(ranged), score_(0),
      mode_(AttackMode::kMelee), spell_hand_(5) {} // Инициализируем SpellHand с размером 5

void Player::TakeDamage(int damage) {
  health_ -= damage;
  if (health_ < 0) {
    health_ = 0;
  }
}

bool Player::IsAlive() const { return health_ > 0; }

int Player::GetCurrentDamage() const {
  return (mode_ == AttackMode::kMelee) ? melee_damage_ : ranged_damage_;
}

void Player::ToggleMode() {
  mode_ =
      (mode_ == AttackMode::kMelee) ? AttackMode::kRanged : AttackMode::kMelee;
}

AttackMode Player::GetMode() const { return mode_; }

void Player::AddScore(int points) { score_ += points; }

int Player::GetHealth() const { return health_; }

int Player::GetScore() const { return score_; }

const std::vector<std::unique_ptr<Spell>>& Player::GetSpells() const {
    return spell_hand_.GetSpells();
}

void Player::AddSpell(std::unique_ptr<Spell> spell) {
    spell_hand_.AddSpell(std::move(spell));
}

void Player::UseSpell(int spellIndex) {
    spell_hand_.UseSpell(spellIndex);
}

size_t Player::GetSpellHandSize() const {
    return spell_hand_.GetSpells().size();
}

size_t Player::GetSpellHandMaxSize() const {
    return spell_hand_.GetMaxSize();
}

} // namespace game
