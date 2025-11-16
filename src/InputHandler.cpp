#include "InputHandler.h"
#include <ncurses.h>

namespace game {

InputHandler::InputHandler() {}

char InputHandler::GetInput() const {
    int ch = getch(); // getch() по умолчанию блокирующий, но может быть неблокирующим в зависимости от nodelay
    if (ch == ERR) {
        return '\0'; // Нет ввода
    }
    switch (ch) {
    case 'w':
    case 'W':
        return 'w';
    case 'a':
    case 'A':
        return 'a';
    case 's':
    case 'S':
        return 's';
    case 'd':
    case 'D':
        return 'd';
    case 'f':
    case 'F':
        return 'f'; // Переключить режим
    case 'r':
    case 'R':
        return 'r'; // Выстрел
    case 'q':
    case 'Q':
        return 'q'; // Выход
    case 'e':
    case 'E':
        return 'e'; // Использовать заклинание
    case KEY_UP:
        return 'w';
    case KEY_DOWN:
        return 's';
    case KEY_LEFT:
        return 'a';
    case KEY_RIGHT:
        return 'd';
    default:
        return '\0';
    }
}

bool InputHandler::ProcessCommand(char cmd) const {
    // Этот метод можно расширить для обработки специфичных команд
    return cmd != '\0';
}

int InputHandler::GetSpellChoice() const {
    int ch = getch();
    if (ch >= '1' && ch <= '9') {
        return ch - '1'; // Возвращаем индекс заклинания (0-8)
    }
    return -1; // Некорректный выбор
}

std::pair<int, int> InputHandler::GetTargetPosition() const {
    int row, col;
    // Используем mvprintw и scanw для получения координат от пользователя
    // mvprintw(rows_ + 2, 0, "Enter target position (row col): "); // Это будет сделано в Field или Renderer
    scanw(const_cast<char *>("%d %d"), &row, &col);
    return {row, col};
}

} // namespace game
