#include "Renderer.h"
#include <ncurses.h>

namespace game {

Renderer::Renderer(int rows, int cols) : rows_(rows), cols_(cols) {}

void Renderer::Render(const std::vector<std::vector<Cell>>& grid,
                      const Player& player,
                      const std::pair<int, int>& player_pos,
                      const std::vector<Enemy>& enemies,
                      const std::vector<std::pair<int, int>>& enemy_positions,
                      const std::vector<Building>& buildings,
                      const std::vector<std::pair<int, int>>& building_positions,
                      int turn_counter) const {
  clear(); // Очищаем экран

  // Отрисовка поля
  for (int r = 0; r < rows_; ++r) {
    for (int c = 0; c < cols_; ++c) {
      if (grid[r][c].type == CellType::kWall) {
        mvaddch(r, c, '#');
      } else if (grid[r][c].type == CellType::kSlow) {
        mvaddch(r, c, 's');
      } else {
        mvaddch(r, c, '.');
      }
    }
  }

  // Здания
  for (size_t i = 0; i < building_positions.size(); ++i) {
    auto p = building_positions[i];
    mvaddch(p.first, p.second, 'B');
  }

  // Враги
  for (size_t i = 0; i < enemy_positions.size(); ++i) {
    if (enemies[i].IsAlive()) {
      auto p = enemy_positions[i];
      mvaddch(p.first, p.second, 'E');
    }
  }

  // Игрок
  mvaddch(player_pos.first, player_pos.second, 'P');

  // Статистика
  mvprintw(
      rows_ + 1, 0,
      "Turn %d | Player HP: %d | Score: %d | Mode: %s | rows: %d | cols: %d",
      turn_counter, player.GetHealth(), player.GetScore(),
      (player.GetMode() == AttackMode::kMelee ? "Melee" : "Ranged"),
      player_pos.first, player_pos.second);

  refresh(); // Обновляем экран
}

void Renderer::ShowGameOver(int score) const {
  mvprintw(0, 0, "=== GAME OVER ===");
  mvprintw(1, 0, "You lost!");
  mvprintw(3, 0, "Final score: %d", score);
  mvprintw(4, 0, "Press Q to exit...");
  refresh();
}

void Renderer::ShowHelp() const {
  mvprintw(0, 0, "=== Welcome to the Game ===");
  mvprintw(1, 0,
           "Use W/A/S/D to move, F to toggle mode, R to shoot, E to use spell, Q to quit.");
  mvprintw(2, 0, "Press any key to start...");
  refresh();
  getch();
}

void Renderer::RenderSpells(const std::vector<std::unique_ptr<Spell>>& spells) const {
    if (spells.empty()) {
        mvprintw(rows_ + 4, 0, "No spells available.");
        return;
    }
    mvprintw(rows_ + 4, 0, "Spells:");
    for (size_t i = 0; i < spells.size(); ++i) {
        mvprintw(rows_ + 5 + i, 0, "%lu: %s", i + 1, spells[i]->GetName().c_str());
    }
}

} // namespace game
