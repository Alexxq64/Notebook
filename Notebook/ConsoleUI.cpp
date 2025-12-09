#include "ConsoleUI.h"
#include "Note.h"
#include <iostream>

using namespace std;

void ConsoleUI::run() {
    cout << "=== Notebook App ===" << endl;

    // Тест класса Note
    Note note("John", "Test Note", "This is a test content");
    note.setTags({ "test", "example" });
    note.print();

    // Тест JSON
    nlohmann::json j = note.toJson();
    cout << "\nJSON: " << j.dump(2) << endl;

    cout << "\nPress Enter to exit...";
    cin.get();
}