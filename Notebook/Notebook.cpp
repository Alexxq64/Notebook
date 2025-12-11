#include "Notebook.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <typeinfo>  // Для typeid и dynamic_cast

using namespace std;

// ========== CRUD ОПЕРАЦИИ ==========

// Добавить заметку в записную книжку
void Notebook::addNote(const Note& note) {
    notes.push_back(note);  // Добавляем копию заметки в вектор
}

// Удалить заметку по индексу
bool Notebook::removeNote(int index) {
    // Проверяем валидность индекса
    if (index < 0 || index >= notes.size()) return false;

    // Удаляем элемент из вектора
    notes.erase(notes.begin() + index);
    return true;  // Успешное удаление
}

// Получить указатель на заметку по индексу
Note* Notebook::getNote(int index) {
    // Проверяем валидность индекса
    if (index < 0 || index >= notes.size()) return nullptr;

    // Возвращаем указатель на элемент вектора
    return &notes[index];
}

// Обновить существующую заметку
bool Notebook::updateNote(int index, const Note& updatedNote) {
    // Проверяем валидность индекса
    if (index < 0 || index >= notes.size()) return false;

    // Заменяем заметку
    notes[index] = updatedNote;
    return true;  // Успешное обновление
}

// ========== МЕТОДЫ ПОИСКА ==========

// Поиск заметок по автору (регистронезависимый)
vector<int> Notebook::findByAuthor(const string& author) const {
    vector<int> result;  // Вектор для хранения индексов найденных заметок

    // Приводим искомое имя автора к нижнему регистру
    string lowerAuthor = toLower(author);

    // Проходим по всем заметкам
    for (int i = 0; i < notes.size(); i++) {
        // Сравниваем имена авторов (без учета регистра)
        if (toLower(notes[i].getAuthor()) == lowerAuthor) {
            result.push_back(i);  // Добавляем индекс найденной заметки
        }
    }
    return result;
}

// Поиск заметок по тегу
vector<int> Notebook::findByTag(const string& tag) const {
    vector<int> result;

    // Проходим по всем заметкам
    for (int i = 0; i < notes.size(); i++) {
        // Используем метод hasTag класса Note
        if (notes[i].hasTag(tag)) {
            result.push_back(i);  // Добавляем индекс заметки с нужным тегом
        }
    }
    return result;
}

// Поиск заметок по слову в заголовке или содержимом
vector<int> Notebook::findByWord(const string& word) const {
    vector<int> result;

    // Проходим по всем заметкам
    for (int i = 0; i < notes.size(); i++) {
        // Используем метод containsWord класса Note
        if (notes[i].containsWord(word)) {
            result.push_back(i);  // Добавляем индекс заметки с искомым словом
        }
    }
    return result;
}

// Поиск заметок по дате создания или обновления
vector<int> Notebook::findByDate(const string& date) const {
    vector<int> result;

    // Проходим по всем заметкам
    for (int i = 0; i < notes.size(); i++) {
        // Ищем совпадение с датой создания ИЛИ датой обновления
        if (notes[i].getCreatedAt() == date || notes[i].getUpdatedAt() == date) {
            result.push_back(i);
        }
    }
    return result;
}

// Поиск заметок, обновленных за последние N дней
vector<int> Notebook::findByLastNDays(int days) const {
    vector<int> result;

    // Вычисляем дату, которая была N дней назад
    time_t now = time(nullptr);  // Текущее время
    now -= days * 24 * 60 * 60;  // Вычитаем N дней в секундах

    // Конвертируем в структуру tm
    tm timeInfo;
    localtime_s(&timeInfo, &now);

    // Форматируем дату в строку
    stringstream ss;
    ss << put_time(&timeInfo, "%Y-%m-%d");
    string targetDate = ss.str();  // Дата N дней назад

    // Проходим по всем заметкам
    for (int i = 0; i < notes.size(); i++) {
        // Сравниваем даты как строки (формат YYYY-MM-DD позволяет лексикографическое сравнение)
        if (notes[i].getUpdatedAt() >= targetDate) {
            result.push_back(i);  // Заметка обновлена не раньше targetDate
        }
    }
    return result;
}

// ========== СТАТИСТИКА ==========

// Статистика по авторам: сколько заметок у каждого автора
map<string, int> Notebook::getAuthorStats() const {
    map<string, int> stats;  // STL map: автор -> количество заметок

    for (const auto& note : notes) {
        stats[note.getAuthor()]++;  // Увеличиваем счетчик для данного автора
    }
    return stats;
}

// Статистика по тегам: сколько раз используется каждый тег
map<string, int> Notebook::getTagStats() const {
    map<string, int> stats;  // STL map: тег -> количество использований

    for (const auto& note : notes) {
        for (const auto& tag : note.getTags()) {
            stats[tag]++;  // Увеличиваем счетчик для каждого тега
        }
    }
    return stats;
}

// ========== ФАЙЛОВЫЕ ОПЕРАЦИИ ==========

