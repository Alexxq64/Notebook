#include "Note.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

Note::Note() {
    initTime();
}

Note::Note(const std::string& author, const std::string& title, const std::string& content)
    : author(author), title(title), content(content) {
    initTime();
}

std::string Note::getCreatedDate() const {
    // Преобразуем time_t в структуру tm
    tm timeInfo;
    localtime_s(&timeInfo, &createdTime);

    // Форматируем дату в строку
    std::ostringstream oss;
    oss << std::setfill('0')
        << (timeInfo.tm_year + 1900) << "-"
        << std::setw(2) << (timeInfo.tm_mon + 1) << "-"
        << std::setw(2) << timeInfo.tm_mday;

    return oss.str();
}

void Note::initTime() {
    createdTime = time(nullptr);
    updatedTime = createdTime;
}

void Note::updateTime() {
    updatedTime = time(nullptr);
}

void Note::setAuthor(const std::string& newAuthor) {
    author = newAuthor;
    updateTime();
}

void Note::setTitle(const std::string& newTitle) {
    title = newTitle;
    updateTime();
}

void Note::setContent(const std::string& newContent) {
    content = newContent;
    updateTime();
}

void Note::setTags(const std::vector<std::string>& newTags) {
    tags = newTags;
    updateTime();
}

void Note::print() const {
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "Автор: " << author << std::endl;

    // Форматирование времени
    char createdBuffer[80];
    char updatedBuffer[80];

    // Используем безопасные функции для Windows
    tm createdTm;
    tm updatedTm;

    localtime_s(&createdTm, &createdTime);
    localtime_s(&updatedTm, &updatedTime);

    strftime(createdBuffer, sizeof(createdBuffer), "%Y-%m-%d %H:%M:%S", &createdTm);
    strftime(updatedBuffer, sizeof(updatedBuffer), "%Y-%m-%d %H:%M:%S", &updatedTm);

    std::cout << "Создано: " << createdBuffer << std::endl;
    std::cout << "Обновлено: " << updatedBuffer << std::endl;
    std::cout << "Содержимое: " << content << std::endl;

    if (!tags.empty()) {
        std::cout << "Теги: ";
        for (size_t i = 0; i < tags.size(); ++i) {
            std::cout << "#" << tags[i];
            if (i < tags.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

std::string Note::getStorageInfo() const {
    // Минимальная реализация - просто возвращаем заголовок
    return "Storable[Note]: \"" + title + "\" by " + author;
}

void Note::markUpdated() {
    // Просто обновляем время
    updateTime();
}
