#include "ConsoleUI.h"
#include <windows.h>

int main() {
    // Просто устанавливаем кодировку консоли
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    ConsoleUI app;
    app.run();

    return 0;
}