// Сохранение всех заметок в JSON файл
void Notebook::saveToFile() {
    nlohmann::json j;  // Создаем корневой JSON объект
    j["notes"] = nlohmann::json::array();  // Создаем массив для заметок

    // Конвертируем каждую заметку в JSON и добавляем в массив
    for (const auto& note : notes) {
        j["notes"].push_back(note.toJson());
    }

    // Открываем файл для записи
    ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(2);  // Записываем JSON с отступами (2 пробела)
        file.close();
        cout << "Saved " << notes.size() << " notes to " << filename << endl;
    }
    else {
        throw runtime_error("Cannot open file for writing: " + filename);
    }
}

// Загрузка заметок из JSON файла
void Notebook::loadFromFile() {
    // Открываем файл для чтения
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "File not found, starting with empty notebook." << endl;
        return;
    }

    try {
        nlohmann::json j;
        file >> j;  // Читаем весь JSON из файла

        notes.clear();  // Очищаем текущий список заметок

        // Проверяем наличие массива заметок в JSON
        if (j.contains("notes") && j["notes"].is_array()) {
            // Проходим по всем элементам массива
            for (const auto& noteJson : j["notes"]) {
                // Восстанавливаем заметку из JSON
                notes.push_back(Note::fromJson(noteJson));
            }
        }
        cout << "Loaded " << notes.size() << " notes from " << filename << endl;
    }
    catch (const exception& e) {
        throw runtime_error("Error loading JSON: " + string(e.what()));
    }
}

// ========== УТИЛИТЫ ==========

// Вывод краткой информации обо всех заметках
void Notebook::printAll() const {
    cout << "\n=== All Notes (" << notes.size() << ") ===" << endl;

    // Проходим по всем заметкам
    for (size_t i = 0; i < notes.size(); i++) {
        // Выводим номер, заголовок и автора
        cout << "[" << i + 1 << "] " << notes[i].getTitle()
            << " by " << notes[i].getAuthor() << endl;
    }
}

// Вывод подробной информации о заметках по указанным индексам
void Notebook::printNotes(const vector<int>& indices) const {
    // Проверяем, есть ли что выводить
    if (indices.empty()) {
        cout << "No notes found." << endl;
        return;
    }

    cout << "\n=== Found " << indices.size() << " notes ===" << endl;

    // Проходим по всем индексам
    for (int idx : indices) {
        // Вызываем метод print() для каждой заметки
        notes[idx].print();
    }
}

// ========== ТЕСТОВЫЕ СЦЕНАРИИ ==========

