#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <utility> // Для std::pair

namespace game {

class InputHandler {
public:
    // Конструктор
    InputHandler();
    
    // Метод для получения ввода от пользователя
    char GetInput() const;
    
    // Метод для обработки команд
    bool ProcessCommand(char cmd) const;

    // Метод для получения выбора заклинания
    int GetSpellChoice() const;
    std::pair<int, int> GetTargetPosition() const; // Добавляем метод для получения позиции цели
};

} // namespace game

#endif // INPUT_HANDLER_H_
