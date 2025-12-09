#include "Note.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std;

Note::Note() {
    createdAt = getCurrentDate();
    updatedAt = createdAt;
}

Note::Note(const string& author, const string& title, const string& content)
    : author(author), title(title), content(content) {
    createdAt = getCurrentDate();
    updatedAt = createdAt;
}

void Note::setAuthor(const string& author) {
    if (author.empty()) throw invalid_argument("Author cannot be empty");
    this->author = author;
    updatedAt = getCurrentDate();
}

void Note::setTitle(const string& title) {
    if (title.empty()) throw invalid_argument("Title cannot be empty");
    this->title = title;
    updatedAt = getCurrentDate();
}

void Note::setContent(const string& content) {
    this->content = content;
    updatedAt = getCurrentDate();
}

void Note::setTags(const vector<string>& tags) {
    this->tags = tags;
    updatedAt = getCurrentDate();
}

void Note::update(const string& newTitle, const string& newContent,
    const vector<string>& newTags) {
    setTitle(newTitle);
    setContent(newContent);
    setTags(newTags);
}

nlohmann::json Note::toJson() const {
    nlohmann::json j;
    j["author"] = author;
    j["title"] = title;
    j["content"] = content;
    j["createdAt"] = createdAt;
    j["updatedAt"] = updatedAt;
    j["tags"] = tags;
    return j;
}

Note Note::fromJson(const nlohmann::json& j) {
    Note note;
    note.author = j.value("author", "");
    note.title = j.value("title", "");
    note.content = j.value("content", "");
    note.createdAt = j.value("createdAt", "");
    note.updatedAt = j.value("updatedAt", "");

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& tag : j["tags"]) {
            note.tags.push_back(tag.get<string>());
        }
    }
    return note;
}

bool Note::containsWord(const string& word) const {
    string lowerWord = word;
    transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    string lowerTitle = title;
    transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);

    string lowerContent = content;
    transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);

    return lowerTitle.find(lowerWord) != string::npos ||
        lowerContent.find(lowerWord) != string::npos;
}

bool Note::hasTag(const string& tag) const {
    return find(tags.begin(), tags.end(), tag) != tags.end();
}

void Note::print() const {
    cout << "\n=== " << title << " ===" << endl;
    cout << "Author: " << author << endl;
    cout << "Created: " << createdAt << endl;
    cout << "Updated: " << updatedAt << endl;
    cout << "Tags: ";
    for (const auto& tag : tags) {
        cout << "#" << tag << " ";
    }
    cout << "\nContent: " << content << endl;
}

string Note::getCurrentDate() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);

    stringstream ss;
    ss << put_time(&localTime, "%Y-%m-%d");
    return ss.str();
}