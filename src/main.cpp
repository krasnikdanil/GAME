#include "Field.h"
#include <iostream>

namespace game {

void RunGame() {
  int r, c;
  std::cout << "Enter rows and cols for field (10..25 recommended, you can "
               "enter e.g. 12 12): ";
  if (!(std::cin >> r >> c))
    return;

  Field field(r, c, 12, 6); // 12 стен, 6 замедления
  field.RunGameLoop();
}

} // namespace game

int main() {
  try {
    game::RunGame();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
