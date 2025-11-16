#ifndef FIELD_H_
#define FIELD_H_

#include "Building.h"
#include "Enemy.h"
#include "InputHandler.h" // Добавляем InputHandler
#include "GameLogic.h" // Добавляем GameLogic
#include "Player.h"
#include "Renderer.h" // Добавляем Renderer
#include "utils.h"
#include <utility>
#include <vector>

namespace game {


class Field {
public:
  // Конструктор: ширина и высота (10..25)
  Field(int rows, int cols, int wall_chance_percent = 10,
        int slow_chance_percent = 5);

  // Копирующий конструктор (глубокая копия)
  Field(const Field &other);

  // Перемещающий конструктор
  Field(Field &&other) noexcept;

  // Оператор присваивания (копирование)
  Field &operator=(const Field &other);

  // Оператор присваивания (перемещение)
  Field &operator=(Field &&other) noexcept;

  // Деструктор
  ~Field() = default;

  // Геттеры
  int GetRows() const;
  int GetCols() const;
  std::pair<int, int> GetPlayerPosition() const;
  bool IsPlayerStunned() const;

  // Проверка, можно ли переместиться на позицию
  bool IsBlocked(int r, int c) const;

  // Проверка, является ли клетка замедляющей
  bool IsSlow(int r, int c) const;

  // Добавить врага рядом с позицией
  void AddEnemyNear(const std::pair<int, int> &pos, const Enemy &e);

  // Добавить здание рядом с позицией
  void AddBuildingNear(const std::pair<int, int> &pos, const Building &b);

  // Публичный API для запуска игрового цикла
  void RunGameLoop();

private:
  int rows_;
  int cols_;
  std::vector<std::vector<Cell>> grid_;
  Player player_;
  std::pair<int, int> player_pos_;
  bool player_stunned_;
  int turn_counter_;

  std::vector<Enemy> enemies_;
  std::vector<std::pair<int, int>> enemy_positions_;

  std::vector<Building> buildings_;
  std::vector<std::pair<int, int>> building_positions_;

  InputHandler input_handler_; // Добавляем обработчик ввода как член класса
  Renderer renderer_;         // Добавляем рендерер как член класса
  GameLogic game_logic_;      // Добавляем GameLogic как член класса

  // Вспомогательные функции
  bool IsInside(int r, int c) const;
  bool IsOccupiedByEnemy(int r, int c) const;
  bool IsOccupiedByBuilding(int r, int c) const;
  bool FindAdjacentFree(const std::pair<int, int> &center,
                        std::pair<int, int> &out);

  // Отрисовка поля
  void Render() const;

  // Обработка ввода
  char GetInput() const;

  // Печать помощи
  void PrintHelp() const;

  // Обработка хода игрока
  void PlayerTurn();

  // Обработка использования заклинания
  void HandleSpellCast();

  // Обработка выстрела
  void HandleRangedAttack();

  // Обработка врагов
  void ProcessEnemies();

  // Обработка зданий
  void ProcessBuildings();

  // Определение swap как friend-функции внутри класса
  friend void swap(Field &a, Field &b) noexcept {
    using std::swap; // Обязательно для std::swap
    swap(a.rows_, b.rows_);
    swap(a.cols_, b.cols_);
    swap(a.grid_, b.grid_);
    swap(a.player_, b.player_);
    swap(a.player_pos_, b.player_pos_);
    swap(a.player_stunned_, b.player_stunned_);
    swap(a.turn_counter_, b.turn_counter_);
    swap(a.enemies_, b.enemies_);
    swap(a.enemy_positions_, b.enemy_positions_);
    swap(a.buildings_, b.buildings_);
    swap(a.building_positions_, b.building_positions_);
    swap(a.game_logic_, b.game_logic_);
  }
};

} // namespace game

#endif // FIELD_H_
