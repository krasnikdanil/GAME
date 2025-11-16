#ifndef RENDERER_H_
#define RENDERER_H_

#include "utils.h"
#include "Player.h"
#include "Enemy.h"
#include "Building.h"
#include "Spell.h" // Включаем Spell.h
#include <vector>

namespace game {


// Предварительные объявления
class Field;

class Renderer {
public:
  Renderer(int rows, int cols);
  
  void Render(const std::vector<std::vector<Cell>>& grid,
              const Player& player,
              const std::pair<int, int>& player_pos,
              const std::vector<Enemy>& enemies,
              const std::vector<std::pair<int, int>>& enemy_positions,
              [[maybe_unused]] const std::vector<Building>& buildings,
              const std::vector<std::pair<int, int>>& building_positions,
              int turn_counter) const;
              
  void ShowGameOver(int score) const;
  void ShowHelp() const;
  void RenderSpells(const std::vector<std::unique_ptr<Spell>>& spells) const; // Добавляем метод для отрисовки заклинаний

private:
  int rows_;
  int cols_;
};

} // namespace game

#endif // RENDERER_H_
