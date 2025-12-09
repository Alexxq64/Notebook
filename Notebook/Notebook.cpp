#include "Notebook.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

// CRUD операции
void Notebook::addNote(const Note& note) {
    notes.push_back(note);
}

bool Notebook::removeNote(int index) {
    if (index < 0 || index >= notes.size()) return false;
    notes.erase(notes.begin() + index);
    return true;
}

Note* Notebook::getNote(int index) {
    if (index < 0 || index >= notes.size()) return nullptr;
    return &notes[index];
}

bool Notebook::updateNote(int index, const Note& updatedNote) {
    if (index < 0 || index >= notes.size()) return false;
    notes[index] = updatedNote;
    return true;
}

// Поиск
vector<int> Notebook::findByAuthor(const string& author) const {
    vector<int> result;
    string lowerAuthor = toLower(author);

    for (int i = 0; i < notes.size(); i++) {
        if (toLower(notes[i].getAuthor()) == lowerAuthor) {
            result.push_back(i);
        }
    }
    return result;
}

vector<int> Notebook::findByTag(const string& tag) const {
    vector<int> result;
    for (int i = 0; i < notes.size(); i++) {
        if (notes[i].hasTag(tag)) {
            result.push_back(i);
        }
    }
    return result;
}

vector<int> Notebook::findByWord(const string& word) const {
    vector<int> result;
    for (int i = 0; i < notes.size(); i++) {
        if (notes[i].containsWord(word)) {
            result.push_back(i);
        }
    }
    return result;
}

vector<int> Notebook::findByDate(const string& date) const {
    vector<int> result;
    for (int i = 0; i < notes.size(); i++) {
        if (notes[i].getCreatedAt() == date || notes[i].getUpdatedAt() == date) {
            result.push_back(i);
        }
    }
    return result;
}

vector<int> Notebook::findByLastNDays(int days) const {
    vector<int> result;
    time_t now = time(nullptr);
    now -= days * 24 * 60 * 60; // Вычитаем N дней

    tm timeInfo;
    localtime_s(&timeInfo, &now);

    stringstream ss;
    ss << put_time(&timeInfo, "%Y-%m-%d");
    string targetDate = ss.str();

    for (int i = 0; i < notes.size(); i++) {
        if (notes[i].getUpdatedAt() >= targetDate) {
            result.push_back(i);
        }
    }
    return result;
}

// Статистика
map<string, int> Notebook::getAuthorStats() const {
    map<string, int> stats;
    for (const auto& note : notes) {
        stats[note.getAuthor()]++;
    }
    return stats;
}

map<string, int> Notebook::getTagStats() const {
    map<string, int> stats;
    for (const auto& note : notes) {
        for (const auto& tag : note.getTags()) {
            stats[tag]++;
        }
    }
    return stats;
}

// Файловые операции
void Notebook::saveToFile() {
    nlohmann::json j;
    j["notes"] = nlohmann::json::array();

    for (const auto& note : notes) {
        j["notes"].push_back(note.toJson());
    }

    ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(2);
        file.close();
        cout << "Saved " << notes.size() << " notes to " << filename << endl;
    }
    else {
        throw runtime_error("Cannot open file for writing: " + filename);
    }
}

void Notebook::loadFromFile() {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "File not found, starting with empty notebook." << endl;
        return;
    }

    try {
        nlohmann::json j;
        file >> j;

        notes.clear();
        if (j.contains("notes") && j["notes"].is_array()) {
            for (const auto& noteJson : j["notes"]) {
                notes.push_back(Note::fromJson(noteJson));
            }
        }
        cout << "Loaded " << notes.size() << " notes from " << filename << endl;
    }
    catch (const exception& e) {
        throw runtime_error("Error loading JSON: " + string(e.what()));
    }
}

// Утилиты
void Notebook::printAll() const {
    cout << "\n=== All Notes (" << notes.size() << ") ===" << endl;
    for (int i = 0; i < notes.size(); i++) {
        cout << "[" << i << "] " << notes[i].getTitle()
            << " by " << notes[i].getAuthor() << endl;
    }
}

void Notebook::printNotes(const vector<int>& indices) const {
    if (indices.empty()) {
        cout << "No notes found." << endl;
        return;
    }

    cout << "\n=== Found " << indices.size() << " notes ===" << endl;
    for (int idx : indices) {
        notes[idx].print();
    }
}

// Тестовые сценарии
void Notebook::runTestScenarios() {
    cout << "\n=== Running Test Scenarios ===" << endl;

    // Сохраняем текущие заметки
    vector<Note> backup = notes;
    notes.clear();

    // 1. Создаем тестовые записи
    cout << "1. Creating test notes..." << endl;
    notes.push_back(Note("Kate", "Shopping List", "Milk, Eggs, Bread"));
    notes.back().setTags({ "shopping", "home" });

    notes.push_back(Note("Bob", "Meeting Notes", "Discuss project timeline"));
    notes.back().setTags({ "work", "meeting" });

    notes.push_back(Note("Kate", "Book Ideas", "Write a novel about programming"));
    notes.back().setTags({ "personal", "ideas" });

    // 2. Поиск по автору
    cout << "2. Searching by author 'Kate': ";
    auto aliceNotes = findByAuthor("Kate");
    cout << aliceNotes.size() << " notes found" << endl;

    // 3. Поиск по тегу
    cout << "3. Searching by tag 'work': ";
    auto workNotes = findByTag("work");
    cout << workNotes.size() << " notes found" << endl;

    // 4. Поиск по слову
    cout << "4. Searching by word 'project': ";
    auto projectNotes = findByWord("project");
    cout << projectNotes.size() << " notes found" << endl;

    // 5. Статистика
    cout << "5. Statistics:" << endl;
    auto authorStats = getAuthorStats();
    for (const auto& pair : authorStats) {
        cout << "   " << pair.first << ": " << pair.second << " notes" << endl;
    }

    // 6. Сохранение/загрузка
    cout << "6. Testing save/load..." << endl;
    saveToFile();
    notes.clear();
    loadFromFile();
    cout << "   Loaded " << notes.size() << " notes" << endl;

    // Восстанавливаем оригинальные данные
    notes = backup;
    cout << "\n=== Test Scenarios Completed ===" << endl;
}

// Вспомогательные методы
string Notebook::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}