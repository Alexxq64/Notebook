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
    cout << "=== ЗАПИСНАЯ КНИЖКА ===" << endl;

    // Автозагрузка данных при запуске приложения
    try {
        notebook.loadFromFile();  // Пытаемся загрузить сохраненные заметки
    }
    catch (const exception& e) {
        cout << "Предупреждение: " << e.what() << endl;
    }

    // Главный цикл приложения
    while (true) {
        showMainMenu();  // Показываем меню и обрабатываем выбор пользователя
    }
}

// Отображение главного меню
void ConsoleUI::showMainMenu() {
    clearScreen();
    cout << "=== ГЛАВНОЕ МЕНЮ ===" << endl;
    cout << "Всего заметок: " << notebook.getNoteCount() << endl;

    // Индикатор несохраненных изменений
    if (unsavedChanges) cout << "(!) Есть несохранённые изменения" << endl;

    cout << "=================" << endl;

    cout << "1. Создать новую заметку" << endl;
    cout << "2. Показать все заметки" << endl;
    cout << "3. Просмотр / редактирование заметки" << endl;
    cout << "4. Удалить заметку" << endl;
    cout << "5. Поиск заметок" << endl;
    cout << "6. Статистика" << endl;
    cout << "7. Сохранить в файл" << endl;
    cout << "8. Загрузить из файла" << endl;
    cout << "9. Тестовые сценарии" << endl;
    cout << "0. Выход" << endl;
    cout << "=================" << endl;

    int choice = getChoice(0, 9);

    switch (choice) {
    case 1: handleCreateNote(); break;
    case 2: handleListNotes(); break;
    case 3: handleViewNote(); break;
    case 4: handleDeleteNote(); break;
    case 5: handleSearch(); break;
    case 6: handleStatistics(); break;
    case 7: handleSave(); break;
    case 8: handleLoad(); break;
    case 9: handleTestScenarios(); break;
    case 0:
        if (unsavedChanges) {
            cout << "Есть несохранённые изменения. Сохранить перед выходом? (да - 1): ";
            char c;
            cin >> c;
            if (c == '1') {
                handleSave();
            }
        }
        cout << "До свидания!" << endl;
        exit(0);
    }
}

// Создание новой заметки
void ConsoleUI::handleCreateNote() {
    clearScreen();
    cout << "=== СОЗДАНИЕ НОВОЙ ЗАМЕТКИ ===" << endl;

    string author = getString("Введите автора: ");
    string title = getString("Введите заголовок: ");
    string content = getString("Введите текст заметки: ");
    vector<string> tags = getTags();

    Note newNote(author, title, content);
    newNote.setTags(tags);

    notebook.addNote(newNote);
    unsavedChanges = true;

    cout << "\nЗаметка успешно создана!" << endl;
    pressAnyKey();
}

// Вывод списка всех заметок
void ConsoleUI::handleListNotes() {
    clearScreen();
    cout << "=== ВСЕ ЗАМЕТКИ ===" << endl;
    notebook.printAll();
    pressAnyKey();
}

// Просмотр конкретной заметки
void ConsoleUI::handleViewNote() {
    clearScreen();
    notebook.printAll();

    if (notebook.getNoteCount() == 0) {
        pressAnyKey();
        return;
    }

    int noteNumber = getInt("Введите номер заметки для просмотра (0 — отмена): ", 0, notebook.getNoteCount());
    if (noteNumber == 0) return;

    int index = noteNumber - 1;

    Note* note = notebook.getNote(index);
    if (note) {
        note->print();

        cout << "\nДействия:" << endl;
        cout << "1. Редактировать заметку" << endl;
        cout << "0. Вернуться в меню" << endl;

        int choice = getChoice(0, 1);
        if (choice == 1) {
            handleEditNote(index);
        }
    }
}

// Редактирование заметки
void ConsoleUI::handleEditNote(int index) {
    if (index < 0 || index >= notebook.getNoteCount()) {
        cout << "Некорректный индекс заметки." << endl;
        pressAnyKey();
        return;
    }

    Note* note = notebook.getNote(index);
    if (!note) {
        cout << "Заметка не найдена." << endl;
        pressAnyKey();
        return;
    }

    clearScreen();
    cout << "=== РЕДАКТИРОВАНИЕ ЗАМЕТКИ ===" << endl;
    note->print();

    cout << "\nЧто вы хотите изменить?" << endl;
    cout << "1. Заголовок" << endl;
    cout << "2. Содержимое" << endl;
    cout << "3. Теги" << endl;
    cout << "4. Всё сразу" << endl;
    cout << "0. Отмена" << endl;

    int choice = getChoice(0, 4);
    if (choice == 0) return;

    try {
        switch (choice) {
        case 1:
            note->setTitle(getString("Новый заголовок: "));
            break;
        case 2:
            note->setContent(getString("Новое содержимое: "));
            break;
        case 3:
            note->setTags(getTags());
            break;
        case 4:
            note->setTitle(getString("Новый заголовок: "));
            note->setContent(getString("Новое содержимое: "));
            note->setTags(getTags());
            break;
        }

        unsavedChanges = true;
        cout << "\nЗаметка успешно обновлена!" << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }

    pressAnyKey();
}

