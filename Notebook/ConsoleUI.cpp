#include "ConsoleUI.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cstdlib>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

using namespace std;

void ConsoleUI::run() {
    clearScreen();
    cout << "=== NOTEBOOK APPLICATION ===" << endl;

    // Автозагрузка при запуске
    try {
        notebook.loadFromFile();
    }
    catch (const exception& e) {
        cout << "Warning: " << e.what() << endl;
    }

    while (true) {
        showMainMenu();
    }
}

void ConsoleUI::showMainMenu() {
    clearScreen();
    cout << "=== MAIN MENU ===" << endl;
    cout << "Total notes: " << notebook.getNoteCount() << endl;
    if (unsavedChanges) cout << "(!) Unsaved changes" << endl;
    cout << "=================" << endl;
    cout << "1. Create new note" << endl;
    cout << "2. List all notes" << endl;
    cout << "3. View/edit note" << endl;
    cout << "4. Delete note" << endl;
    cout << "5. Search notes" << endl;
    cout << "6. Statistics" << endl;
    cout << "7. Save to file" << endl;
    cout << "8. Load from file" << endl;
    cout << "9. Test scenarios" << endl;
    cout << "0. Exit" << endl;
    cout << "=================" << endl;

    int choice = getChoice(0, 9);

    switch (choice) {
    case 1: handleCreateNote(); break;
    case 2: handleListNotes(); break;
    case 3: handleViewNote(); break;
    case 4: handleDeleteNote(); break;
    case 5: handleSearch(); break;
    case 6: handleStatistics(); break;
    case 7: handleSave(); break;
    case 8: handleLoad(); break;
    case 9: handleTestScenarios(); break;
    case 0:
        if (unsavedChanges) {
            cout << "You have unsaved changes. Save before exit? (y/n): ";
            char c;
            cin >> c;
            if (c == 'y' || c == 'Y') {
                handleSave();
            }
        }
        cout << "Goodbye!" << endl;
        exit(0);
    }
}

void ConsoleUI::handleCreateNote() {
    clearScreen();
    cout << "=== CREATE NEW NOTE ===" << endl;

    string author = getString("Enter author: ");
    string title = getString("Enter title: ");
    string content = getString("Enter content: ");
    vector<string> tags = getTags();

    Note newNote(author, title, content);
    newNote.setTags(tags);

    notebook.addNote(newNote);
    unsavedChanges = true;

    cout << "\nNote created successfully!" << endl;
    pressAnyKey();
}

void ConsoleUI::handleListNotes() {
    clearScreen();
    cout << "=== ALL NOTES ===" << endl;
    notebook.printAll();
    pressAnyKey();
}

void ConsoleUI::handleViewNote() {
    clearScreen();
    notebook.printAll();

    if (notebook.getNoteCount() == 0) {
        pressAnyKey();
        return;
    }

    // Запрашиваем номер заметки, теперь от 1 до количества заметок
    int noteNumber = getInt("Enter note number to view (0 to cancel): ", 0, notebook.getNoteCount());
    if (noteNumber == 0) return;

    // Конвертируем номер заметки в индекс (номер-1)
    int index = noteNumber - 1;

    Note* note = notebook.getNote(index);
    if (note) {
        note->print();

        cout << "\nOptions:" << endl;
        cout << "1. Edit this note" << endl;
        cout << "2. Back to menu" << endl;

        int choice = getChoice(1, 2);
        if (choice == 1) {
            handleEditNote(index);  // Передаем индекс выбранной заметки
        }
    }
}

void ConsoleUI::handleEditNote(int index) {
    if (index < 0 || index >= notebook.getNoteCount()) {
        cout << "Invalid note index." << endl;
        pressAnyKey();
        return;
    }

    Note* note = notebook.getNote(index);
    if (!note) {
        cout << "Note not found." << endl;
        pressAnyKey();
        return;
    }

    clearScreen();
    cout << "=== Editing Note ===" << endl;
    note->print();

    cout << "\nWhat do you want to edit?" << endl;
    cout << "1. Title" << endl;
    cout << "2. Content" << endl;
    cout << "3. Tags" << endl;
    cout << "4. All fields" << endl;
    cout << "0. Cancel" << endl;

    int choice = getChoice(0, 4);
    if (choice == 0) return;

    try {
        switch (choice) {
        case 1: {
            string newTitle = getString("New title: ");
            note->setTitle(newTitle);
            break;
        }
        case 2: {
            string newContent = getString("New content: ");
            note->setContent(newContent);
            break;
        }
        case 3: {
            vector<string> newTags = getTags();
            note->setTags(newTags);
            break;
        }
        case 4: {
            string newTitle = getString("New title: ");
            string newContent = getString("New content: ");
            vector<string> newTags = getTags();

            note->setTitle(newTitle);
            note->setContent(newContent);
            note->setTags(newTags);
            break;
        }
        }

        unsavedChanges = true;
        cout << "\nNote updated successfully!" << endl;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    pressAnyKey();
}

void ConsoleUI::handleDeleteNote() {
    clearScreen();
    notebook.printAll();

    if (notebook.getNoteCount() == 0) {
        pressAnyKey();
        return;
    }

    int index = getInt("Enter note number to delete (-1 to cancel): ", -1, notebook.getNoteCount() - 1);
    if (index == -1) return;

    cout << "Are you sure? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (confirm == 'y' || confirm == 'Y') {
        if (notebook.removeNote(index)) {
            unsavedChanges = true;
            cout << "Note deleted." << endl;
        }
        else {
            cout << "Error deleting note." << endl;
        }
    }
    pressAnyKey();
}

