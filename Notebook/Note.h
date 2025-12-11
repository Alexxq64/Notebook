// Note.h
#pragma once

#include <string>
#include <vector>
#include "json.hpp"
#include "Storable.h"  // Базовый класс для наследования

// Класс Note представляет отдельную заметку в записной книжке
// Наследует от Storable - демонстрация принципа наследования в ООП
class Note : public Storable {

private:
    // Приватные поля - принцип инкапсуляции
    std::string author;        // Автор заметки
    std::string title;         // Заголовок заметки
    std::string content;       // Текст заметки
    std::string createdAt;     // Дата создания (формат: YYYY-MM-DD)
    std::string updatedAt;     // Дата последнего изменения
    std::vector<std::string> tags;  // Список тегов (STL контейнер)

public:
    // ========== КОНСТРУКТОРЫ ==========

    // Конструктор по умолчанию - создает заметку с текущей датой
    Note();

    // Конструктор с параметрами - создает заметку с заданными данными
    Note(const std::string& author, const std::string& title,
        const std::string& content);

    // ========== ГЕТТЕРЫ (получение данных) ==========

    // Геттеры для доступа к приватным полям
    std::string getAuthor() const { return author; }
    std::string getTitle() const { return title; }
    std::string getContent() const { return content; }
    std::string getCreatedAt() const { return createdAt; }
    std::string getUpdatedAt() const { return updatedAt; }
    std::vector<std::string> getTags() const { return tags; }

    // ========== СЕТТЕРЫ (изменение данных) ==========

    // Установка автора с проверкой на пустую строку
    void setAuthor(const std::string& author);

    // Установка заголовка с проверкой на пустую строку
    void setTitle(const std::string& title);

    // Установка содержимого заметки
    void setContent(const std::string& content);

    // Установка списка тегов
    void setTags(const std::vector<std::string>& tags);

    // ========== ОБНОВЛЕНИЕ ==========

    // Комплексное обновление всех полей заметки
    void update(const std::string& newTitle, const std::string& newContent,
        const std::vector<std::string>& newTags);

    // ========== РАБОТА С JSON (сериализация) ==========

    // Преобразование заметки в JSON объект (для сохранения в файл)
    nlohmann::json toJson() const;

    // Создание заметки из JSON объекта (для загрузки из файла)
    static Note fromJson(const nlohmann::json& j);

    // ========== ПОИСК ==========

    // Поиск слова в заголовке или содержимом (без учета регистра)
    bool containsWord(const std::string& word) const;

    // Проверка наличия тега в списке тегов
    bool hasTag(const std::string& tag) const;

    // ========== ВЫВОД НА ЭКРАН ==========

    // Вывод полной информации о заметке в консоль
    void print() const;

    // ========== ПЕРЕОПРЕДЕЛЕНИЕ ВИРТУАЛЬНЫХ МЕТОДОВ (ПОЛИМОРФИЗМ) ==========

    // Полиморфный метод: возвращает строку с информацией для хранения
    // Вызывается через указатель на базовый класс Storable
    std::string getStorageInfo() const override;

    // Полиморфный метод: обновляет временную метку
    // Вызывается через указатель на базовый класс Storable
    void markUpdated() override;

private:
    // ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

    // Статический метод для получения текущей даты
    // Используется для инициализации полей createdAt и updatedAt
    static std::string getCurrentDate();
};