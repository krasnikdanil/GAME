#ifndef ENEMY_H_
#define ENEMY_H_

namespace game {

// Представляет врага в игре.
// Управляет здоровьем и уроном.
// Позиция хранится в Field.
class Enemy {
public:
  // Конструктор: создаёт врага с заданными параметрами.
  explicit Enemy(int hp = 5, int damage = 1);

  // Получить урон
  void TakeDamage(int damage);

  // Проверка жизни
  bool IsAlive() const;

  // Геттеры
  int GetDamage() const;
  int GetHealth() const;

private:
  int health_;
  int damage_;
};

} // namespace game

#endif // ENEMY_H_
