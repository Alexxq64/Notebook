#include "Note.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std;

// Конструктор по умолчанию: создает пустую заметку с текущей датой
Note::Note() {
    createdAt = getCurrentDate();  // Устанавливаем дату создания
    updatedAt = createdAt;         // При создании дата обновления = дате создания
}

// Конструктор с параметрами: создает заметку с заданными данными
Note::Note(const string& author, const string& title, const string& content)
    : author(author), title(title), content(content) {  // Инициализируем поля
    createdAt = getCurrentDate();  // Устанавливаем дату создания
    updatedAt = createdAt;         // При создании дата обновления = дате создания
}

// Установка автора заметки
void Note::setAuthor(const string& author) {
    if (author.empty()) throw invalid_argument("Author cannot be empty");  // Проверка на пустую строку
    this->author = author;      // Присваиваем нового автора
    updatedAt = getCurrentDate(); // Обновляем дату изменения
}

// Установка заголовка заметки
void Note::setTitle(const string& title) {
    if (title.empty()) throw invalid_argument("Title cannot be empty");  // Проверка на пустую строку
    this->title = title;        // Присваиваем новый заголовок
    updatedAt = getCurrentDate(); // Обновляем дату изменения
}

// Установка содержимого заметки
void Note::setContent(const string& content) {
    this->content = content;    // Присваиваем новое содержимое
    updatedAt = getCurrentDate(); // Обновляем дату изменения
}

// Установка тегов заметки
void Note::setTags(const vector<string>& tags) {
    this->tags = tags;          // Присваиваем новый список тегов
    updatedAt = getCurrentDate(); // Обновляем дату изменения
}

// Комплексное обновление заметки: заголовка, содержимого и тегов
void Note::update(const string& newTitle, const string& newContent,
    const vector<string>& newTags) {
    setTitle(newTitle);     // Обновляем заголовок
    setContent(newContent); // Обновляем содержимое
    setTags(newTags);       // Обновляем теги
    // Каждый сеттер сам обновит updatedAt
}

// Преобразование заметки в JSON формат (сериализация)
nlohmann::json Note::toJson() const {
    nlohmann::json j;           // Создаем JSON объект

    // Заполняем все поля заметки
    j["author"] = author;       // Автор
    j["title"] = title;         // Заголовок
    j["content"] = content;     // Содержимое
    j["createdAt"] = createdAt; // Дата создания
    j["updatedAt"] = updatedAt; // Дата обновления
    j["tags"] = tags;           // Список тегов (вектор автоматически конвертируется)

    return j;                   // Возвращаем JSON объект
}

// Создание заметки из JSON объекта (десериализация)
Note Note::fromJson(const nlohmann::json& j) {
    Note note;  // Создаем новую заметку

    // Извлекаем данные из JSON, используя значения по умолчанию
    note.author = j.value("author", "");       // Автор (по умолчанию: пустая строка)
    note.title = j.value("title", "");         // Заголовок
    note.content = j.value("content", "");     // Содержимое
    note.createdAt = j.value("createdAt", ""); // Дата создания
    note.updatedAt = j.value("updatedAt", ""); // Дата обновления

    // Обрабатываем теги (если они есть в JSON)
    if (j.contains("tags") && j["tags"].is_array()) {
        // Проходим по всем тегам в массиве
        for (const auto& tag : j["tags"]) {
            note.tags.push_back(tag.get<string>());  // Добавляем тег в вектор
        }
    }

    return note;  // Возвращаем восстановленную заметку
}

// Поиск слова в заголовке или содержимом (без учета регистра)
bool Note::containsWord(const string& word) const {
    // Приводим искомое слово к нижнему регистру
    string lowerWord = word;
    transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    // Приводим заголовок к нижнему регистру
    string lowerTitle = title;
    transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);

    // Приводим содержимое к нижнему регистру
    string lowerContent = content;
    transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);

    // Ищем слово в заголовке ИЛИ содержимом
    return lowerTitle.find(lowerWord) != string::npos ||
        lowerContent.find(lowerWord) != string::npos;
}

// Проверка наличия тега в списке тегов
bool Note::hasTag(const string& tag) const {
    // Ищем точное совпадение тега в векторе тегов
    return find(tags.begin(), tags.end(), tag) != tags.end();
}

// Вывод информации о заметке в консоль
void Note::print() const {
    cout << "\n=== " << title << " ===" << endl;  // Заголовок с рамкой
    cout << "Author: " << author << endl;         // Автор
    cout << "Created: " << createdAt << endl;     // Дата создания
    cout << "Updated: " << updatedAt << endl;     // Дата обновления

    // Вывод тегов
    cout << "Tags: ";
    for (const auto& tag : tags) {
        cout << "#" << tag << " ";  // Каждый тег с решеткой
    }

    cout << "\nContent: " << content << endl;     // Содержимое заметки
}

// Вспомогательный метод: получение текущей даты в формате YYYY-MM-DD
string Note::getCurrentDate() {
    time_t now = time(nullptr);      // Получаем текущее время
    tm localTime;                    // Структура для локального времени
    localtime_s(&localTime, &now);   // Конвертируем в локальное время

    // Форматируем дату в строку
    stringstream ss;
    ss << put_time(&localTime, "%Y-%m-%d");  // Формат: год-месяц-день
    return ss.str();  // Возвращаем строку с датой
}

// ПОЛИМОРФИЗМ: переопределение метода из базового класса Storable
// Возвращает строку с информацией о заметке для хранения
std::string Note::getStorageInfo() const {
    // Форматируем информацию: заголовок, автор и дата обновления
    return "Note: \"" + title + "\" by " + author + " (Updated: " + updatedAt + ")";
}

// ПОЛИМОРФИЗМ: переопределение метода из базового класса Storable
// Обновляет временную метку заметки
void Note::markUpdated() {
    Storable::markUpdated();   // Вызываем базовую реализацию (если есть)
    updatedAt = getCurrentDate();  // Обновляем дату изменения текущей датой
}