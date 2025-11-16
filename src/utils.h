#ifndef UTILS_H_
#define UTILS_H_

#include <utility> // Для std::pair

namespace game {

using Position = std::pair<int, int>;

enum class AttackMode { kMelee, kRanged };

enum class CellType { kEmpty, kWall, kSlow };

struct Cell {
  CellType type = CellType::kEmpty;
};

// Функция для анимации выстрела (только по прямым направлениям)
void AnimateArrow(const Position &start, const Position &end);

} // namespace game

#endif // UTILS_H_
