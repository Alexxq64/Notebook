#pragma once
#pragma once
#include "Note.h"
#include <vector>
#include <map>
#include <string>
#include <algorithm>

class Notebook {
private:
    std::vector<Note> notes;
    std::string filename = "notes.json";

public:
    // CRUD операции
    void addNote(const Note& note);
    bool removeNote(int index);
    Note* getNote(int index);
    bool updateNote(int index, const Note& updatedNote);

    // Поиск
    std::vector<int> findByAuthor(const std::string& author) const;
    std::vector<int> findByTag(const std::string& tag) const;
    std::vector<int> findByWord(const std::string& word) const;
    std::vector<int> findByDate(const std::string& date) const;
    std::vector<int> findByLastNDays(int days) const;

    // Статистика
    std::map<std::string, int> getAuthorStats() const;
    std::map<std::string, int> getTagStats() const;

    // Файловые операции
    void saveToFile();
    void loadFromFile();

    // Утилиты
    int getNoteCount() const { return (int)notes.size(); }
    void printAll() const;
    void printNotes(const std::vector<int>& indices) const;

    // Тестовые сценарии
    void runTestScenarios();

private:
    static std::string toLower(const std::string& str);
    std::vector<Note> loadNotesFromJson(const nlohmann::json& j);
};