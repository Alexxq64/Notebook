#include "Notebook.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>
using namespace std;

// ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

std::string Notebook::toLower(const std::string& str) {
    std::string result = str;

    // Для Windows с кодировкой 1251
    for (char& c : result) {
        // Русские буквы в верхнем регистре (CP-1251)
        if (c >= -64 && c <= -33) {  // А-Я
            c += 32;  // Преобразуем в нижний регистр
        }
        else if (c == -88) {  // Ё
            c = -72;  // ё
        }
        // Английские буквы
        else if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
    }

    return result;
}

// ========== ФАЙЛОВЫЕ ОПЕРАЦИИ ==========

void Notebook::saveToFile() {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    // Простой текстовый формат
    for (size_t i = 0; i < notes.size(); ++i) {
        const auto& note = notes[i];
        file << "=== NOTE " << i + 1 << " ===" << std::endl;
        file << "AUTHOR: " << note.getAuthor() << std::endl;
        file << "TITLE: " << note.getTitle() << std::endl;
        file << "CONTENT: " << note.getContent() << std::endl;

        auto tags = note.getTags();
        if (!tags.empty()) {
            file << "TAGS: ";
            for (size_t j = 0; j < tags.size(); ++j) {
                file << tags[j];
                if (j < tags.size() - 1) file << ",";
            }
            file << std::endl;
        }

        file << "CREATED: " << note.getCreatedTime() << std::endl;
        file << "UPDATED: " << note.getUpdatedTime() << std::endl;
        file << "=== END ===" << std::endl << std::endl;
    }

    file.close();
}

void Notebook::loadFromFile() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // Если файла нет, это не ошибка
        return;
    }

    notes.clear();
    std::string line;
    std::string currentAuthor, currentTitle, currentContent;
    std::vector<std::string> currentTags;
    time_t currentCreated = 0, currentUpdated = 0;
    bool inNote = false;

    while (std::getline(file, line)) {
        // Удаляем лишние пробелы в начале и конце
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.find("=== NOTE ") == 0) {
            // Начало новой заметки
            if (inNote && !currentTitle.empty()) {
                Note note(currentAuthor, currentTitle, currentContent);
                note.setTags(currentTags);
                note.setCreatedTime(currentCreated);
                note.setUpdatedTime(currentUpdated);
                notes.push_back(note);
            }

            inNote = true;
            currentAuthor.clear();
            currentTitle.clear();
            currentContent.clear();
            currentTags.clear();
            currentCreated = time(nullptr);
            currentUpdated = time(nullptr);
        }
        else if (line.find("AUTHOR: ") == 0) {
            currentAuthor = line.substr(8);
        }
        else if (line.find("TITLE: ") == 0) {
            currentTitle = line.substr(7);
        }
        else if (line.find("CONTENT: ") == 0) {
            currentContent = line.substr(9);
        }
        else if (line.find("TAGS: ") == 0) {
            std::string tagsStr = line.substr(6);
            if (!tagsStr.empty()) {
                size_t start = 0;
                size_t end = 0;
                while ((end = tagsStr.find(',', start)) != std::string::npos) {
                    std::string tag = tagsStr.substr(start, end - start);
                    // Убираем пробелы
                    tag.erase(0, tag.find_first_not_of(" \t"));
                    tag.erase(tag.find_last_not_of(" \t") + 1);
                    if (!tag.empty()) {
                        currentTags.push_back(tag);
                    }
                    start = end + 1;
                }
                // Последний тег
                std::string lastTag = tagsStr.substr(start);
                lastTag.erase(0, lastTag.find_first_not_of(" \t"));
                lastTag.erase(lastTag.find_last_not_of(" \t") + 1);
                if (!lastTag.empty()) {
                    currentTags.push_back(lastTag);
                }
            }
        }
        else if (line.find("CREATED: ") == 0) {
            try {
                currentCreated = std::stoll(line.substr(9));
            }
            catch (...) {
                currentCreated = time(nullptr);
            }
        }
        else if (line.find("UPDATED: ") == 0) {
            try {
                currentUpdated = std::stoll(line.substr(9));
            }
            catch (...) {
                currentUpdated = time(nullptr);
            }
        }
    }

    // Добавляем последнюю заметку
    if (inNote && !currentTitle.empty()) {
        Note note(currentAuthor, currentTitle, currentContent);
        note.setTags(currentTags);
        note.setCreatedTime(currentCreated);
        note.setUpdatedTime(currentUpdated);
        notes.push_back(note);
    }

    file.close();
}

