#ifndef GAME_LOGIC_H_
#define GAME_LOGIC_H_

#include "utils.h"
#include "Player.h"
#include "Enemy.h"
#include "Building.h"
#include <vector>
#include <utility>
#include "Spell.h" // Добавляем Spell.h для CreateRandomSpell
#include <memory> // Для std::unique_ptr

namespace game {


class GameLogic {
public:
    // Конструктор
    GameLogic(int rows, int cols);
    
    // Методы для обработки хода игрока
    void ProcessPlayerTurn(Player& player,
                           std::pair<int, int>& player_pos,
                           bool& player_stunned,
                           const std::vector<std::vector<Cell>>& grid,
                           std::vector<Enemy>& enemies,
                           std::vector<std::pair<int, int>>& enemy_positions,
                           [[maybe_unused]] std::vector<Building>& buildings,
                           std::vector<std::pair<int, int>>& building_positions,
                           char cmd);
    
    // Метод для обработки использования заклинаний
    void ProcessSpellCast(Player& player,
                          std::vector<Enemy>& enemies, // Изменено на неконстантную ссылку
                          std::vector<std::pair<int, int>>& enemy_positions, // Изменено на неконстантную ссылку
                          std::vector<Building>& buildings, // Изменено на неконстантную ссылку
                          std::vector<std::pair<int, int>>& building_positions, // Изменено на неконстантную ссылку
                          int spell_index,
                          const std::pair<int, int>& target_pos); // Добавлен target_pos
    
    // Методы для обработки врагов
    void ProcessEnemies(Player& player,
                        const std::pair<int, int>& player_pos,
                        const std::vector<std::vector<Cell>>& grid,
                        std::vector<Enemy>& enemies,
                        std::vector<std::pair<int, int>>& enemy_positions,
                        const std::vector<std::pair<int, int>>& building_positions);
    
    // Методы для обработки зданий
    void ProcessBuildings(std::vector<Building>& buildings,
                          const std::vector<std::pair<int, int>>& building_positions,
                          std::vector<Enemy>& enemies,
                          std::vector<std::pair<int, int>>& enemy_positions,
                          const std::vector<std::vector<Cell>>& grid,
                          const std::pair<int, int>& player_pos);
                          
    // Проверка, можно ли переместиться на позицию
    bool IsBlocked(int r, int c, const std::vector<std::vector<Cell>>& grid) const;
    
    // Проверка, является ли клетка замедляющей
    bool IsSlow(int r, int c, const std::vector<std::vector<Cell>>& grid) const;
    
    // Проверка, находится ли позиция внутри поля
    bool IsInside(int r, int c) const;
    
    // Проверка, занята ли клетка врагом
    bool IsOccupiedByEnemy(int r, int c,
                           const std::vector<std::pair<int, int>>& enemy_positions) const;
    
    // Проверка, занята ли клетка зданием
    bool IsOccupiedByBuilding(int r, int c,
                              const std::vector<std::pair<int, int>>& building_positions) const;
    
    // Поиск свободной клетки рядом с позицией
    bool FindAdjacentFree(const std::pair<int, int> &center,
                          std::pair<int, int> &out,
                          const std::vector<std::vector<Cell>>& grid,
                          const std::pair<int, int>& player_pos,
                          const std::vector<std::pair<int, int>>& enemy_positions,
                          const std::vector<std::pair<int, int>>& building_positions) const;

private:
    int rows_;
    int cols_;

public:
    // Создание случайного заклинания
    static std::unique_ptr<Spell> CreateRandomSpell();

    // Добавляем метод для получения нового случайного заклинания
    void GainRandomSpell(Player& player);
};

} // namespace game

#endif // GAME_LOGIC_H_
