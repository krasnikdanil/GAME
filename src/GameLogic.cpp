#include "GameLogic.h"
#include "Spell.h" // Добавляем Spell.h
#include "DamageSpell.h" // Добавляем DamageSpell.h
#include "HealSpell.h" // Добавляем HealSpell.h
#include "BuffSpell.h" // Добавляем BuffSpell.h
#include "DebuffSpell.h" // Добавляем DebuffSpell.h
#include "AoEDamageSpell.h" // Добавляем AoEDamageSpell.h
#include <ncurses.h>
#include <random>
#include <ctime> // Для инициализации генератора случайных чисел
#include <memory> // Для std::unique_ptr

namespace game {

GameLogic::GameLogic(int rows, int cols) : rows_(rows), cols_(cols) {}

bool GameLogic::IsInside(int r, int c) const {
    return r >= 0 && r < rows_ && c >= 0 && c < cols_;
}

bool GameLogic::IsBlocked(int r, int c, const std::vector<std::vector<Cell>>& grid) const {
    if (!IsInside(r, c))
        return true;
    return grid[r][c].type == CellType::kWall;
}

bool GameLogic::IsSlow(int r, int c, const std::vector<std::vector<Cell>>& grid) const {
    if (!IsInside(r, c))
        return false;
    return grid[r][c].type == CellType::kSlow;
}

bool GameLogic::IsOccupiedByEnemy(int r, int c, 
                                  const std::vector<std::pair<int, int>>& enemy_positions) const {
    for (auto &p : enemy_positions) {
        if (p.first == r && p.second == c)
            return true;
    }
    return false;
}

bool GameLogic::IsOccupiedByBuilding(int r, int c, 
                                     const std::vector<std::pair<int, int>>& building_positions) const {
    for (auto &p : building_positions) {
        if (p.first == r && p.second == c)
            return true;
    }
    return false;
}

bool GameLogic::FindAdjacentFree(const std::pair<int, int> &center,
                                 std::pair<int, int> &out,
                                 const std::vector<std::vector<Cell>>& grid,
                                 const std::pair<int, int>& player_pos,
                                 const std::vector<std::pair<int, int>>& enemy_positions,
                                 const std::vector<std::pair<int, int>>& building_positions) const {
    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int nr = center.first + dr[i];
        int nc = center.second + dc[i];

        if (!IsInside(nr, nc))
            continue;
        if (IsBlocked(nr, nc, grid))
            continue;
        if (nr == player_pos.first && nc == player_pos.second)
            continue;
        if (IsOccupiedByEnemy(nr, nc, enemy_positions))
            continue;
        if (IsOccupiedByBuilding(nr, nc, building_positions))
            continue;

        out = {nr, nc};
        return true;
    }
    return false;
}

void GameLogic::ProcessPlayerTurn(Player& player, 
                                  std::pair<int, int>& player_pos,
                                  bool& player_stunned,
                                  const std::vector<std::vector<Cell>>& grid,
                                  std::vector<Enemy>& enemies,
                                  std::vector<std::pair<int, int>>& enemy_positions,
                                  [[maybe_unused]] std::vector<Building>& buildings,
                                  std::vector<std::pair<int, int>>& building_positions,
                                  char cmd) {
    if (cmd == 'f') {
        player.ToggleMode();
        mvprintw(rows_ + 2, 0, "Mode switched to %s. This consumed your turn.",
                 (player.GetMode() == AttackMode::kMelee ? "Melee" : "Ranged"));
        refresh();
        napms(1000);
        return;
    }

    if (cmd == 'r' && player.GetMode() == AttackMode::kRanged) {
        // Обработка выстрела будет в отдельном методе
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
        mvprintw(rows_ + 2, 0, "Unknown command.");
        refresh();
        napms(500);
        return;
    }

    int nr = player_pos.first + dr;
    int nc = player_pos.second + dc;

    if (!IsInside(nr, nc)) {
        mvprintw(rows_ + 2, 0, "Cannot move there (out of bounds).");
        refresh();
        napms(500);
        return;
    }

    if (IsBlocked(nr, nc, grid)) {
        mvprintw(rows_ + 2, 0, "Movement blocked (wall).");
        refresh();
        napms(500);
        return;
    }

    // Проверка на врага
    for (size_t i = 0; i < enemy_positions.size(); ++i) {
        if (enemy_positions[i].first == nr && enemy_positions[i].second == nc &&
            enemies[i].IsAlive()) {
            int dmg = player.GetCurrentDamage();
            enemies[i].TakeDamage(dmg);
            mvprintw(rows_ + 2, 0, "You attack enemy for %d damage. Enemy HP now %d",
                     dmg, enemies[i].GetHealth());
            if (!enemies[i].IsAlive()) {
                mvprintw(rows_ + 3, 0, "Enemy dies. +10 score.");
                player.AddScore(10);
            }
            refresh();
            napms(100);
            return;      // Не двигаемся на клетку врага
        }
    }

    // Проверка на здание
    for (size_t i = 0; i < building_positions.size(); ++i) {
        if (building_positions[i].first == nr &&
            building_positions[i].second == nc) {
            mvprintw(rows_ + 2, 0, "This cell has a building - you cannot enter it.");
            refresh();
            napms(500);
            return;
        }
    }

    // Двигаем игрока
    player_pos = {nr, nc};

    if (IsSlow(nr, nc, grid)) {
        mvprintw(rows_ + 2, 0,
                 "You stepped on a slowing cell: you will miss your next turn.");
        player_stunned = true;
        refresh();
        napms(1000);
    }
}

