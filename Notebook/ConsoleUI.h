#pragma once
#include "Notebook.h"
#include <string>

class ConsoleUI {
private:
    Notebook notebook;
    bool unsavedChanges = false;

    // Методы отображения
    void showMainMenu();
    void showSearchMenu();

    // Методы обработки
    void handleCreateNote();
    void handleListNotes();
    void handleViewNote();
    void handleEditNote(int index);
    void handleDeleteNote();
    void handleSearch();
    void handleStatistics();
    void handleSave();
    void handleLoad();
    void handleTestScenarios();

    // Вспомогательные методы
    int getChoice(int min, int max);
    std::string getString(const std::string& prompt, bool allowEmpty = false);
    int getInt(const std::string& prompt, int min, int max);
    std::vector<std::string> getTags();
    void pressAnyKey();
    void clearScreen();

public:
    void run();
};