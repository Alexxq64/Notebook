#include "ConsoleUI.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cstdlib>

// Кроссплатформенная команда очистки экрана
#ifdef _WIN32
#define CLEAR_SCREEN "cls"     // Для Windows
#else
#define CLEAR_SCREEN "clear"   // Для Linux/Mac
#endif

using namespace std;

// Главный метод запуска приложения
void ConsoleUI::run() {
    clearScreen();  // Очищаем экран
    cout << "=== NOTEBOOK APPLICATION ===" << endl;

    // Автозагрузка данных при запуске приложения
    try {
        notebook.loadFromFile();  // Пытаемся загрузить сохраненные заметки
    }
    catch (const exception& e) {
        cout << "Warning: " << e.what() << endl;  // Выводим предупреждение при ошибке
    }

    // Главный цикл приложения
    while (true) {
        showMainMenu();  // Показываем меню и обрабатываем выбор пользователя
    }
}

// Отображение главного меню
void ConsoleUI::showMainMenu() {
    clearScreen();
    cout << "=== MAIN MENU ===" << endl;
    cout << "Total notes: " << notebook.getNoteCount() << endl;  // Показываем количество заметок

    // Индикатор несохраненных изменений
    if (unsavedChanges) cout << "(!) Unsaved changes" << endl;

    cout << "=================" << endl;

    // Опции меню
    cout << "1. Create new note" << endl;
    cout << "2. List all notes" << endl;
    cout << "3. View/edit note" << endl;
    cout << "4. Delete note" << endl;
    cout << "5. Search notes" << endl;
    cout << "6. Statistics" << endl;
    cout << "7. Save to file" << endl;
    cout << "8. Load from file" << endl;
    cout << "9. Test scenarios" << endl;
    cout << "0. Exit" << endl;
    cout << "=================" << endl;

    // Получаем выбор пользователя (от 0 до 9)
    int choice = getChoice(0, 9);

    // Обработка выбора пользователя
    switch (choice) {
    case 1: handleCreateNote(); break;     // Создание заметки
    case 2: handleListNotes(); break;      // Список заметок
    case 3: handleViewNote(); break;       // Просмотр/редактирование
    case 4: handleDeleteNote(); break;     // Удаление заметки
    case 5: handleSearch(); break;         // Поиск заметок
    case 6: handleStatistics(); break;     // Статистика
    case 7: handleSave(); break;           // Сохранение в файл
    case 8: handleLoad(); break;           // Загрузка из файла
    case 9: handleTestScenarios(); break;  // Тестовые сценарии
    case 0:  // Выход из приложения
        // Проверяем наличие несохраненных изменений
        if (unsavedChanges) {
            cout << "You have unsaved changes. Save before exit? (y/n): ";
            char c;
            cin >> c;
            if (c == 'y' || c == 'Y') {
                handleSave();  // Сохраняем если пользователь согласен
            }
        }
        cout << "Goodbye!" << endl;
        exit(0);  // Завершаем программу
    }
}

// Создание новой заметки
void ConsoleUI::handleCreateNote() {
    clearScreen();
    cout << "=== CREATE NEW NOTE ===" << endl;

    // Получаем данные от пользователя
    string author = getString("Enter author: ");        // Автор (не может быть пустым)
    string title = getString("Enter title: ");          // Заголовок (не может быть пустым)
    string content = getString("Enter content: ");      // Содержимое
    vector<string> tags = getTags();                    // Теги через запятую

    // Создаем новую заметку
    Note newNote(author, title, content);
    newNote.setTags(tags);  // Устанавливаем теги

    // Добавляем заметку в записную книжку
    notebook.addNote(newNote);
    unsavedChanges = true;  // Устанавливаем флаг изменений

    cout << "\nNote created successfully!" << endl;
    pressAnyKey();  // Ожидаем нажатия Enter
}

// Вывод списка всех заметок (краткий формат)
void ConsoleUI::handleListNotes() {
    clearScreen();
    cout << "=== ALL NOTES ===" << endl;
    notebook.printAll();  // Вызываем метод Notebook для вывода списка
    pressAnyKey();
}

// Просмотр конкретной заметки
void ConsoleUI::handleViewNote() {
    clearScreen();
    notebook.printAll();  // Сначала показываем список заметок

    // Проверяем, есть ли заметки
    if (notebook.getNoteCount() == 0) {
        pressAnyKey();
        return;
    }

    // Запрашиваем номер заметки (от 1 до количества заметок)
    int noteNumber = getInt("Enter note number to view (0 to cancel): ", 0, notebook.getNoteCount());
    if (noteNumber == 0) return;  // Отмена

    // Конвертируем номер в индекс (нумерация с 1, индексы с 0)
    int index = noteNumber - 1;

    // Получаем указатель на заметку
    Note* note = notebook.getNote(index);
    if (note) {
        note->print();  // Выводим полную информацию о заметке

        // Дополнительные опции
        cout << "\nOptions:" << endl;
        cout << "1. Edit this note" << endl;
        cout << "2. Back to menu" << endl;

        int choice = getChoice(1, 2);
        if (choice == 1) {
            handleEditNote(index);  // Переходим к редактированию
        }
    }
}

