#ifndef PLAYER_H_
#define PLAYER_H_

#include "utils.h"
#include "Spell.h"
#include "SpellHand.h" // Добавляем SpellHand

namespace game {
// Представляет игрока в игре.
// Управляет здоровьем, уроном, очками и режимом боя.
class Player {
public:
  Player();
  Player(int hp, int melee, int ranged);

  // Получить урон
  void TakeDamage(int damage);

  // Проверка жизни
  bool IsAlive() const;

  // Получить текущий урон в зависимости от режима
  int GetCurrentDamage() const;

  // Переключить режим боя (ближний/дальний)
  void ToggleMode();

  // Получить текущий режим боя
  AttackMode GetMode() const;

  // Добавить очки
  void AddScore(int points);

  // Геттеры
  int GetHealth() const;
  int GetScore() const;
  const std::vector<std::unique_ptr<Spell>>& GetSpells() const; // Возвращаем ссылку на вектор unique_ptr
  void AddSpell(std::unique_ptr<Spell> spell); // Принимаем unique_ptr
  void UseSpell(int spellIndex);
  size_t GetSpellHandSize() const; // Добавляем геттер для текущего размера руки
  size_t GetSpellHandMaxSize() const; // Добавляем геттер для максимального размера руки

private:
  int health_;
  int melee_damage_;
  int ranged_damage_;
  int score_;
  AttackMode mode_;
  SpellHand spell_hand_; // Заменяем вектор заклинаний на SpellHand
};

} // namespace game

#endif // PLAYER_H_
