#pragma once
#include <string>
#include <vector>
#include <ctime>
#include "Storable.h"

class Note : public Storable {
private:
    std::string author;
    std::string title;
    std::string content;
    std::vector<std::string> tags;
    time_t createdTime;
    time_t updatedTime;

public:
    // Конструкторы
    Note();
    Note(const std::string& author, const std::string& title, const std::string& content);

    // Геттеры
    std::string getAuthor() const { return author; }
    std::string getTitle() const { return title; }
    std::string getContent() const { return content; }
    std::vector<std::string> getTags() const { return tags; }
    time_t getCreatedTime() const { return createdTime; }
    time_t getUpdatedTime() const { return updatedTime; }
    std::string getCreatedDate() const;  // Возвращает дату в формате "ГГГГ-ММ-ДД"

    // Сеттеры
    void setAuthor(const std::string& newAuthor);
    void setTitle(const std::string& newTitle);
    void setContent(const std::string& newContent);
    void setTags(const std::vector<std::string>& newTags);
    void setCreatedTime(time_t time) { createdTime = time; }
    void setUpdatedTime(time_t time) { updatedTime = time; }

    // Утилиты
    void print() const;

    std::string getStorageInfo() const override;  // Реализация полиморфного метода
    void markUpdated() override;  // Реализация виртуального метода

private:
    void updateTime();
    void initTime();
};