// Полный набор тестов для проверки функциональности
void Notebook::runTestScenarios() {
    cout << "\n=== Running Test Scenarios ===" << endl;

    // 1. Сохраняем текущие данные перед тестами
    cout << "1. Saving current data to file..." << endl;
    try {
        saveToFile();
        cout << "   + SAVE: " << notes.size() << " notes saved to file" << endl;
    }
    catch (const exception& e) {
        cout << "   * SAVE FAILED: " << e.what() << endl;
    }

    // 2. Очищаем и заполняем тестовыми данными
    cout << "\n2. Creating test notes..." << endl;
    notes.clear();  // Очищаем все существующие заметки

    // Создаем тестовые заметки
    notes.push_back(Note("Kate", "Shopping List", "Milk, Eggs, Bread"));
    notes.back().setTags({ "shopping", "home" });

    notes.push_back(Note("Bob", "Meeting Notes", "Discuss project timeline"));
    notes.back().setTags({ "work", "meeting" });

    notes.push_back(Note("Kate", "Book Ideas", "Write a novel about programming"));
    notes.back().setTags({ "personal", "ideas" });

    // 3. ТЕСТЫ ПОИСКА
    cout << "\n3. Testing search functions..." << endl;

    // 3.1 Поиск по автору
    cout << "   Search by author 'Kate': ";
    auto res1 = findByAuthor("Kate");
    cout << res1.size() << " found (expected: 2)" << endl;

    // 3.2 Поиск по тегу
    cout << "   Search by tag 'work': ";
    auto res2 = findByTag("work");
    cout << res2.size() << " found (expected: 1)" << endl;

    // 3.3 Поиск по слову
    cout << "   Search by word 'project': ";
    auto res3 = findByWord("project");
    cout << res3.size() << " found (expected: 1)" << endl;

    // 3.4 Поиск по дате
    cout << "   Search by date (today): ";
    string today = notes[0].getCreatedAt();
    auto res4 = findByDate(today);
    cout << res4.size() << " found (expected: 3)" << endl;

    // 4. ТЕСТЫ СТАТИСТИКИ
    cout << "\n4. Testing statistics..." << endl;

    // 4.1 Статистика по авторам
    cout << "   Author statistics:" << endl;
    auto authStats = getAuthorStats();
    for (auto& p : authStats) {
        cout << "     " << p.first << ": " << p.second << " notes" << endl;
    }
    cout << "     Expected: Kate:2, Bob:1" << endl;

    // 4.2 Статистика по тегам
    cout << "   Tag statistics:" << endl;
    auto tagStats = getTagStats();
    for (auto& p : tagStats) {
        cout << "     #" << p.first << ": " << p.second << " times" << endl;
    }
    cout << "     Expected: shopping:1, home:1, work:1, meeting:1, personal:1, ideas:1" << endl;

    // 5. ТЕСТ НАСЛЕДОВАНИЯ И ПОЛИМОРФИЗМА
    cout << "\n5. Testing inheritance and polymorphism..." << endl;

    if (notes.empty()) {
        cout << "   SKIP: No notes available for testing" << endl;
    }
    else {
        // 5.1 Тест восходящего преобразования (upcasting)
        cout << "   Testing upcasting (Note* -> Storable*): ";
        try {
            const Storable* storable = &notes[0];  // Восходящее преобразование
            string info = storable->getStorageInfo();
            cout << "PASS" << endl;
            cout << "     Virtual call result: " << info << endl;
        }
        catch (const exception& e) {
            cout << "FAIL: " << e.what() << endl;
        }

        // 5.2 Тест полиморфного вызова через базовый класс
        cout << "   Testing polymorphic calls via Storable*: ";
        try {
            vector<const Storable*> storables;
            int successfulCalls = 0;

            for (const auto& note : notes) {
                storables.push_back(&note);  // Заполняем указателями на базовый класс
            }

            for (const auto& storable : storables) {
                string info = storable->getStorageInfo();
                if (!info.empty()) successfulCalls++;
            }

            cout << successfulCalls << "/" << storables.size()
                << " successful virtual calls" << endl;
        }
        catch (const exception& e) {
            cout << "FAIL: " << e.what() << endl;
        }

        // 5.3 Тест метода markUpdated()
        cout << "   Testing virtual method markUpdated(): ";
        try {
            // Сохраняем текущую дату
            string beforeUpdate = notes[0].getUpdatedAt();

            // Вызываем через базовый класс
            Storable& storable = notes[0];
            storable.markUpdated();  // Должно обновить updatedAt

            string afterUpdate = notes[0].getUpdatedAt();

            // Просто проверяем, что метод выполнился без ошибок
            cout << "PASS (method executed successfully)" << endl;
            cout << "     Before: " << beforeUpdate << endl;
            cout << "     After:  " << afterUpdate << endl;

            // Дополнительная проверка для демонстрации
            if (afterUpdate != beforeUpdate) {
                cout << "     + Timestamp was updated" << endl;
            }
            else {
                cout << "     * Timestamp unchanged (same day)" << endl;
            }
        }
        catch (const exception& e) {
            cout << "FAIL: " << e.what() << endl;
        }
    }

    // 6. ТЕСТ ЗАГРУЗКИ ИЗ ФАЙЛА
    cout << "\n6. Testing file load..." << endl;
    notes.clear();  // Очищаем тестовые данные
    try {
        loadFromFile();  // Загружаем из файла
        cout << "   + LOAD: " << notes.size() << " notes loaded from file" << endl;
    }
    catch (const exception& e) {
        cout << "   * LOAD FAILED: " << e.what() << endl;
    }

    cout << "\n=== All Tests Completed ===" << endl;
}

// ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

// Преобразование строки к нижнему регистру
string Notebook::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ========== ДЕМОНСТРАЦИЯ ПОЛИМОРФИЗМА ==========

// Демонстрация полиморфизма: вывод информации через указатели на базовый класс
void Notebook::printAllStorageInfo() const {
    std::cout << "\n=== Storage Information (Polymorphism Demo) ===" << std::endl;

    for (size_t i = 0; i < notes.size(); i++) {
        // Восходящее преобразование: Note* -> Storable*
        const Storable* storable = &notes[i];

        // ПОЛИМОРФНЫЙ ВЫЗОВ: вызывается Note::getStorageInfo(), 
        // хотя тип указателя Storable*
        std::cout << i + 1 << ". " << storable->getStorageInfo() << std::endl;
    }
}

// Расширенная демонстрация полиморфизма
void Notebook::demonstratePolymorphism() const {
    std::cout << "\n=== Advanced Polymorphism Demo ===" << std::endl;

    // Создаем вектор указателей на базовый класс
    std::vector<const Storable*> storables;

    // Заполняем вектор указателями на Note (восходящее преобразование)
    for (const auto& note : notes) {
        storables.push_back(&note);  // Note* -> Storable*
    }

    // Вызываем виртуальные методы через указатели на базовый класс
    for (size_t i = 0; i < storables.size(); i++) {
        std::cout << "Element " << i + 1 << ":" << std::endl;

        // Полиморфный вызов виртуального метода
        std::cout << "  " << storables[i]->getStorageInfo() << std::endl;

        // Демонстрация динамического приведения типов
        const Note* notePtr = dynamic_cast<const Note*>(storables[i]);
        if (notePtr) {
            // Успешное нисходящее преобразование
            std::cout << "  (Successfully cast back to Note*)" << std::endl;
            std::cout << "  Author: " << notePtr->getAuthor() << std::endl;
        }
    }
}