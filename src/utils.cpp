#include "utils.h"
#include <ncurses.h>

namespace game {

// Функция для анимации выстрела по прямым
void AnimateArrow(const Position &start, const Position &end) {
  int dx = end.first - start.first;
  int dy = end.second - start.second;

  // Нормализуем вектор
  int step_x = 0;
  int step_y = 0;

  if (dx != 0 && dy == 0) {
    step_x = (dx > 0) ? 1 : -1;
  } else if (dy != 0 && dx == 0) {
    step_y = (dy > 0) ? 1 : -1;
  } else {
    return; // Не по прямой — не анимируем
  }

  Position current = start;

  while (true) {
    // Очистка предыдущей позиции
    mvaddch(current.first, current.second, '.'); // Или символ поля

    // Перемещаемся по вектору
    current.first += step_x;
    current.second += step_y;

    // Если вышли за пределы цели — выходим
    if (current.first == end.first && current.second == end.second) {
      mvaddch(current.first, current.second, 'X'); // Попадание
      break;
    }

    // Рисуем стрелку
    char arrow = '>';
    if (step_x == 0 && step_y > 0)
      arrow = 'v';
    else if (step_x == 0 && step_y < 0)
      arrow = '^';
    else if (step_x > 0 && step_y == 0)
      arrow = '>';
    else if (step_x < 0 && step_y == 0)
      arrow = '<';

    mvaddch(current.first, current.second, arrow);
    refresh();
    napms(100);
  }

  refresh();
}

} // namespace game