// Редактирование существующей заметки
void ConsoleUI::handleEditNote(int index) {
    // Проверка валидности индекса
    if (index < 0 || index >= notebook.getNoteCount()) {
        cout << "Invalid note index." << endl;
        pressAnyKey();
        return;
    }

    // Получаем указатель на заметку
    Note* note = notebook.getNote(index);
    if (!note) {
        cout << "Note not found." << endl;
        pressAnyKey();
        return;
    }

    clearScreen();
    cout << "=== Editing Note ===" << endl;
    note->print();  // Показываем текущее содержимое заметки

    // Меню редактирования
    cout << "\nWhat do you want to edit?" << endl;
    cout << "1. Title" << endl;
    cout << "2. Content" << endl;
    cout << "3. Tags" << endl;
    cout << "4. All fields" << endl;
    cout << "0. Cancel" << endl;

    int choice = getChoice(0, 4);
    if (choice == 0) return;  // Отмена

    try {
        switch (choice) {
        case 1: {  // Редактирование заголовка
            string newTitle = getString("New title: ");
            note->setTitle(newTitle);
            break;
        }
        case 2: {  // Редактирование содержимого
            string newContent = getString("New content: ");
            note->setContent(newContent);
            break;
        }
        case 3: {  // Редактирование тегов
            vector<string> newTags = getTags();
            note->setTags(newTags);
            break;
        }
        case 4: {  // Полное редактирование
            string newTitle = getString("New title: ");
            string newContent = getString("New content: ");
            vector<string> newTags = getTags();

            note->setTitle(newTitle);
            note->setContent(newContent);
            note->setTags(newTags);
            break;
        }
        }

        unsavedChanges = true;  // Устанавливаем флаг изменений
        cout << "\nNote updated successfully!" << endl;
    }
    catch (const exception& e) {
        // Обработка ошибок (например, пустой заголовок)
        cout << "Error: " << e.what() << endl;
    }

    pressAnyKey();
}

// Удаление заметки
void ConsoleUI::handleDeleteNote() {
    clearScreen();
    notebook.printAll();  // Показываем список заметок

    // Проверяем, есть ли заметки
    if (notebook.getNoteCount() == 0) {
        pressAnyKey();
        return;
    }

    // Запрашиваем номер заметки для удаления
    int index = getInt("Enter note number to delete (-1 to cancel): ", -1, notebook.getNoteCount() - 1);
    if (index == -1) return;  // Отмена

    // Подтверждение удаления
    cout << "Are you sure? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (confirm == 'y' || confirm == 'Y') {
        // Пытаемся удалить заметку
        if (notebook.removeNote(index)) {
            unsavedChanges = true;  // Устанавливаем флаг изменений
            cout << "Note deleted." << endl;
        }
        else {
            cout << "Error deleting note." << endl;
        }
    }
    pressAnyKey();
}

// Меню поиска
void ConsoleUI::showSearchMenu() {
    clearScreen();
    cout << "=== SEARCH NOTES ===" << endl;
    cout << "1. By author" << endl;
    cout << "2. By tag" << endl;
    cout << "3. By word" << endl;
    cout << "4. By date" << endl;
    cout << "5. Last N days" << endl;
    cout << "0. Back" << endl;
}

// Поиск заметок по различным критериям
void ConsoleUI::handleSearch() {
    clearScreen();
    showSearchMenu();  // Показываем меню поиска

    int choice = getChoice(0, 5);
    if (choice == 0) return;  // Назад

    vector<int> results;  // Вектор для хранения индексов найденных заметок

    // Обработка выбора критерия поиска
    switch (choice) {
    case 1: {  // Поиск по автору
        string author = getString("Enter author name: ");
        results = notebook.findByAuthor(author);
        break;
    }
    case 2: {  // Поиск по тегу
        string tag = getString("Enter tag: ");
        results = notebook.findByTag(tag);
        break;
    }
    case 3: {  // Поиск по слову
        string word = getString("Enter word to search: ");
        results = notebook.findByWord(word);
        break;
    }
    case 4: {  // Поиск по дате
        string date = getString("Enter date (YYYY-MM-DD): ");
        results = notebook.findByDate(date);
        break;
    }
    case 5: {  // Поиск за последние N дней
        int days = getInt("Enter number of days: ", 1, 365);
        results = notebook.findByLastNDays(days);
        break;
    }
    }

    // Выводим результаты поиска
    notebook.printNotes(results);
    pressAnyKey();
}

