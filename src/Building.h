#ifndef BUILDING_H_
#define BUILDING_H_

namespace game {

// Представляет здание на поле.
// Может спавнить врагов.
class Building {
public:
  // Конструктор: создаёт здание с заданным интервалом спавна.
  explicit Building(int spawn_interval = 5);

  // Продвинуть счётчик хода и проверить, пора ли спавнить
  bool AdvanceTurnAndCheckSpawn();

  // Геттер
  int GetSpawnInterval() const;
  int GetHealth() const;
  void TakeDamage(int damage);
  bool IsAlive() const;

private:
  int spawn_interval_;
  int counter_;
  int health_; // Добавляем здоровье для зданий
};

} // namespace game

#endif // BUILDING_H_
