// ConsoleUI.h
#pragma once

#include "Notebook.h"
#include <string>

// Класс ConsoleUI представляет консольный пользовательский интерфейс
// Реализует взаимодействие с пользователем через текстовое меню
class ConsoleUI {
private:
    Notebook notebook;           // Основной объект записной книжки
    bool unsavedChanges = false; // Флаг наличия несохраненных изменений

    // ========== МЕТОДЫ ОТОБРАЖЕНИЯ ==========

    // Показать главное меню приложения
    void showMainMenu();

    // Показать меню поиска
    void showSearchMenu();

    // ========== МЕТОДЫ ОБРАБОТКИ ==========

    // Обработка создания новой заметки
    void handleCreateNote();

    // Обработка вывода списка всех заметок
    void handleListNotes();

    // Обработка просмотра конкретной заметки
    void handleViewNote();

    // Обработка редактирования заметки
    void handleEditNote(int index);

    // Обработка удаления заметки
    void handleDeleteNote();

    // Обработка поиска заметок
    void handleSearch();

    // Обработка вывода статистики
    void handleStatistics();

    // Обработка сохранения в файл
    void handleSave();

    // Обработка загрузки из файла
    void handleLoad();

    // Обработка запуска тестовых сценариев
    void handleTestScenarios();

    //// Обработка демонстрации полиморфизма
    //void handlePolymorphismDemo();

    // ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

    // Получить выбор пользователя из заданного диапазона
    int getChoice(int min, int max);

    // Получить строку от пользователя с валидацией
    std::string getString(const std::string& prompt, bool allowEmpty = false);

    // Получить целое число от пользователя с валидацией диапазона
    int getInt(const std::string& prompt, int min, int max);

    // Получить список тегов от пользователя (через запятую)
    std::vector<std::string> getTags();

    // Ожидание нажатия клавиши Enter для продолжения
    void pressAnyKey();

    // Очистка экрана консоли (кроссплатформенная)
    void clearScreen();

public:
    // Главный метод запуска приложения
    void run();
};