void GameLogic::ProcessEnemies(Player& player,
                               const std::pair<int, int>& player_pos,
                               const std::vector<std::vector<Cell>>& grid,
                               std::vector<Enemy>& enemies,
                               std::vector<std::pair<int, int>>& enemy_positions,
                               const std::vector<std::pair<int, int>>& building_positions) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (!enemies[i].IsAlive())
            continue;

        auto pos = enemy_positions[i];
        int r = pos.first, c = pos.second;
        int pr = player_pos.first, pc = player_pos.second;

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
            int dmg = enemies[i].GetDamage();
            player.TakeDamage(dmg);
            mvprintw(rows_ + 2, 0, "Enemy at (%d,%d) attacks you for %d. Your HP: %d",
                     r, c, dmg, player.GetHealth());
            refresh();
            napms(1000);
            continue;
        }

        if (!IsInside(nr, nc))
            continue;
        if (IsBlocked(nr, nc, grid))
            continue;
        if (IsOccupiedByEnemy(nr, nc, enemy_positions))
            continue;
        if (IsOccupiedByBuilding(nr, nc, building_positions))
            continue;

        enemy_positions[i] = {nr, nc};
    }
}

void GameLogic::ProcessBuildings(std::vector<Building>& buildings,
                                 const std::vector<std::pair<int, int>>& building_positions,
                                 std::vector<Enemy>& enemies,
                                 std::vector<std::pair<int, int>>& enemy_positions,
                                 const std::vector<std::vector<Cell>>& grid,
                                 const std::pair<int, int>& player_pos) {
    for (size_t i = 0; i < buildings.size(); ++i) {
        if (buildings[i].AdvanceTurnAndCheckSpawn()) {
            std::pair<int, int> spawn_cell;
            if (FindAdjacentFree(building_positions[i], spawn_cell, grid, player_pos, 
                                enemy_positions, building_positions)) {
                enemies.emplace_back(Enemy(4, 1));
                enemy_positions.emplace_back(spawn_cell);
                mvprintw(rows_ + 2, 0,
                         "A building at (%d,%d) spawned an enemy at (%d,%d)",
                         building_positions[i].first, building_positions[i].second,
                         spawn_cell.first, spawn_cell.second);
                refresh();
                napms(1000);
            }
        }
    }
}

