// Notebook.h
#pragma once

#include "Note.h"
#include <vector>    // Для хранения списка заметок
#include <map>       // Для статистики
#include <string>
#include <algorithm>

// Класс Notebook представляет записную книжку - коллекцию заметок
// Отвечает за управление заметками, поиск, статистику и работу с файлами
class Notebook {
private:
    // Приватные поля - инкапсуляция данных
    std::vector<Note> notes;      // Основной контейнер для хранения заметок (STL vector)
    std::string filename = "notes.json";  // Имя файла для сохранения/загрузки

public:
    // ========== CRUD ОПЕРАЦИИ (основные операции с данными) ==========

    // Добавить новую заметку в записную книжку
    void addNote(const Note& note);

    // Удалить заметку по индексу, возвращает успешность операции
    bool removeNote(int index);

    // Получить указатель на заметку по индексу (для редактирования)
    Note* getNote(int index);

    // Обновить существующую заметку, возвращает успешность операции
    bool updateNote(int index, const Note& updatedNote);

    // ========== ПОИСК И ФИЛЬТРАЦИЯ ==========

    // Найти все заметки указанного автора (регистронезависимый поиск)
    std::vector<int> findByAuthor(const std::string& author) const;

    // Найти все заметки с указанным тегом
    std::vector<int> findByTag(const std::string& tag) const;

    // Найти все заметки, содержащие указанное слово (регистронезависимый)
    std::vector<int> findByWord(const std::string& word) const;

    // Найти все заметки, созданные или обновленные в указанную дату
    std::vector<int> findByDate(const std::string& date) const;

    // Найти все заметки, обновленные за последние N дней
    std::vector<int> findByLastNDays(int days) const;

    // ========== СТАТИСТИКА ==========

    // Получить статистику по авторам: автор -> количество заметок
    std::map<std::string, int> getAuthorStats() const;

    // Получить статистику по тегам: тег -> количество использований
    std::map<std::string, int> getTagStats() const;

    // ========== ФАЙЛОВЫЕ ОПЕРАЦИИ ==========

    // Сохранить все заметки в файл (JSON формат)
    void saveToFile();

    // Загрузить заметки из файла
    void loadFromFile();

    // ========== УТИЛИТЫ ==========

    // Получить количество заметок в записной книжке
    int getNoteCount() const { return (int)notes.size(); }

    // Вывести список всех заметок (краткий формат)
    void printAll() const;

    // Вывести подробную информацию о заметках по указанным индексам
    void printNotes(const std::vector<int>& indices) const;

    // ========== ТЕСТОВЫЕ СЦЕНАРИИ ==========

    // Запуск всех тестов: поиск, статистика, файлы, полиморфизм
    void runTestScenarios();

    // ========== ДЕМОНСТРАЦИЯ ПОЛИМОРФИЗМА ==========

    // Демонстрация полиморфизма: вывод информации через указатели на базовый класс
    void printAllStorageInfo() const;

    // Расширенная демонстрация полиморфизма с преобразованием типов
    void demonstratePolymorphism() const;

private:
    // ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

    // Преобразовать строку к нижнему регистру (для регистронезависимого поиска)
    static std::string toLower(const std::string& str);

    // Загрузить заметки из JSON объекта (внутренний метод)
};