// Вывод статистики
void ConsoleUI::handleStatistics() {
    clearScreen();
    cout << "=== STATISTICS ===" << endl;

    // Статистика по авторам
    auto authorStats = notebook.getAuthorStats();
    cout << "\nBy Author:" << endl;
    if (authorStats.empty()) {
        cout << "No data" << endl;
    }
    else {
        // Выводим количество заметок для каждого автора
        for (const auto& pair : authorStats) {
            cout << "  " << pair.first << ": " << pair.second << " notes" << endl;
        }
    }

    // Статистика по тегам
    auto tagStats = notebook.getTagStats();
    cout << "\nBy Tag:" << endl;
    if (tagStats.empty()) {
        cout << "No data" << endl;
    }
    else {
        // Выводим количество использований каждого тега
        for (const auto& pair : tagStats) {
            cout << "  #" << pair.first << ": " << pair.second << " times" << endl;
        }
    }

    pressAnyKey();
}

// Сохранение данных в файл
void ConsoleUI::handleSave() {
    try {
        notebook.saveToFile();    // Сохраняем в файл
        unsavedChanges = false;   // Сбрасываем флаг изменений
        cout << "Saved successfully!" << endl;
    }
    catch (const exception& e) {
        cout << "Error saving: " << e.what() << endl;
    }
    pressAnyKey();
}

// Загрузка данных из файла
void ConsoleUI::handleLoad() {
    try {
        notebook.loadFromFile();  // Загружаем из файла
        unsavedChanges = false;   // Сбрасываем флаг изменений
        cout << "Loaded successfully!" << endl;
    }
    catch (const exception& e) {
        cout << "Error loading: " << e.what() << endl;
    }
    pressAnyKey();
}

// Запуск тестовых сценариев
void ConsoleUI::handleTestScenarios() {
    clearScreen();
    notebook.runTestScenarios();  // Запускаем все тесты
    pressAnyKey();
}

// Демонстрация полиморфизма
void ConsoleUI::handlePolymorphismDemo() {
    clearScreen();
    std::cout << "=== Polymorphism Demonstration ===" << std::endl;
    notebook.printAllStorageInfo();  // Вызываем метод, демонстрирующий полиморфизм
    pressAnyKey();
}

// ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

// Получение выбора пользователя с валидацией
int ConsoleUI::getChoice(int min, int max) {
    int choice;
    while (true) {
        cout << "Enter choice [" << min << "-" << max << "]: ";
        cin >> choice;

        // Проверка корректности ввода
        if (cin.fail() || choice < min || choice > max) {
            cin.clear();  // Сбрасываем флаг ошибки
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Очищаем буфер
            cout << "Invalid input. Try again." << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Очищаем буфер
            break;
        }
    }
    return choice;
}

// Получение строки от пользователя с валидацией
string ConsoleUI::getString(const string& prompt, bool allowEmpty) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);  // Читаем всю строку

        // Убираем пробелы в начале и конце
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);

        // Проверка на пустую строку (если не разрешено)
        if (!allowEmpty && input.empty()) {
            cout << "Input cannot be empty. Try again." << endl;
        }
        else {
            break;
        }
    }
    return input;
}

// Получение целого числа от пользователя с валидацией диапазона
int ConsoleUI::getInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        // Проверка корректности ввода
        if (cin.fail() || value < min || value > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Must be between " << min << " and " << max << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return value;
}

// Получение списка тегов от пользователя
vector<string> ConsoleUI::getTags() {
    vector<string> tags;
    cout << "Enter tags (comma separated, e.g.: work,personal,urgent): ";
    string input;
    getline(cin, input);  // Читаем строку с тегами

    if (!input.empty()) {
        string tag;
        size_t start = 0, end = 0;

        // Разделяем строку по запятым
        while ((end = input.find(',', start)) != string::npos) {
            tag = input.substr(start, end - start);

            // Убираем пробелы в начале и конце тега
            tag.erase(0, tag.find_first_not_of(" \t"));
            tag.erase(tag.find_last_not_of(" \t") + 1);

            // Добавляем непустые теги
            if (!tag.empty()) {
                tags.push_back(tag);
            }
            start = end + 1;
        }

        // Обрабатываем последний тег (после последней запятой)
        tag = input.substr(start);
        tag.erase(0, tag.find_first_not_of(" \t"));
        tag.erase(tag.find_last_not_of(" \t") + 1);

        if (!tag.empty()) {
            tags.push_back(tag);
        }
    }
    return tags;
}

// Ожидание нажатия Enter для продолжения
void ConsoleUI::pressAnyKey() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Очистка экрана консоли
void ConsoleUI::clearScreen() {
    system(CLEAR_SCREEN);  // Выполняем команду очистки экрана
}