void ConsoleUI::showSearchMenu() {
    clearScreen();
    cout << "=== SEARCH NOTES ===" << endl;
    cout << "1. By author" << endl;
    cout << "2. By tag" << endl;
    cout << "3. By word" << endl;
    cout << "4. By date" << endl;
    cout << "5. Last N days" << endl;
    cout << "0. Back" << endl;
}

void ConsoleUI::handleSearch() {
    clearScreen();
    showSearchMenu();

    int choice = getChoice(0, 5);
    if (choice == 0) return;

    vector<int> results;

    switch (choice) {
    case 1: {
        string author = getString("Enter author name: ");
        results = notebook.findByAuthor(author);
        break;
    }
    case 2: {
        string tag = getString("Enter tag: ");
        results = notebook.findByTag(tag);
        break;
    }
    case 3: {
        string word = getString("Enter word to search: ");
        results = notebook.findByWord(word);
        break;
    }
    case 4: {
        string date = getString("Enter date (YYYY-MM-DD): ");
        results = notebook.findByDate(date);
        break;
    }
    case 5: {
        int days = getInt("Enter number of days: ", 1, 365);
        results = notebook.findByLastNDays(days);
        break;
    }
    }

    notebook.printNotes(results);
    pressAnyKey();
}

void ConsoleUI::handleStatistics() {
    clearScreen();
    cout << "=== STATISTICS ===" << endl;

    auto authorStats = notebook.getAuthorStats();
    cout << "\nBy Author:" << endl;
    if (authorStats.empty()) {
        cout << "No data" << endl;
    }
    else {
        for (const auto& pair : authorStats) {
            cout << "  " << pair.first << ": " << pair.second << " notes" << endl;
        }
    }

    auto tagStats = notebook.getTagStats();
    cout << "\nBy Tag:" << endl;
    if (tagStats.empty()) {
        cout << "No data" << endl;
    }
    else {
        for (const auto& pair : tagStats) {
            cout << "  #" << pair.first << ": " << pair.second << " times" << endl;
        }
    }

    pressAnyKey();
}

void ConsoleUI::handleSave() {
    try {
        notebook.saveToFile();
        unsavedChanges = false;
        cout << "Saved successfully!" << endl;
    }
    catch (const exception& e) {
        cout << "Error saving: " << e.what() << endl;
    }
    pressAnyKey();
}

void ConsoleUI::handleLoad() {
    try {
        notebook.loadFromFile();
        unsavedChanges = false;
        cout << "Loaded successfully!" << endl;
    }
    catch (const exception& e) {
        cout << "Error loading: " << e.what() << endl;
    }
    pressAnyKey();
}

void ConsoleUI::handleTestScenarios() {
    clearScreen();
    notebook.runTestScenarios();
    pressAnyKey();
}

// Вспомогательные методы
int ConsoleUI::getChoice(int min, int max) {
    int choice;
    while (true) {
        cout << "Enter choice [" << min << "-" << max << "]: ";
        cin >> choice;
        if (cin.fail() || choice < min || choice > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again." << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return choice;
}

string ConsoleUI::getString(const string& prompt, bool allowEmpty) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);

        // Убираем пробелы в начале и конце
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);

        if (!allowEmpty && input.empty()) {
            cout << "Input cannot be empty. Try again." << endl;
        }
        else {
            break;
        }
    }
    return input;
}

int ConsoleUI::getInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < min || value > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Must be between " << min << " and " << max << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return value;
}

vector<string> ConsoleUI::getTags() {
    vector<string> tags;
    cout << "Enter tags (comma separated, e.g.: work,personal,urgent): ";
    string input;
    getline(cin, input);

    if (!input.empty()) {
        string tag;
        size_t start = 0, end = 0;
        while ((end = input.find(',', start)) != string::npos) {
            tag = input.substr(start, end - start);
            // Убираем пробелы
            tag.erase(0, tag.find_first_not_of(" \t"));
            tag.erase(tag.find_last_not_of(" \t") + 1);
            if (!tag.empty()) {
                tags.push_back(tag);
            }
            start = end + 1;
        }
        // Последний тег
        tag = input.substr(start);
        tag.erase(0, tag.find_first_not_of(" \t"));
        tag.erase(tag.find_last_not_of(" \t") + 1);
        if (!tag.empty()) {
            tags.push_back(tag);
        }
    }
    return tags;
}

void ConsoleUI::pressAnyKey() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void ConsoleUI::clearScreen() {
    system(CLEAR_SCREEN);
}