// Удаление заметки
void ConsoleUI::handleDeleteNote() {
    clearScreen();
    notebook.printAll();

    if (notebook.getNoteCount() == 0) {
        pressAnyKey();
        return;
    }

    int index = getInt("Введите номер заметки для удаления (0 — отмена): ",
        0, notebook.getNoteCount());  // теперь от 0 до N

    if (index == 0) return;  // 0 - отмена

    // Преобразуем номер в индекс
    int actualIndex = index - 1;

    cout << "Вы уверены? (да - 1): ";
    char confirm;
    cin >> confirm;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (confirm == '1') {
        if (notebook.removeNote(actualIndex)) {
            unsavedChanges = true;
            cout << "Заметка удалена." << endl;
        }
        else {
            cout << "Ошибка при удалении заметки." << endl;
        }
    }
    pressAnyKey();
}

// Меню поиска
void ConsoleUI::showSearchMenu() {
    clearScreen();
    cout << "=== ПОИСК ЗАМЕТОК ===" << endl;
    cout << "1. По автору" << endl;
    cout << "2. По тегу" << endl;
    cout << "3. По слову" << endl;
    cout << "4. По дате" << endl;
    cout << "5. За последние N дней" << endl;
    cout << "0. Назад" << endl;
}

// Поиск заметок
void ConsoleUI::handleSearch() {
    clearScreen();
    showSearchMenu();

    int choice = getChoice(0, 5);
    if (choice == 0) return;

    vector<int> results;

    switch (choice) {
    case 1:
        results = notebook.findByAuthor(getString("Введите автора: "));
        break;
    case 2:
        results = notebook.findByTag(getString("Введите тег: "));
        break;
    case 3:
        results = notebook.findByWord(getString("Введите слово для поиска: "));
        break;
    case 4:
        results = notebook.findByDate(getString("Введите дату создания (ГГГГ-ММ-ДД): "));
        break;
    case 5:
        results = notebook.findByLastNDays(getInt("Введите количество дней: ", 1, 365));
        break;
    }

    notebook.printNotes(results);
    pressAnyKey();
}

// Статистика
void ConsoleUI::handleStatistics() {
    clearScreen();
    cout << "=== СТАТИСТИКА ===" << endl;

    auto authorStats = notebook.getAuthorStats();
    cout << "\nПо авторам:" << endl;
    if (authorStats.empty()) {
        cout << "Нет данных" << endl;
    }
    else {
        for (const auto& pair : authorStats) {
            cout << "  " << pair.first << ": " << pair.second << " заметок" << endl;
        }
    }

    auto tagStats = notebook.getTagStats();
    cout << "\nПо тегам:" << endl;
    if (tagStats.empty()) {
        cout << "Нет данных" << endl;
    }
    else {
        for (const auto& pair : tagStats) {
            cout << "  #" << pair.first << ": " << pair.second << " раз" << endl;
        }
    }

    pressAnyKey();
}

// Сохранение
void ConsoleUI::handleSave() {
    try {
        notebook.saveToFile();
        unsavedChanges = false;
        cout << "Сохранение выполнено успешно!" << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка сохранения: " << e.what() << endl;
    }
    pressAnyKey();
}

// Загрузка
void ConsoleUI::handleLoad() {
    try {
        notebook.loadFromFile();
        unsavedChanges = false;
        cout << "Данные успешно загружены!" << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка загрузки: " << e.what() << endl;
    }
    pressAnyKey();
}

// Тестовые сценарии
void ConsoleUI::handleTestScenarios() {
    clearScreen();
    notebook.runTestScenarios();
    pressAnyKey();
}


// Вспомогательные методы
int ConsoleUI::getChoice(int min, int max) {
    int choice;
    while (true) {
        cout << "Введите выбор [" << min << "-" << max << "]: ";
        cin >> choice;

        if (cin.fail() || choice < min || choice > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод. Повторите." << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return choice;
}

string ConsoleUI::getString(const string& prompt, bool allowEmpty) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);

        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);

        if (!allowEmpty && input.empty()) {
            cout << "Поле не может быть пустым. Повторите ввод." << endl;
        }
        else {
            break;
        }
    }
    return input;
}

int ConsoleUI::getInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail() || value < min || value > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод. Допустимый диапазон: " << min << " - " << max << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return value;
}

vector<string> ConsoleUI::getTags() {
    vector<string> tags;
    cout << "Введите теги (через запятую): ";
    string input;
    getline(cin, input);

    if (!input.empty()) {
        string tag;
        size_t start = 0, end = 0;

        while ((end = input.find(',', start)) != string::npos) {
            tag = input.substr(start, end - start);
            tag.erase(0, tag.find_first_not_of(" \t"));
            tag.erase(tag.find_last_not_of(" \t") + 1);
            if (!tag.empty()) tags.push_back(tag);
            start = end + 1;
        }

        tag = input.substr(start);
        tag.erase(0, tag.find_first_not_of(" \t"));
        tag.erase(tag.find_last_not_of(" \t") + 1);
        if (!tag.empty()) tags.push_back(tag);
    }
    return tags;
}

void ConsoleUI::pressAnyKey() {
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void ConsoleUI::clearScreen() {
    system(CLEAR_SCREEN);
}