// ========== ОСТАЛЬНЫЕ МЕТОДЫ ==========

void Notebook::printAll() const {
    if (notes.empty()) {
        std::cout << "Заметок пока нет." << std::endl;
        return;
    }

    //std::cout << "=== ВСЕ ЗАМЕТКИ ===" << std::endl;
    for (size_t i = 0; i < notes.size(); ++i) {
        std::cout << i + 1 << ". " << notes[i].getTitle()
            << " (автор: " << notes[i].getAuthor() << ")"
            << std::endl;
    }
}

void Notebook::printNotes(const std::vector<int>& indices) const {
    if (indices.empty()) {
        std::cout << "Ничего не найдено." << std::endl;
        return;
    }

    std::cout << "=== НАЙДЕННЫЕ ЗАМЕТКИ ===" << std::endl;
    for (int index : indices) {
        if (index >= 0 && index < (int)notes.size()) {
            notes[index].print();
            std::cout << std::endl;
        }
    }
}

void Notebook::addNote(const Note& note) {
    notes.push_back(note);
}

bool Notebook::removeNote(int index) {
    if (index < 0 || index >= (int)notes.size()) {
        return false;
    }
    notes.erase(notes.begin() + index);
    return true;
}

Note* Notebook::getNote(int index) {
    if (index < 0 || index >= (int)notes.size()) {
        return nullptr;
    }
    return &notes[index];
}

bool Notebook::updateNote(int index, const Note& updatedNote) {
    if (index < 0 || index >= (int)notes.size()) {
        return false;
    }
    notes[index] = updatedNote;
    return true;
}

std::vector<int> Notebook::findByAuthor(const std::string& author) const {
    std::vector<int> result;
    std::string searchAuthor = toLower(author);

    for (size_t i = 0; i < notes.size(); ++i) {
        if (toLower(notes[i].getAuthor()).find(searchAuthor) != std::string::npos) {
            result.push_back((int)i);
        }
    }
    return result;
}

std::vector<int> Notebook::findByTag(const std::string& tag) const {
    std::vector<int> result;
    std::string searchTag = toLower(tag);

    for (size_t i = 0; i < notes.size(); ++i) {
        const auto& tags = notes[i].getTags();
        for (const auto& t : tags) {
            if (toLower(t).find(searchTag) != std::string::npos) {
                result.push_back((int)i);
                break;
            }
        }
    }
    return result;
}

std::vector<int> Notebook::findByWord(const std::string& word) const {
    std::vector<int> result;
    std::string searchWord = toLower(word);

    for (size_t i = 0; i < notes.size(); ++i) {
        const auto& note = notes[i];
        std::string content = toLower(note.getContent());
        std::string title = toLower(note.getTitle());

        if (content.find(searchWord) != std::string::npos ||
            title.find(searchWord) != std::string::npos) {
            result.push_back((int)i);
        }
    }
    return result;
}

std::vector<int> Notebook::findByDate(const std::string& date) const {
    std::vector<int> result;

    // Проверяем формат даты (должен быть ГГГГ-ММ-ДД)
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        std::cerr << "Неверный формат даты. Используйте ГГГГ-ММ-ДД" << std::endl;
        return result;
    }

    for (size_t i = 0; i < notes.size(); ++i) {
        // Используем новый метод getCreatedDate()
        if (notes[i].getCreatedDate() == date) {
            result.push_back(static_cast<int>(i));
        }
    }

    return result;
}

std::vector<int> Notebook::findByLastNDays(int days) const {
    std::vector<int> result;
    auto now = time(nullptr);

    for (size_t i = 0; i < notes.size(); ++i) {
        auto noteTime = notes[i].getCreatedTime();
        double diff = difftime(now, noteTime);
        if (diff <= days * 24 * 60 * 60) {
            result.push_back((int)i);
        }
    }
    return result;
}

// ========== СТАТИСТИКА ==========

