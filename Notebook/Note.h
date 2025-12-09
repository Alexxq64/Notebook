#pragma once
#include <string>
#include <vector>
#include "json.hpp"

class Note {
private:
    std::string author;
    std::string title;
    std::string content;
    std::string createdAt;
    std::string updatedAt;
    std::vector<std::string> tags;

public:
    // Конструкторы
    Note();
    Note(const std::string& author, const std::string& title,
        const std::string& content);

    // Геттеры
    std::string getAuthor() const { return author; }
    std::string getTitle() const { return title; }
    std::string getContent() const { return content; }
    std::string getCreatedAt() const { return createdAt; }
    std::string getUpdatedAt() const { return updatedAt; }
    std::vector<std::string> getTags() const { return tags; }

    // Сеттеры
    void setAuthor(const std::string& author);
    void setTitle(const std::string& title);
    void setContent(const std::string& content);
    void setTags(const std::vector<std::string>& tags);

    // Обновление
    void update(const std::string& newTitle, const std::string& newContent,
        const std::vector<std::string>& newTags);

    // JSON
    nlohmann::json toJson() const;
    static Note fromJson(const nlohmann::json& j);

    // Поиск
    bool containsWord(const std::string& word) const;
    bool hasTag(const std::string& tag) const;

    // Вывод
    void print() const;

private:
    static std::string getCurrentDate();
};