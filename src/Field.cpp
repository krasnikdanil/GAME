#include "Field.h"
#include "GameLogic.h" // Добавляем GameLogic.h
#include "utils.h"
#include <ncurses.h>
#include <random>
#include <utility>
namespace game {

Field::Field(int rows, int cols, int wall_chance_percent,
             int slow_chance_percent)
    : rows_(rows), cols_(cols), grid_(rows, std::vector<Cell>(cols)),
      player_{}, // Используем делегирующий конструктор Player()
      player_pos_{rows / 2, cols / 2}, player_stunned_(false),
      turn_counter_(0), enemies_(), enemy_positions_(), buildings_(), building_positions_(),
      input_handler_(), renderer_(rows, cols), game_logic_(rows, cols) { // Инициализируем InputHandler, Renderer и GameLogic
  if (rows < 10)
    rows_ = 10;
  if (cols < 10)
    cols_ = 10;
  if (rows > 25)
    rows_ = 25;
  if (cols > 25)
    cols_ = 25;

  grid_.assign(rows_, std::vector<Cell>(cols_, Cell()));

  // Заполнение стен и замедляющих клеток
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist100(1, 100);

  for (int r = 0; r < rows_; ++r) {
    for (int c = 0; c < cols_; ++c) {
      int pr = player_pos_.first;
      int pc = player_pos_.second;
      if (std::abs(pr - r) + std::abs(pc - c) <= 1)
        continue; // Оставляем стартовую зону чистой

      int roll = dist100(gen);
      if (roll <= wall_chance_percent) {
        grid_[r][c].type = CellType::kWall;
      } else if (roll <= wall_chance_percent + slow_chance_percent) {
        grid_[r][c].type = CellType::kSlow;
      }
    }
  }

  // Края — стены
  for (int r = 0; r < rows_; ++r) {
    grid_[r][0].type = CellType::kWall;
    grid_[r][cols_ - 1].type = CellType::kWall;
  }
  for (int c = 0; c < cols_; ++c) {
    grid_[0][c].type = CellType::kWall;
    grid_[rows_ - 1][c].type = CellType::kWall;
  }

  // Пример: добавляем врага и здание
  AddEnemyNear({player_pos_.first, player_pos_.second + 3}, Enemy(6, 2));
  AddBuildingNear({player_pos_.first - 3, player_pos_.second}, Building(4));

  // Добавляем одно случайное заклинание игроку при инициализации поля
  player_.AddSpell(GameLogic::CreateRandomSpell());
}

Field::Field(const Field &other)
    : rows_(other.rows_), cols_(other.cols_), grid_(other.grid_),
      player_(other.player_), player_pos_(other.player_pos_),
      player_stunned_(other.player_stunned_),
      turn_counter_(other.turn_counter_),
      enemies_(other.enemies_), enemy_positions_(other.enemy_positions_),
      buildings_(other.buildings_), building_positions_(other.building_positions_),
      input_handler_(other.input_handler_),
      renderer_(other.rows_, other.cols_), game_logic_(other.rows_, other.cols_) {}

Field::Field(Field &&other) noexcept
    : rows_(other.rows_), cols_(other.cols_), grid_(std::move(other.grid_)),
      player_(std::move(other.player_)), player_pos_(other.player_pos_),
      player_stunned_(other.player_stunned_),
      turn_counter_(other.turn_counter_),
      enemies_(std::move(other.enemies_)), enemy_positions_(std::move(other.enemy_positions_)),
      buildings_(std::move(other.buildings_)), building_positions_(std::move(other.building_positions_)),
      input_handler_(std::move(other.input_handler_)),
      renderer_(std::move(other.renderer_)), game_logic_(std::move(other.game_logic_)) {
  other.rows_ = 0;
  other.cols_ = 0;
  other.player_stunned_ = false;
  other.turn_counter_ = 0;
}

Field &Field::operator=(const Field &other) {
  if (this == &other)
    return *this; // Защита от self-assignment

  rows_ = other.rows_;
  cols_ = other.cols_;
  grid_ = other.grid_;
  player_ = other.player_;
  player_pos_ = other.player_pos_;
  player_stunned_ = other.player_stunned_;
  turn_counter_ = other.turn_counter_;
  enemies_ = other.enemies_;
  enemy_positions_ = other.enemy_positions_;
  buildings_ = other.buildings_;
  building_positions_ = other.building_positions_;
  input_handler_ = other.input_handler_;
  renderer_ = other.renderer_; // Присваивание renderer_ в операторе присваивания копированием
  game_logic_ = other.game_logic_; // Присваивание game_logic_ в операторе присваивания копированием

  return *this;
}

Field &Field::operator=(Field &&other) noexcept {
  if (this == &other)
    return *this; // Защита от self-assignment

  rows_ = other.rows_;
  cols_ = other.cols_;
  grid_ = std::move(other.grid_);
  player_ = std::move(other.player_);
  player_pos_ = other.player_pos_;
  player_stunned_ = other.player_stunned_;
  turn_counter_ = other.turn_counter_;
  enemies_ = std::move(other.enemies_);
  enemy_positions_ = std::move(other.enemy_positions_);
  buildings_ = std::move(other.buildings_);
  building_positions_ = std::move(other.building_positions_);
  input_handler_ = std::move(other.input_handler_);
  renderer_ = std::move(other.renderer_); // Присваивание renderer_ в операторе присваивания перемещением
  game_logic_ = std::move(other.game_logic_); // Присваивание game_logic_ в операторе присваивания перемещением

  other.rows_ = 0;
  other.cols_ = 0;
  other.player_stunned_ = false;
  other.turn_counter_ = 0;

  return *this;
}

int Field::GetRows() const { return rows_; }

int Field::GetCols() const { return cols_; }

std::pair<int, int> Field::GetPlayerPosition() const { return player_pos_; }

bool Field::IsPlayerStunned() const { return player_stunned_; }

bool Field::IsInside(int r, int c) const {
  return r >= 0 && r < rows_ && c >= 0 && c < cols_;
}

bool Field::IsBlocked(int r, int c) const {
  if (!IsInside(r, c))
    return true;
  return grid_[r][c].type == CellType::kWall;
}

bool Field::IsSlow(int r, int c) const {
  if (!IsInside(r, c))
    return false;
  return grid_[r][c].type == CellType::kSlow;
}

bool Field::IsOccupiedByEnemy(int r, int c) const {
  for (auto &p : enemy_positions_) {
    if (p.first == r && p.second == c)
      return true;
  }
  return false;
}

bool Field::IsOccupiedByBuilding(int r, int c) const {
  for (auto &p : building_positions_) {
    if (p.first == r && p.second == c)
      return true;
  }
  return false;
}

bool Field::FindAdjacentFree(const std::pair<int, int> &center,
                             std::pair<int, int> &out) {
  static const int dr[4] = {-1, 1, 0, 0};
  static const int dc[4] = {0, 0, -1, 1};

  for (int i = 0; i < 4; i++) {
    int nr = center.first + dr[i];
    int nc = center.second + dc[i];

    if (!IsInside(nr, nc))
      continue;
    if (IsBlocked(nr, nc))
      continue;
    if (nr == player_pos_.first && nc == player_pos_.second)
      continue;
    if (IsOccupiedByEnemy(nr, nc))
      continue;
    if (IsOccupiedByBuilding(nr, nc))
      continue;

    out = {nr, nc};
    return true;
  }
  return false;
}

void Field::AddEnemyNear(const std::pair<int, int> &pos, const Enemy &e) {
  std::pair<int, int> free_cell;
  if (FindAdjacentFree(pos, free_cell)) {
    enemies_.push_back(e);
    enemy_positions_.push_back(free_cell);
  } else {
    int r = pos.first, c = pos.second;
    if (IsInside(r, c) && !IsBlocked(r, c) &&
        !(r == player_pos_.first && c == player_pos_.second) &&
        !IsOccupiedByEnemy(r, c) && !IsOccupiedByBuilding(r, c)) {
      enemies_.push_back(e);
      enemy_positions_.push_back({r, c});
    }
  }
}

void Field::AddBuildingNear(const std::pair<int, int> &pos, const Building &b) {
  std::pair<int, int> free_cell;
  if (FindAdjacentFree(pos, free_cell)) {
    buildings_.push_back(b);
    building_positions_.push_back(free_cell);
  }
}

void Field::Render() const {
  renderer_.Render(grid_, player_, player_pos_, enemies_, enemy_positions_,
                   buildings_, building_positions_, turn_counter_);
}

char Field::GetInput() const { return input_handler_.GetInput(); }

void Field::PrintHelp() const { renderer_.ShowHelp(); }

void Field::PlayerTurn() {
  char cmd = input_handler_.GetInput();

  if (cmd == 'q') {
    endwin();
    exit(0);
  }

  if (cmd == 'f') {
    player_.ToggleMode();
    mvprintw(rows_ + 2, 0, "Mode switched to %s. This consumed your turn.",
             (player_.GetMode() == AttackMode::kMelee ? "Melee" : "Ranged"));
    refresh();
    napms(1000); // Заменено на napms
    return;
  }

  if (cmd == 'r' && player_.GetMode() == AttackMode::kRanged) {
    HandleRangedAttack();
    return;
  }

  if (cmd == 'e') { // Use spell
    HandleSpellCast();
    return;
  }

  int dr = 0, dc = 0;
  if (cmd == 'w')
    dr = -1;
  else if (cmd == 's')
    dr = 1;
  else if (cmd == 'a')
    dc = -1;
  else if (cmd == 'd')
    dc = 1;
  else {
    mvprintw(rows_ + 2, 0, "Unknown command: %d", cmd); // Добавляем отладочный вывод
    refresh();
    napms(500); // Заменено на napms
    return;
  }

  int nr = player_pos_.first + dr;
  int nc = player_pos_.second + dc;

  if (!IsInside(nr, nc)) {
    mvprintw(rows_ + 2, 0, "Cannot move there (out of bounds).");
    refresh();
    napms(500); // Заменено на napms
    return;
  }

  if (IsBlocked(nr, nc)) {
    mvprintw(rows_ + 2, 0, "Movement blocked (wall).");
    refresh();
    napms(500); // Заменено на napms
    return;
  }

  // Проверка на врага
  for (size_t i = 0; i < enemy_positions_.size(); ++i) {
    if (enemy_positions_[i].first == nr && enemy_positions_[i].second == nc &&
        enemies_[i].IsAlive()) {
      int dmg = player_.GetCurrentDamage();
      enemies_[i].TakeDamage(dmg);
      mvprintw(rows_ + 2, 0, "You attack enemy for %d damage. Enemy HP now %d",
               dmg, enemies_[i].GetHealth());
      if (!enemies_[i].IsAlive()) {
        mvprintw(rows_ + 3, 0, "Enemy dies. +10 score.");
        player_.AddScore(10);
        game_logic_.GainRandomSpell(player_); // Игрок получает случайное заклинание
      }
      refresh();
      napms(1000); // Заменено на napms
      return;      // Не двигаемся на клетку врага
    }
  }

  // Проверка на здание
  for (size_t i = 0; i < building_positions_.size(); ++i) {
    if (building_positions_[i].first == nr &&
        building_positions_[i].second == nc) {
      mvprintw(rows_ + 2, 0, "This cell has a building - you cannot enter it.");
      refresh();
      napms(500); // Заменено на napms
      return;
    }
  }

  // Двигаем игрока
  player_pos_ = {nr, nc};

  if (IsSlow(nr, nc)) {
    mvprintw(rows_ + 2, 0,
             "You stepped on a slowing cell: you will miss your next turn.");
    player_stunned_ = true;
    refresh();
    napms(1000); // Заменено на napms
  }
}

void Field::HandleRangedAttack() {
  mvprintw(rows_ + 2, 0, "Enter target position (row col): ");
  refresh();

  int target_row, target_col;
  scanw(const_cast<char *>("%d %d"), &target_row, &target_col);
  Position target_pos{target_row, target_col};

  // Проверяем, по прямой ли выстрел
  int dx = target_pos.first - player_pos_.first;
  int dy = target_pos.second - player_pos_.second;

  if (!((dx != 0 && dy == 0) || (dx == 0 && dy != 0))) {
    mvprintw(rows_ + 3, 0, "Invalid direction! Only straight lines allowed.");
    refresh();
    napms(1000);
    return;
  }

  // Анимация выстрела (только в utils.cpp, если там используется napms)
  AnimateArrow(player_pos_, target_pos);

  // Проверяем, есть ли враг на цели
  for (size_t i = 0; i < enemy_positions_.size(); ++i) {
    if (enemy_positions_[i] == target_pos && enemies_[i].IsAlive()) {
      int dmg = player_.GetCurrentDamage();
      enemies_[i].TakeDamage(dmg);
      mvprintw(rows_ + 3, 0, "You shoot enemy for %d damage. Enemy HP now %d",
               dmg, enemies_[i].GetHealth());
      if (!enemies_[i].IsAlive()) {
        mvprintw(rows_ + 4, 0, "Enemy dies. +10 score.");
        player_.AddScore(10);
      }
      refresh();
      napms(1000); // Заменено на napms
      return;
    }
  }

  mvprintw(rows_ + 3, 0, "Missed!");
  refresh();
  napms(500); // Заменено на napms
}

void Field::ProcessEnemies() {
  for (size_t i = 0; i < enemies_.size(); ++i) {
    if (!enemies_[i].IsAlive())
      continue;

    auto pos = enemy_positions_[i];
    int r = pos.first, c = pos.second;
    int pr = player_pos_.first, pc = player_pos_.second;

    int dr = 0, dc = 0;
    if (std::abs(pr - r) > std::abs(pc - c)) {
      dr = (pr > r) ? 1 : -1;
    } else if (pc != c) {
      dc = (pc > c) ? 1 : -1;
    } else if (pr != r) {
      dr = (pr > r) ? 1 : -1;
    }

    int nr = r + dr;
    int nc = c + dc;

    if (nr == pr && nc == pc) {
      int dmg = enemies_[i].GetDamage();
      player_.TakeDamage(dmg);
      mvprintw(rows_ + 2, 0, "Enemy at (%d,%d) attacks you for %d. Your HP: %d",
               r, c, dmg, player_.GetHealth());
      refresh();
      napms(1000); // Заменено на napms
      continue;
    }

    if (!IsInside(nr, nc))
      continue;
    if (IsBlocked(nr, nc))
      continue;
    if (IsOccupiedByEnemy(nr, nc))
      continue;
    if (IsOccupiedByBuilding(nr, nc))
      continue;

    enemy_positions_[i] = {nr, nc};
  }
}

void Field::ProcessBuildings() {
  for (size_t i = 0; i < buildings_.size(); ++i) {
    if (buildings_[i].AdvanceTurnAndCheckSpawn()) {
      std::pair<int, int> spawn_cell;
      if (FindAdjacentFree(building_positions_[i], spawn_cell)) {
        enemies_.emplace_back(Enemy(4, 1));
        enemy_positions_.emplace_back(spawn_cell);
        mvprintw(rows_ + 2, 0,
                 "A building at (%d,%d) spawned an enemy at (%d,%d)",
                 building_positions_[i].first, building_positions_[i].second,
                 spawn_cell.first, spawn_cell.second);
        refresh();
        napms(1000); // Заменено на napms
      }
    }
  }
}

void Field::RunGameLoop() {
  initscr();            // Инициализация ncurses
  cbreak();             // Отключаем буферизацию ввода
  noecho();             // Не показываем введённые символы
  keypad(stdscr, TRUE); // Включаем обработку функциональных клавиш
  curs_set(0);          // Скрываем курсор
  PrintHelp();

  renderer_.ShowHelp();
  getch();

  while (player_.IsAlive()) {
    Render();

    if (player_stunned_) {
      mvprintw(rows_ + 2, 0, "You are slowed and lose this turn.");
      refresh();
      napms(1000); // Заменено на napms
      player_stunned_ = false;
    } else {
      PlayerTurn();
    }

    ProcessBuildings();
    ProcessEnemies();

    turn_counter_++;

    if (!player_.IsAlive())
      break;
  }

  renderer_.ShowGameOver(player_.GetScore());

  renderer_.ShowGameOver(player_.GetScore());
  while (GetInput() != 'q') {
  }
  endwin();
}

void Field::HandleSpellCast() {
  if (player_.GetSpells().empty()) {
    mvprintw(rows_ + 2, 0, "No spells available.");
    refresh();
    napms(1000);
    return;
  }

  renderer_.RenderSpells(player_.GetSpells());
  refresh();

  int spell_choice = input_handler_.GetSpellChoice();
  if (spell_choice != -1 && static_cast<size_t>(spell_choice) < player_.GetSpells().size()) {
    // Получаем заклинание для применения эффекта перед его использованием (удалением из руки)
    const std::unique_ptr<Spell>& chosen_spell = player_.GetSpells()[spell_choice];
    // Сохраняем имя заклинания ДО удаления, так как после UseSpell ссылка станет невалидной
    std::string spell_name = chosen_spell->GetName();
    mvprintw(rows_ + 2, 0, "Enter target position (row col): ");
    refresh();
    std::pair<int, int> target_pos = input_handler_.GetTargetPosition();

    game_logic_.ProcessSpellCast(player_, enemies_, enemy_positions_, buildings_, building_positions_, spell_choice, target_pos);
    player_.UseSpell(spell_choice); // Удаляем заклинание из руки после использования
    mvprintw(rows_ + 2, 0, "You used %s. This consumed your turn.", spell_name.c_str());
  } else {
    mvprintw(rows_ + 2, 0, "Invalid spell choice.");
  }
  refresh();
  napms(1000);
}

} // namespace game
