#include "ConsoleUI.h"
#include "Notebook.h"
#include <iostream>

using namespace std;

void ConsoleUI::run() {
    cout << "=== Notebook App ===" << endl;

    Notebook notebook;

    // Тест 1: Добавление заметок
    cout << "\n1. Testing Notebook CRUD..." << endl;
    notebook.addNote(Note("Alice", "First Note", "Hello world"));
    notebook.addNote(Note("Bob", "Second Note", "Meeting tomorrow"));
    cout << "   Added " << notebook.getNoteCount() << " notes" << endl;

    // Тест 2: Поиск
    cout << "\n2. Testing search..." << endl;
    auto results = notebook.findByAuthor("Alice");
    cout << "   Found " << results.size() << " notes by Alice" << endl;

    // Тест 3: Статистика
    cout << "\n3. Testing statistics..." << endl;
    auto authorStats = notebook.getAuthorStats();
    for (const auto& pair : authorStats) {
        cout << "   " << pair.first << ": " << pair.second << endl;
    }

    // Тест 4: Тестовые сценарии
    cout << "\n4. Running test scenarios..." << endl;
    notebook.runTestScenarios();

    cout << "\nPress Enter to exit...";
    cin.get();
}