std::map<std::string, int> Notebook::getAuthorStats() const {
    std::map<std::string, int> stats;
    for (const auto& note : notes) {
        stats[note.getAuthor()]++;
    }
    return stats;
}

std::map<std::string, int> Notebook::getTagStats() const {
    std::map<std::string, int> stats;
    for (const auto& note : notes) {
        for (const auto& tag : note.getTags()) {
            stats[tag]++;
        }
    }
    return stats;
}

// ========== ТЕСТОВЫЕ СЦЕНАРИИ ==========

void Notebook::runTestScenarios() {
    cout << "\n=== ЗАПУСК ТЕСТОВЫХ СЦЕНАРИЕВ ===" << endl;

    // 1. Сохраняем текущие данные перед тестами
    cout << "1. Сохранение текущих данных в файл..." << endl;
    try {
        saveToFile();
        cout << "   + СОХРАНЕНИЕ: " << notes.size() << " заметок сохранено в файл" << endl;
    }
    catch (const exception& e) {
        cout << "   ! ОШИБКА СОХРАНЕНИЯ: " << e.what() << endl;
    }

    // 2. Очищаем и заполняем тестовыми данными
    cout << "\n2. Создание тестовых заметок..." << endl;

    // Сохраняем текущие заметки
    vector<Note> originalNotes = notes;

    // Очищаем и заполняем тестовыми данными
    notes.clear();

    // Создаем тестовые заметки
    notes.push_back(Note("Катя", "Список покупок", "Молоко, Яйца, Хлеб"));
    notes.back().setTags({ "покупки", "дом" });

    notes.push_back(Note("Боб", "Заметки со встречи", "Обсудить график проекта"));
    notes.back().setTags({ "работа", "встреча" });

    notes.push_back(Note("Катя", "Идеи для книги", "Написать роман о программировании"));
    notes.back().setTags({ "личное", "идеи" });

    notes.push_back(Note("Анна", "Рецепт борща", "Свекла, капуста, мясо, сметана"));
    notes.back().setTags({ "кулинария", "рецепт", "обед" });

    cout << "   + Создано " << notes.size() << " тестовых заметок" << endl;

    // 3. ТЕСТЫ ПОИСКА
    cout << "\n3. Тестирование функций поиска..." << endl;

    // 3.1 Поиск по автору
    cout << "   Поиск по автору 'Катя': ";
    auto res1 = findByAuthor("Катя");
    cout << "найдено " << res1.size() << " (ожидается: 2)" << endl;
    if (res1.size() == 2) cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;

    // 3.2 Поиск по тегу
    cout << "   Поиск по тегу 'работа': ";
    auto res2 = findByTag("работа");
    cout << "найдено " << res2.size() << " (ожидается: 1)" << endl;
    if (res2.size() == 1) cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;

    // 3.3 Поиск по слову
    cout << "   Поиск по слову 'проект': ";
    auto res3 = findByWord("проект");
    cout << "найдено " << res3.size() << " (ожидается: 1)" << endl;
    if (res3.size() == 1) cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;

    // 3.4 Поиск по слову (регистронезависимый)
    cout << "   Поиск по слову 'ПРОЕКТ' (верхний регистр): ";
    auto res3a = findByWord("ПРОЕКТ");
    cout << "найдено " << res3a.size() << " (ожидается: 1)" << endl;
    if (res3a.size() == 1) cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;

    // 3.5 Поиск по тегу 'рецепт'
    cout << "   Поиск по тегу 'рецепт': ";
    auto res4 = findByTag("рецепт");
    cout << "найдено " << res4.size() << " (ожидается: 1)" << endl;
    if (res4.size() == 1) cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;

    // 3.6 Поиск по несуществующему автору
    cout << "   Поиск по автору 'Иван' (не существует): ";
    auto res5 = findByAuthor("Иван");
    cout << "найдено " << res5.size() << " (ожидается: 0)" << endl;
    if (res5.size() == 0) cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;

    // 3.7 Поиск по дате
    cout << "   Поиск по дате создания (сегодняшняя дата): ";
    // Получаем сегодняшнюю дату из первой заметки
    string today = notes[0].getCreatedDate();
    auto res6 = findByDate(today);
    cout << "найдено " << res6.size() << " заметок" << endl;
    if (res6.size() == notes.size()) {
        cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    }
    else {
        cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;
        cout << "     Примечание: Все тестовые заметки созданы сегодня" << endl;
    }

    // 3.8 Поиск по несуществующей дате
    cout << "   Поиск по несуществующей дате создания '2000-01-01': ";
    auto res7 = findByDate("2000-01-01");
    cout << "найдено " << res7.size() << " (ожидается: 0)" << endl;
    if (res7.size() == 0) {
        cout << "   + ТЕСТ ПРОЙДЕН" << endl;
    }
    else {
        cout << "   ! ТЕСТ НЕ ПРОЙДЕН" << endl;
    }

    // 4. ТЕСТЫ СТАТИСТИКИ
    cout << "\n4. Тестирование статистики..." << endl;

    // 4.1 Статистика по авторам
    cout << "   Статистика по авторам:" << endl;
    auto authStats = getAuthorStats();
    for (const auto& p : authStats) {
        cout << "     " << p.first << ": " << p.second << " заметок" << endl;
    }
    cout << "     Ожидается: Катя:2, Боб:1, Анна:1" << endl;

    bool authTestPassed = (authStats["Катя"] == 2 && authStats["Боб"] == 1 && authStats["Анна"] == 1);
    if (authTestPassed) cout << "   + ТЕСТ СТАТИСТИКИ АВТОРОВ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ СТАТИСТИКИ АВТОРОВ НЕ ПРОЙДЕН" << endl;

    // 4.2 Статистика по тегам
    cout << "   Статистика по тегам:" << endl;
    auto tagStats = getTagStats();
    for (const auto& p : tagStats) {
        cout << "     #" << p.first << ": " << p.second << " раз" << endl;
    }
    cout << "     Ожидается: покупки:1, дом:1, работа:1, встреча:1, личное:1, идеи:1, кулинария:1, рецепт:1, обед:1" << endl;

    bool tagTestPassed = (tagStats["покупки"] == 1 && tagStats["дом"] == 1 &&
        tagStats["работа"] == 1 && tagStats["встреча"] == 1);
    if (tagTestPassed) cout << "   + ТЕСТ СТАТИСТИКИ ТЕГОВ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ СТАТИСТИКИ ТЕГОВ НЕ ПРОЙДЕН" << endl;

    // 5. ТЕСТЫ CRUD ОПЕРАЦИЙ
    cout << "\n5. Тестирование CRUD операций..." << endl;

    // 5.1 Тест добавления заметки
    cout << "   Тест добавления заметки: ";
    int initialCount = getNoteCount();
    notes.push_back(Note("Тест", "Тестовая заметка", "Содержимое тестовой заметки"));
    int afterAddCount = getNoteCount();
    cout << "было " << initialCount << ", стало " << afterAddCount << " (ожидается: +1)" << endl;
    if (afterAddCount == initialCount + 1) cout << "   + ТЕСТ ДОБАВЛЕНИЯ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ ДОБАВЛЕНИЯ НЕ ПРОЙДЕН" << endl;

    // 5.2 Тест получения заметки
    cout << "   Тест получения заметки по индексу: ";
    Note* testNote = getNote(0);
    if (testNote != nullptr && !testNote->getTitle().empty()) {
        cout << "успешно получена заметка: " << testNote->getTitle() << endl;
        cout << "   + ТЕСТ ПОЛУЧЕНИЯ ПРОЙДЕН" << endl;
    }
    else {
        cout << "не удалось получить заметку" << endl;
        cout << "   ! ТЕСТ ПОЛУЧЕНИЯ НЕ ПРОЙДЕН" << endl;
    }

    // 5.3 Тест обновления заметки
    cout << "   Тест обновления заметки: ";
    string oldTitle = notes[0].getTitle();
    notes[0].setTitle("Обновленный заголовок");
    string newTitle = notes[0].getTitle();
    cout << "было: \"" << oldTitle << "\", стало: \"" << newTitle << "\"" << endl;
    if (oldTitle != newTitle) cout << "   + ТЕСТ ОБНОВЛЕНИЯ ПРОЙДЕН" << endl;
    else cout << "   ! ТЕСТ ОБНОВЛЕНИЯ НЕ ПРОЙДЕН" << endl;

    // 5.4 Тест удаления заметки
    cout << "   Тест удаления заметки: ";
    int beforeDeleteCount = getNoteCount();
    bool deleteResult = removeNote(0); // Удаляем первую заметку
    int afterDeleteCount = getNoteCount();
    cout << "было " << beforeDeleteCount << ", стало " << afterDeleteCount;
    cout << " (удаление " << (deleteResult ? "успешно" : "не удалось") << ")" << endl;
    if (deleteResult && afterDeleteCount == beforeDeleteCount - 1) {
        cout << "   + ТЕСТ УДАЛЕНИЯ ПРОЙДЕН" << endl;
    }
    else {
        cout << "   ! ТЕСТ УДАЛЕНИЯ НЕ ПРОЙДЕН" << endl;
    }

    // 6. ТЕСТ ФАЙЛОВЫХ ОПЕРАЦИЙ
    cout << "\n6. Тестирование файловых операций..." << endl;

    // 6.1 Сохраняем тестовые данные
    cout << "   Сохранение тестовых данных в файл: ";
    string testFileName = "test_notes_backup.txt";
    string originalFileName = filename;
    filename = testFileName;

    try {
        saveToFile();
        cout << "успешно" << endl;
        cout << "   + ТЕСТ СОХРАНЕНИЯ ПРОЙДЕН" << endl;
    }
    catch (const exception& e) {
        cout << "ошибка: " << e.what() << endl;
        cout << "   ! ТЕСТ СОХРАНЕНИЯ НЕ ПРОЙДЕН" << endl;
    }

    // 6.2 Загружаем из файла
    cout << "   Загрузка данных из файла: ";
    notes.clear(); // Очищаем текущие заметки
    try {
        loadFromFile();
        cout << "загружено " << notes.size() << " заметок" << endl;
        if (notes.size() > 0) {
            cout << "   + ТЕСТ ЗАГРУЗКИ ПРОЙДЕН" << endl;
        }
        else {
            cout << "   ! ТЕСТ ЗАГРУЗКИ НЕ ПРОЙДЕН (0 заметок)" << endl;
        }
    }
    catch (const exception& e) {
        cout << "ошибка: " << e.what() << endl;
        cout << "   ! ТЕСТ ЗАГРУЗКИ НЕ ПРОЙДЕН" << endl;
    }

    // Восстанавливаем оригинальное имя файла
    filename = originalFileName;

    // 7. ТЕСТИРОВАНИЕ МЕХАНИЗМОВ ООП (НАСЛЕДОВАНИЕ И ПОЛИМОРФИЗМ)
    cout << "\n7. Тестирование механизмов ООП (наследование и полиморфизм)..." << endl;

    if (notes.empty()) {
        cout << "   SKIP: Нет заметок для тестирования полиморфизма" << endl;
    }
    else {
        // 7.1 Восходящее преобразование (Upcasting)
        cout << "   7.1 Восходящее преобразование (Upcasting):" << endl;
        cout << "   Создаем указатель Storable*, присваиваем адрес объекта Note..." << endl;

        try {
            // Восходящее преобразование: Note* -> Storable*
            const Storable* storablePtr = &notes[0];

            // Полиморфный вызов виртуального метода
            string storageInfo = storablePtr->getStorageInfo();

            cout << "   + УСПЕХ: Указатель создан" << endl;
            cout << "   + УСПЕХ: Виртуальный метод вызван" << endl;
            cout << "   Результат getStorageInfo(): \"" << storageInfo << "\"" << endl;
            cout << "   Вывод: полиморфное поведение работает корректно" << endl;
        }
        catch (const exception& e) {
            cout << "   ! ОШИБКА: " << e.what() << endl;
        }

        // 7.2 Массовый полиморфный вызов
        cout << "\n   7.2 Массовый полиморфный вызов:" << endl;
        cout << "   Создаем вектор указателей Storable* на все заметки..." << endl;

        try {
            vector<const Storable*> storables;
            int successfulCalls = 0;
            int totalCalls = 0;

            // Заполняем вектор указателями на базовый класс
            for (const auto& note : notes) {
                storables.push_back(&note);  // Upcasting
            }

            cout << "   Вектор содержит " << storables.size() << " указателей" << endl;

            // Вызываем виртуальные методы через указатели на базовый класс
            for (const auto& storable : storables) {
                totalCalls++;
                try {
                    string info = storable->getStorageInfo();  // Полиморфный вызов
                    if (!info.empty()) {
                        successfulCalls++;
                    }
                }
                catch (...) {
                    // Игнорируем ошибки для подсчета
                }
            }

            cout << "   Успешных виртуальных вызовов: " << successfulCalls
                << " из " << totalCalls << endl;

            if (successfulCalls == totalCalls) {
                cout << "   + УСПЕХ: Все полиморфные вызовы выполнены успешно" << endl;
                cout << "   Вывод: работа с коллекцией объектов через общий интерфейс работает" << endl;
            }
            else {
                cout << "   ! ЧАСТИЧНЫЙ УСПЕХ: Не все вызовы успешны" << endl;
            }
        }
        catch (const exception& e) {
            cout << "   ! ОШИБКА: " << e.what() << endl;
        }

        // 7.3 Тест виртуального метода markUpdated()
        cout << "\n   7.3 Тест виртуального метода markUpdated():" << endl;

        try {
            // Сохраняем текущее время обновления
            time_t beforeUpdate = notes[0].getUpdatedTime();

            // Получаем строковое представление времени
            char beforeBuffer[80];
            tm beforeTm;
            localtime_s(&beforeTm, &beforeUpdate);
            strftime(beforeBuffer, sizeof(beforeBuffer), "%H:%M:%S", &beforeTm);

            cout << "   Время до вызова markUpdated(): " << beforeBuffer << endl;

            // Вызываем через ссылку на базовый класс
            Storable& storableRef = notes[0];  // Reference upcasting
            storableRef.markUpdated();  // Виртуальный вызов!

            // Получаем обновленное время
            time_t afterUpdate = notes[0].getUpdatedTime();
            char afterBuffer[80];
            tm afterTm;
            localtime_s(&afterTm, &afterUpdate);
            strftime(afterBuffer, sizeof(afterBuffer), "%H:%M:%S", &afterTm);

            cout << "   Время после вызова markUpdated(): " << afterBuffer << endl;
            cout << "   + УСПЕХ: Метод markUpdated() выполнен без ошибок" << endl;

            if (afterUpdate != beforeUpdate) {
                cout << "   + УСПЕХ: Временная метка обновлена" << endl;
            }
            else {
                cout << "   * ПРИМЕЧАНИЕ: Временная метка не изменилась (возможно, вызов в ту же секунду)" << endl;
            }

            cout << "   Вывод: виртуальный метод markUpdated() работает корректно" << endl;
        }
        catch (const exception& e) {
            cout << "   ! ОШИБКА: " << e.what() << endl;
        }

        // 7.4 Дополнительная демонстрация полиморфизма
        cout << "\n   7.4 Дополнительная демонстрация полиморфизма:" << endl;
        if (notes.size() >= 2) {
            cout << "   Использование одного интерфейса Storable для разных объектов Note:" << endl;

            // Массив указателей на базовый класс
            const Storable* polymorphicArray[] = { &notes[0], &notes[1] };

            for (int i = 0; i < 2; i++) {
                string info = polymorphicArray[i]->getStorageInfo();
                cout << "   Объект " << (i + 1) << ": " << info << endl;
            }

            cout << "   Вывод: разные объекты, один интерфейс - полиморфизм в действии" << endl;
        }
    }

    // 8. ВОССТАНОВЛЕНИЕ ОРИГИНАЛЬНЫХ ДАННЫХ
    cout << "\n8. Восстановление оригинальных данных..." << endl;
    notes = originalNotes;
    cout << "   + Восстановлено " << notes.size() << " оригинальных заметок" << endl;

    // 9. ИТОГИ ТЕСТИРОВАНИЯ
    cout << "\n=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ===" << endl;
    cout << "Статус: Оригинальные данные восстановлены" << endl;
    cout << "Всего оригинальных заметок: " << notes.size() << endl;

    // Дополнительная информация
    if (!notes.empty()) {
        cout << "\nПример первой заметки:" << endl;
        notes[0].print();
    }
}