void GameLogic::ProcessSpellCast(Player& player,
                                 std::vector<Enemy>& enemies,
                                 std::vector<std::pair<int, int>>& enemy_positions,
                                 std::vector<Building>& buildings,
                                 std::vector<std::pair<int, int>>& building_positions,
                                 int spell_index,
                                 const std::pair<int, int>& target_pos) {
    if (spell_index >= 0 && static_cast<size_t>(spell_index) < player.GetSpells().size()) {
        const std::unique_ptr<Spell>& spell = player.GetSpells()[spell_index];

        if (spell->GetType() == SpellType::Damage) {
            bool target_found = false;
            // Проверяем, есть ли враг на целевой позиции
            for (size_t i = 0; i < enemy_positions.size(); ++i) {
                if (enemy_positions[i] == target_pos && enemies[i].IsAlive()) {
                    enemies[i].TakeDamage(spell->GetEffectValue());
                    mvprintw(rows_ + 4, 0, "Spell '%s' hit enemy at (%d,%d) for %d damage. Enemy HP now %d",
                             spell->GetName().c_str(), target_pos.first, target_pos.second, spell->GetEffectValue(), enemies[i].GetHealth());
                    if (!enemies[i].IsAlive()) {
                        mvprintw(rows_ + 5, 0, "Enemy dies. +10 score.");
                        player.AddScore(10);
                        GainRandomSpell(player); // Игрок получает случайное заклинание
                    }
                    target_found = true;
                    break;
                }
            }
            // Проверяем, есть ли здание на целевой позиции
            if (!target_found) {
                for (size_t i = 0; i < building_positions.size(); ++i) {
                    if (building_positions[i] == target_pos && buildings[i].IsAlive()) {
                        buildings[i].TakeDamage(spell->GetEffectValue());
                        mvprintw(rows_ + 4, 0, "Spell '%s' hit building at (%d,%d) for %d damage. Building HP now %d",
                                 spell->GetName().c_str(), target_pos.first, target_pos.second, spell->GetEffectValue(), buildings[i].GetHealth());
                        if (!buildings[i].IsAlive()) {
                            mvprintw(rows_ + 5, 0, "Building destroyed. +5 score.");
                            player.AddScore(5);
                            GainRandomSpell(player); // Игрок получает случайное заклинание
                        }
                        target_found = true;
                        break;
                    }
                }
            }

            if (!target_found) {
                mvprintw(rows_ + 4, 0, "Spell '%s' missed. No enemy or building at target position.", spell->GetName().c_str());
            }

        } else if (spell->GetType() == SpellType::AoEDamage) {
            mvprintw(rows_ + 4, 0, "Spell '%s' used for AoE damage at (%d,%d)!", spell->GetName().c_str(), target_pos.first, target_pos.second);
            int damage_dealt = 0;
            for (int dr = 0; dr < 2; ++dr) {
                for (int dc = 0; dc < 2; ++dc) {
                    int current_r = target_pos.first + dr;
                    int current_c = target_pos.second + dc;

                    // Проверяем врагов
                    for (size_t i = 0; i < enemy_positions.size(); ++i) {
                        if (enemy_positions[i].first == current_r && enemy_positions[i].second == current_c && enemies[i].IsAlive()) {
                            enemies[i].TakeDamage(spell->GetEffectValue());
                            damage_dealt += spell->GetEffectValue();
                            if (!enemies[i].IsAlive()) {
                                mvprintw(rows_ + 5, 0, "Enemy at (%d,%d) dies. +10 score.", current_r, current_c);
                                player.AddScore(10);
                                GainRandomSpell(player);
                            }
                        }
                    }
                    // Проверяем здания
                    for (size_t i = 0; i < building_positions.size(); ++i) {
                        if (building_positions[i].first == current_r && building_positions[i].second == current_c && buildings[i].IsAlive()) {
                            buildings[i].TakeDamage(spell->GetEffectValue());
                            damage_dealt += spell->GetEffectValue();
                            if (!buildings[i].IsAlive()) {
                                mvprintw(rows_ + 5, 0, "Building at (%d,%d) destroyed. +5 score.", current_r, current_c);
                                player.AddScore(5);
                                GainRandomSpell(player);
                            }
                        }
                    }
                }
            }
            if (damage_dealt == 0) {
                mvprintw(rows_ + 5, 0, "No targets hit in the area.");
            } else {
                mvprintw(rows_ + 5, 0, "Total %d damage dealt in the area.", damage_dealt);
            }
        }
        else if (spell->GetType() == SpellType::Heal) {
            // Логика исцеления
            mvprintw(rows_ + 4, 0, "Spell '%s' used for healing!", spell->GetName().c_str());
        } else if (spell->GetType() == SpellType::Buff) {
            // Логика баффа
            mvprintw(rows_ + 4, 0, "Spell '%s' used for buffing!", spell->GetName().c_str());
        } else if (spell->GetType() == SpellType::Debuff) {
            // Логика дебаффа
            mvprintw(rows_ + 4, 0, "Spell '%s' used for debuffing!", spell->GetName().c_str());
        }
        refresh();
        napms(1000);
    }
}

std::unique_ptr<Spell> GameLogic::CreateRandomSpell() {
    static std::mt19937 gen(time(0)); // Инициализируем генератор случайных чисел один раз
    std::uniform_int_distribution<> spell_type_dist(0, 4); // 0-4 для SpellType (включая AoEDamage)
    std::uniform_int_distribution<> effect_value_dist(5, 20); // Значение эффекта от 5 до 20

    SpellType type = static_cast<SpellType>(spell_type_dist(gen));
    int effect_value = effect_value_dist(gen);

    std::string name;
    std::string description;

    switch (type) {
        case SpellType::Damage:
            return std::make_unique<DamageSpell>("Fireball", "Deals " + std::to_string(effect_value) + " damage.", effect_value);
        case SpellType::Heal:
            return std::make_unique<HealSpell>("Healing Potion", "Heals for " + std::to_string(effect_value) + " HP.", effect_value);
        case SpellType::Buff:
            return std::make_unique<BuffSpell>("Strength Potion", "Increases attack by " + std::to_string(effect_value) + ".", effect_value);
        case SpellType::Debuff:
            return std::make_unique<DebuffSpell>("Weakness Curse", "Decreases enemy attack by " + std::to_string(effect_value) + ".", effect_value);
        case SpellType::AoEDamage:
            return std::make_unique<AoEDamageSpell>("Explosion", "Deals " + std::to_string(effect_value) + " damage in 2x2 area.", effect_value);
    }
    return nullptr; // Should not reach here
}

void GameLogic::GainRandomSpell(Player& player) {
    if (player.GetSpellHandSize() < player.GetSpellHandMaxSize()) { // Проверяем, есть ли место в руке
        player.AddSpell(CreateRandomSpell());
        mvprintw(rows_ + 5, 0, "You gained a new spell!");
        refresh();
        napms(1000);
    } else {
        mvprintw(rows_ + 5, 0, "Your spell hand is full!");
        refresh();
        napms(1000);
    }
}

} // namespace game
