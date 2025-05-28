#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <unordered_set>
#include <fstream>
#include <sstream>

using namespace std;

// Предварительные объявления
class Person;
class Relationship;

// Базовый класс наблюдателя
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onPersonChanged(const Person& person, const string& changeType) = 0;
    virtual void onRelationshipChanged(const Relationship& rel,
        const shared_ptr<Person>& p1,
        const shared_ptr<Person>& p2,
        const string& changeType) = 0;
};

// Базовый класс наблюдаемого объекта
class Observable {
protected:
    unordered_set<shared_ptr<IObserver>> observers;

public:
    void addObserver(const shared_ptr<IObserver>& observer) {
        observers.insert(observer);
    }

    void removeObserver(const shared_ptr<IObserver>& observer) {
        observers.erase(observer);
    }

    void notifyPersonChanged(const Person& person, const string& changeType);
    void notifyRelationshipChanged(const Relationship& rel,
        const shared_ptr<Person>& p1,
        const shared_ptr<Person>& p2,
        const string& changeType);
};

// Класс человека
class Person : public Observable {
private:
    string firstName;
    string lastName;
    string middleName;
    string gender;
    string birthPlace;
    string deathPlace;
    string occupation;
    string birthDate;
    string biography;
    bool alive;

public:
    Person(string fName, string lName, string mName, string gender,
        string bPlace, string bDate, string occupation, string bio = "")
        : firstName(fName), lastName(lName), middleName(mName), gender(gender),
        birthPlace(bPlace), birthDate(bDate), occupation(occupation),
        biography(bio), deathPlace(""), alive(true) {
    }

    // Геттеры
    string getFullName() const { return lastName + " " + firstName + " " + middleName; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    string getMiddleName() const { return middleName; }
    string getGender() const { return gender; }
    string getBirthPlace() const { return birthPlace; }
    string getDeathPlace() const { return deathPlace; }
    string getOccupation() const { return occupation; }
    string getBirthDate() const { return birthDate; }
    string getBiography() const { return biography; }
    bool isAlive() const { return alive; }

    // Сеттеры
    void setDeath(string place) {
        deathPlace = place;
        alive = false;
        notifyPersonChanged(*this, "death");
    }

    void printInfo() const {
        cout << "ФИО: " << getFullName() << endl;
        cout << "Пол: " << gender << endl;
        cout << "Место рождения: " << birthPlace << endl;
        if (!alive) {
            cout << "Место смерти: " << deathPlace << endl;
        }
        cout << "Профессия: " << occupation << endl;
        cout << "Статус: " << (alive ? "Жив" : "Умер") << endl;
        cout << "Дата рождения: " << birthDate << endl;
        if (!biography.empty())
            cout << "Биография: " << biography << endl;
    }

    // Методы для сериализации
    string serialize() const {
        return firstName + "|" + lastName + "|" + middleName + "|" + gender + "|" +
            birthPlace + "|" + birthDate + "|" + deathPlace + "|" + occupation + "|" +
            (alive ? "1" : "0") + "|" + biography;
    }


    static shared_ptr<Person> deserialize(const string& data) {
        vector<string> fields;
        stringstream ss(data);
        string field;

        while (getline(ss, field, '|')) {
            fields.push_back(field);
        }

        if (fields.size() < 8) return nullptr;

        auto person = make_shared<Person>(
            fields[0], // firstName
            fields[1], // lastName
            fields[2], // middleName
            fields[3], // gender
            fields[4], // birthPlace
            fields[5], // birthDate
            fields[7], // occupation
            fields.size() > 9 ? fields[9] : "" // biography
        );

        if (!fields[6].empty()) {
            person->deathPlace = fields[6];
        }
        if (fields.size() > 8) {
            person->alive = fields[8] == "1";
        }

        return person;
    }

};

// Реализация методов Observable
void Observable::notifyPersonChanged(const Person& person, const string& changeType) {
    for (const auto& observer : observers) {
        observer->onPersonChanged(person, changeType);
    }
}

void Observable::notifyRelationshipChanged(const Relationship& rel,
    const shared_ptr<Person>& p1,
    const shared_ptr<Person>& p2,
    const string& changeType) {
    for (const auto& observer : observers) {
        observer->onRelationshipChanged(rel, p1, p2, changeType);
    }
}

// Класс отношений
class Relationship : public Observable {
private:
    vector<tuple<shared_ptr<Person>, shared_ptr<Person>, string>> relationships;

public:
    // Добавление отношения
    void addRelationship(shared_ptr<Person> person1, shared_ptr<Person> person2, string relationType) {
        relationships.emplace_back(person1, person2, relationType);
        notifyRelationshipChanged(*this, person1, person2, "add:" + relationType);

        // Автоматически добавляем обратное отношение
        if (relationType == "parent-child") {
            relationships.emplace_back(person2, person1, "child-parent");
            notifyRelationshipChanged(*this, person2, person1, "add:child-parent");
        }
        else if (relationType == "sibling") {
            relationships.emplace_back(person2, person1, "sibling");
            notifyRelationshipChanged(*this, person2, person1, "add:sibling");
        }
        else if (relationType == "spouse") {
            relationships.emplace_back(person2, person1, "spouse");
            notifyRelationshipChanged(*this, person2, person1, "add:spouse");
        }
    }

    // Удаление отношения
    void removeRelationship(shared_ptr<Person> person1, shared_ptr<Person> person2, string relationType) {
        relationships.erase(
            remove_if(relationships.begin(), relationships.end(),
                [&](const auto& rel) {
                    return get<0>(rel) == person1 && get<1>(rel) == person2 && get<2>(rel) == relationType;
                }),
            relationships.end());
        notifyRelationshipChanged(*this, person1, person2, "remove:" + relationType);

        // Удаляем обратное отношение
        if (relationType == "spouse") {
            relationships.erase(
                remove_if(relationships.begin(), relationships.end(),
                    [&](const auto& rel) {
                        return get<0>(rel) == person2 && get<1>(rel) == person1 && get<2>(rel) == "spouse";
                    }),
                relationships.end());
            notifyRelationshipChanged(*this, person2, person1, "remove:spouse");
        }
    }

    // Получение всех отношений для человека
    vector<tuple<shared_ptr<Person>, string>> getRelationshipsFor(shared_ptr<Person> person) const {
        vector<tuple<shared_ptr<Person>, string>> result;
        for (const auto& rel : relationships) {
            if (get<0>(rel) == person) {
                result.emplace_back(get<1>(rel), get<2>(rel));
            }
        }
        return result;
    }

    // Получение всех отношений
    const vector<tuple<shared_ptr<Person>, shared_ptr<Person>, string>>& getAllRelationships() const {
        return relationships;
    }

    // Очистка всех отношений
    void clear() {
        relationships.clear();
    }
};

// Класс генеалогического древа (только хранение и вывод информации)
class GenealogicalTree {
private:
    vector<shared_ptr<Person>> familyMembers;
    shared_ptr<Relationship> relationshipSystem;

public:
    GenealogicalTree() : relationshipSystem(make_shared<Relationship>()) {}

    // Добавление нового члена семьи
    void addFamilyMember(shared_ptr<Person> person) {
        familyMembers.push_back(person);
    }

    // Получение системы отношений
    shared_ptr<Relationship> getRelationshipSystem() const {
        return relationshipSystem;
    }

    // Получение всех членов семьи
    const vector<shared_ptr<Person>>& getFamilyMembers() const {
        return familyMembers;
    }

    // Поиск человека по ФИО
    shared_ptr<Person> findPerson(const string& fullName) const {
        for (const auto& person : familyMembers) {
            if (person->getFullName() == fullName) {
                return person;
            }
        }
        return nullptr;
    }

    // Печать всех отношений
    void printAllRelationships() const {
        cout << "=== Все связи ===" << endl;
        auto relationships = relationshipSystem->getAllRelationships();
        if (relationships.empty()) {
            cout << "Связей не найдено." << endl;
            return;
        }
        for (const auto& rel : relationships) {
            cout << get<0>(rel)->getFullName()
                << " -> "
                << get<1>(rel)->getFullName()
                << " (" << get<2>(rel) << ")" << endl;
        }
    }


    // Вывод информации о человеке
    void printPersonInfo(const string& fullName) const {
        auto person = findPerson(fullName);
        if (person) {
            person->printInfo();
        }
        else {
            cout << "Человек с именем " << fullName << " не найден в древе." << endl;
        }
    }

    // Вывод всех членов семьи
    void printAllMembers() const {
        cout << "=== Все члены семьи ===" << endl;
        for (size_t i = 0; i < familyMembers.size(); ++i) {
            cout << i + 1 << ". " << familyMembers[i]->getFullName() << endl;
        }
    }

    // Вывод родственных связей для человека
    void printRelationships(const string& fullName) const {
        auto person = findPerson(fullName);
        if (!person) {
            cout << "Человек не найден." << endl;
            return;
        }

        cout << "=== Родственные связи для " << fullName << " ===" << endl;
        auto relationships = relationshipSystem->getRelationshipsFor(person);

        if (relationships.empty()) {
            cout << "Связи не найдены." << endl;
            return;
        }

        // Сначала выводим детей (если есть)
        bool hasChildren = false;
        for (const auto& rel : relationships) {
            if (get<1>(rel) == "parent-child") {
                if (!hasChildren) {
                    cout << "Дети:" << endl;
                    hasChildren = true;
                }
                cout << "- " << get<0>(rel)->getFullName() << endl;
            }
        }

        // Затем выводим супругов (если есть)
        bool hasSpouses = false;
        for (const auto& rel : relationships) {
            if (get<1>(rel) == "spouse") {
                if (!hasSpouses) {
                    cout << "Супруги:" << endl;
                    hasSpouses = true;
                }
                cout << "- " << get<0>(rel)->getFullName() << endl;
            }
        }

        // Затем выводим родителей (если есть)
        bool hasParents = false;
        for (const auto& rel : relationships) {
            if (get<1>(rel) == "child-parent") {
                if (!hasParents) {
                    cout << "Родители:" << endl;
                    hasParents = true;
                }
                cout << "- " << get<0>(rel)->getFullName() << endl;
            }
        }

        // Затем выводим братьев и сестер (если есть)
        bool hasSiblings = false;
        for (const auto& rel : relationships) {
            if (get<1>(rel) == "sibling") {
                if (!hasSiblings) {
                    cout << "Сиблинги:" << endl;
                    hasSiblings = true;
                }
                cout << "- " << get<0>(rel)->getFullName() << endl;
            }
        }

        // Выводим внуков (если есть)
        bool hasGrandchildren = false;
        for (const auto& childRel : relationships) {
            if (get<1>(childRel) == "parent-child") {
                auto child = get<0>(childRel);
                auto childRelationships = relationshipSystem->getRelationshipsFor(child);
                for (const auto& grandchildRel : childRelationships) {
                    if (get<1>(grandchildRel) == "parent-child") {
                        if (!hasGrandchildren) {
                            cout << "Внуки:" << endl;
                            hasGrandchildren = true;
                        }
                        cout << "- " << get<0>(grandchildRel)->getFullName() << endl;
                    }
                }
            }
        }

        // Выводим дядей и тёть (если есть)
        bool hasUnclesAunts = false;
        for (const auto& parentRel : relationships) {
            if (get<1>(parentRel) == "child-parent") {
                auto parent = get<0>(parentRel);
                auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
                for (const auto& uncleAuntRel : parentRelationships) {
                    if (get<1>(uncleAuntRel) == "sibling" && get<0>(uncleAuntRel) != person) {
                        if (!hasUnclesAunts) {
                            cout << "Дяди и тёти:" << endl;
                            hasUnclesAunts = true;
                        }
                        cout << "- " << get<0>(uncleAuntRel)->getFullName() << endl;
                    }
                }
            }
        }

        // Выводим племянников (если есть)
        bool hasNephews = false;
        for (const auto& siblingRel : relationships) {
            if (get<1>(siblingRel) == "sibling") {
                auto sibling = get<0>(siblingRel);
                auto siblingRelationships = relationshipSystem->getRelationshipsFor(sibling);
                for (const auto& nephewRel : siblingRelationships) {
                    if (get<1>(nephewRel) == "parent-child") {
                        if (!hasNephews) {
                            cout << "Племянники:" << endl;
                            hasNephews = true;
                        }
                        cout << "- " << get<0>(nephewRel)->getFullName() << endl;
                    }
                }
            }
        }

        // Выводим золовок и деверей (братья и сестры супругов) (если есть)
        bool hasInLawsSiblings = false;
        for (const auto& spouseRel : relationships) {
            if (get<1>(spouseRel) == "spouse") {
                auto spouse = get<0>(spouseRel);
                auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
                for (const auto& inLawRel : spouseRelationships) {
                    if (get<1>(inLawRel) == "sibling") {
                        if (!hasInLawsSiblings) {
                            cout << "Братья и сестры супруга:" << endl;
                            hasInLawsSiblings = true;
                        }
                        cout << "- " << get<0>(inLawRel)->getFullName() << endl;
                    }
                }
            }
        }

        // Выводим племянников со стороны супруга (если есть)
        bool hasSpouseNephews = false;
        for (const auto& spouseRel : relationships) {
            if (get<1>(spouseRel) == "spouse") {
                auto spouse = get<0>(spouseRel);
                auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
                for (const auto& siblingRel : spouseRelationships) {
                    if (get<1>(siblingRel) == "sibling") {
                        auto sibling = get<0>(siblingRel);
                        auto siblingRelationships = relationshipSystem->getRelationshipsFor(sibling);
                        for (const auto& nephewRel : siblingRelationships) {
                            if (get<1>(nephewRel) == "parent-child") {
                                if (!hasSpouseNephews) {
                                    cout << "Племянники со стороны супруга:" << endl;
                                    hasSpouseNephews = true;
                                }
                                cout << "- " << get<0>(nephewRel)->getFullName() << endl;
                            }
                        }
                    }
                }
            }
        }

        // Выводим бывших супругов (если есть)
        bool hasExSpouses = false;
        for (const auto& rel : relationships) {
            if (get<1>(rel) == "ex-spouse") {
                if (!hasExSpouses) {
                    cout << "Бывшие супруги:" << endl;
                    hasExSpouses = true;
                }
                cout << "- " << get<0>(rel)->getFullName() << endl;
            }
        }

        // Родители супруга (свекровь, свекор / тёща, тесть) (если есть)
        bool hasInLawParents = false;
        for (const auto& spouseRel : relationships) {
            if (get<1>(spouseRel) == "spouse") {
                auto spouse = get<0>(spouseRel);
                auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
                for (const auto& inLawParentRel : spouseRelationships) {
                    if (get<1>(inLawParentRel) == "child-parent") {
                        if (!hasInLawParents) {
                            cout << "Родители супруга/супруги (свекровь, свекор / тёща, тесть):" << endl;
                            hasInLawParents = true;
                        }
                        cout << "- " << get<0>(inLawParentRel)->getFullName()
                            << " (родитель " << spouse->getFullName() << ")" << endl;
                    }
                }
            }
        }

        // Дедушки и бабушки (родители родителей) (если есть)
        bool hasGrandParents = false;
        for (const auto& parentRel : relationships) {
            if (get<1>(parentRel) == "child-parent") {
                auto parent = get<0>(parentRel);
                auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
                for (const auto& grandParentRel : parentRelationships) {
                    if (get<1>(grandParentRel) == "child-parent") {
                        if (!hasGrandParents) {
                            cout << "Дедушки и бабушки:" << endl;
                            hasGrandParents = true;
                        }
                        cout << "- " << get<0>(grandParentRel)->getFullName()
                            << " (родитель " << parent->getFullName() << ")" << endl;
                    }
                }
            }
        }

        // Прадеды и прабабушки (если есть)
        bool hasGreatGrandParents = false;
        for (const auto& parentRel : relationships) {
            if (get<1>(parentRel) == "child-parent") {
                auto parent = get<0>(parentRel);
                auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
                for (const auto& grandParentRel : parentRelationships) {
                    if (get<1>(grandParentRel) == "child-parent") {
                        auto grandParent = get<0>(grandParentRel);
                        auto grandParentRelationships = relationshipSystem->getRelationshipsFor(grandParent);
                        for (const auto& greatGrandRel : grandParentRelationships) {
                            if (get<1>(greatGrandRel) == "child-parent") {
                                if (!hasGreatGrandParents) {
                                    cout << "Прадеды и прабабушки:" << endl;
                                    hasGreatGrandParents = true;
                                }
                                cout << "- " << get<0>(greatGrandRel)->getFullName()
                                    << " (родитель " << grandParent->getFullName() << ")" << endl;
                            }
                        }
                    }
                }
            }
        }

        // Прапрадеды и прапрабабушки (если есть)
        bool hasGreatGreatGrandParents = false;
        for (const auto& parentRel : relationships) {
            if (get<1>(parentRel) == "child-parent") {
                auto parent = get<0>(parentRel);
                auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
                for (const auto& grandParentRel : parentRelationships) {
                    if (get<1>(grandParentRel) == "child-parent") {
                        auto grandParent = get<0>(grandParentRel);
                        auto grandParentRelationships = relationshipSystem->getRelationshipsFor(grandParent);
                        for (const auto& greatGrandRel : grandParentRelationships) {
                            if (get<1>(greatGrandRel) == "child-parent") {
                                auto greatGrand = get<0>(greatGrandRel);
                                auto greatGrandRelationships = relationshipSystem->getRelationshipsFor(greatGrand);
                                for (const auto& greatGreatGrandRel : greatGrandRelationships) {
                                    if (get<1>(greatGreatGrandRel) == "child-parent") {
                                        if (!hasGreatGreatGrandParents) {
                                            cout << "Прапрадеды и прапрабабушки:" << endl;
                                            hasGreatGreatGrandParents = true;
                                        }
                                        cout << "- " << get<0>(greatGreatGrandRel)->getFullName()
                                            << " (родитель " << greatGrand->getFullName() << ")" << endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Сохранение данных в файл
    void saveToFile(const string& filename) {
        ofstream outFile(filename);
        if (!outFile) {
            cerr << "Не удалось открыть файл для записи: " << filename << endl;
            return;
        }

        // Сохраняем людей
        outFile << "[People]" << endl;
        for (const auto& person : familyMembers) {
            outFile << person->serialize() << endl;
        }

        // Сохраняем отношения
        outFile << "[Relationships]" << endl;
        auto relationships = relationshipSystem->getAllRelationships();
        for (const auto& rel : relationships) {
            outFile << get<0>(rel)->getFullName() << "|"
                << get<1>(rel)->getFullName() << "|"
                << get<2>(rel) << endl;
        }

        outFile.close();
        cout << "Данные успешно сохранены в файл: " << filename << endl;
    }

    // Загрузка данных из файла
    bool loadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "Не удалось открыть файл для чтения: " << filename << endl;
            return false;
        }

        // Очищаем текущие данные
        familyMembers.clear();
        relationshipSystem->clear();

        string line;
        bool readingPeople = false;
        bool readingRelationships = false;

        while (getline(inFile, line)) {
            if (line.empty()) continue;

            if (line == "[People]") {
                readingPeople = true;
                readingRelationships = false;
                continue;
            }
            else if (line == "[Relationships]") {
                readingPeople = false;
                readingRelationships = true;
                continue;
            }

            if (readingPeople) {
                auto person = Person::deserialize(line);
                if (person) {
                    familyMembers.push_back(person);
                }
            }
            else if (readingRelationships) {
                size_t pos1 = line.find('|');
                size_t pos2 = line.find('|', pos1 + 1);
                if (pos1 != string::npos && pos2 != string::npos) {
                    string person1Name = line.substr(0, pos1);
                    string person2Name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    string relationType = line.substr(pos2 + 1);

                    auto person1 = findPerson(person1Name);
                    auto person2 = findPerson(person2Name);

                    if (person1 && person2) {
                        relationshipSystem->addRelationship(person1, person2, relationType);
                    }
                }
            }
        }

        inFile.close();
        cout << "Данные успешно загружены из файла: " << filename << endl;
        return true;
    }
};
class FamilyEvent {
protected:
    shared_ptr<GenealogicalTree> tree;

public:
    FamilyEvent(shared_ptr<GenealogicalTree> tree) : tree(tree) {}
    virtual ~FamilyEvent() = default;
    virtual void execute() = 0;
};

// Событие рождения
class BirthEvent : public FamilyEvent {
    shared_ptr<Person> mother, father, child;
public:
    BirthEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> m, shared_ptr<Person> f,
        const string& fn, const string& ln, const string& mn,
        const string& g, const string& bPlace, const string& bDate,
        const string& occ, const string& bio)
        : FamilyEvent(tree), mother(m), father(f) {
        child = make_shared<Person>(fn, ln, mn, g, bPlace, bDate, occ, bio);
    }

    void execute() override {
        tree->addFamilyMember(child);
        auto rel = tree->getRelationshipSystem();
        rel->addRelationship(mother, child, "parent-child");
        rel->addRelationship(father, child, "parent-child");
        cout << "Ребёнок добавлен: " << child->getFullName() << endl;
    }
};

// Событие смерти
class DeathEvent : public FamilyEvent {
private:
    shared_ptr<Person> person;
    string deathPlace;

public:
    DeathEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> person,
        const string& deathPlace)
        : FamilyEvent(tree), person(person), deathPlace(deathPlace) {
    }

    void execute() override {
        person->setDeath(deathPlace);
        cout << "Зарегистрирована смерть: " << person->getFullName()
            << ", место: " << deathPlace << endl;
    }
};

// Событие брака
class MarriageEvent : public FamilyEvent {
private:
    shared_ptr<Person> person1;
    shared_ptr<Person> person2;

public:
    MarriageEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> person1,
        shared_ptr<Person> person2)
        : FamilyEvent(tree), person1(person1), person2(person2) {
    }

    void execute() override {
        auto relSystem = tree->getRelationshipSystem();
        relSystem->addRelationship(person1, person2, "spouse");
        relSystem->addRelationship(person2, person1, "spouse");
        cout << "Зарегистрирован брак между: " << person1->getFullName()
            << " и " << person2->getFullName() << endl;
    }
};

// Событие развода
class DivorceEvent : public FamilyEvent {
private:
    shared_ptr<Person> person1;
    shared_ptr<Person> person2;

public:
    DivorceEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> person1,
        shared_ptr<Person> person2)
        : FamilyEvent(tree), person1(person1), person2(person2) {
    }

    void execute() override {
        auto relSystem = tree->getRelationshipSystem();
        relSystem->removeRelationship(person1, person2, "spouse");
        relSystem->removeRelationship(person2, person1, "spouse");

        // Добавляем отношения "бывший супруг"
        relSystem->addRelationship(person1, person2, "ex-spouse");
        relSystem->addRelationship(person2, person1, "ex-spouse");

        cout << "Зарегистрирован развод между: " << person1->getFullName()
            << " и " << person2->getFullName() << endl;
    }
};

// Класс наблюдателя для логирования
class LoggerObserver : public IObserver {
public:
    void onPersonChanged(const Person& person, const string& changeType) override {
        cout << "[Лог] Изменение в Person: " << person.getFullName()
            << ", тип изменения: " << changeType << endl;
    }

    void onRelationshipChanged(const Relationship& rel,
        const shared_ptr<Person>& p1,
        const shared_ptr<Person>& p2,
        const string& changeType) override {
        cout << "[Лог] Изменение в Relationship: "
            << p1->getFullName() << " -> " << p2->getFullName()
            << ", тип изменения: " << changeType << endl;
    }
};

// Функции для работы с консольным интерфейсом
void printMenu() {
    cout << "\n=== Меню ===" << endl;
    cout << "1. Все люди" << endl;
    cout << "2. Инфо о человеке" << endl;
    cout << "3. Связи человека" << endl;
    cout << "4. Все связи" << endl;
    cout << "5. Добавить человека" << endl;
    cout << "6. Удалить человека" << endl;
    cout << "7. Добавить связь вручную" << endl;
    cout << "8. Удалить связь вручную" << endl;
    cout << "9. Вызвать событие (рождение, смерть, брак, развод)" << endl;
    cout << "10. Сохранить в файл" << endl;
    cout << "0. Выход\nВыберите: ";
}


void printRelationshipTypes() {
    cout << "Типы связей:" << endl;
    cout << "1. Родитель-ребенок" << endl;
    cout << "2. Супруг/супруга" << endl;
    cout << "3. Брат/сестра" << endl;
    cout << "4. Бывший супруг/супруга" << endl;
    cout << "Выберите тип связи: ";
}

shared_ptr<Person> selectPerson(const GenealogicalTree& tree) {
    tree.printAllMembers();
    cout << "Введите номер человека (0 для отмены): ";
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 0) return nullptr;

    const auto& members = tree.getFamilyMembers();
    if (choice > 0 && choice <= static_cast<int>(members.size())) {
        return members[choice - 1];
    }
    return nullptr;
}

void triggerEvent(shared_ptr<GenealogicalTree> tree) {
    cout << "\nВыберите событие:\n";
    cout << "1. Рождение\n2. Смерть\n3. Брак\n4. Развод\nВыбор: ";
    int ch;
    cin >> ch;
    cin.ignore();

    if (ch == 1) {
        auto mother = selectPerson(*tree);
        auto father = selectPerson(*tree);
        if (!mother || !father) return;

        string fn, ln, mn, g, bPlace, bDate, occ, bio;
        cout << "Имя ребёнка: "; getline(cin, fn);
        cout << "Фамилия: "; getline(cin, ln);
        cout << "Отчество: "; getline(cin, mn);
        cout << "Пол: "; getline(cin, g);
        cout << "Место рождения: "; getline(cin, bPlace);
        cout << "Дата рождения: "; getline(cin, bDate);
        cout << "Профессия: "; getline(cin, occ);
        cout << "Биография (необязательно): "; getline(cin, bio);

        BirthEvent event(tree, mother, father, fn, ln, mn, g, bPlace, bDate, occ, bio);
        event.execute();
    }
    else if (ch == 2) {
        auto person = selectPerson(*tree);
        if (!person) return;
        string place;
        cout << "Место смерти: ";
        getline(cin, place);
        DeathEvent event(tree, person, place);
        event.execute();
    }
    else if (ch == 3 || ch == 4) {
        auto p1 = selectPerson(*tree);
        auto p2 = selectPerson(*tree);
        if (!p1 || !p2) return;
        if (ch == 3) {
            MarriageEvent event(tree, p1, p2);
            event.execute();
        }
        else {
            DivorceEvent event(tree, p1, p2);
            event.execute();
        }
    }
}


void addPerson(GenealogicalTree& tree) {
    string firstName, lastName, middleName, gender, birthPlace, birthDate, occupation, bio;

    cout << "Введите имя: ";
    getline(cin, firstName);
    cout << "Введите фамилию: ";
    getline(cin, lastName);
    cout << "Введите отчество: ";
    getline(cin, middleName);
    cout << "Введите пол (мужской/женский): ";
    getline(cin, gender);
    cout << "Введите место рождения: ";
    getline(cin, birthPlace);
    cout << "Введите дату рождения: ";
    getline(cin, birthDate);
    cout << "Введите профессию: ";
    getline(cin, occupation);
    cout << "Введите биографию (необязательно): ";
    getline(cin, bio);

    auto person = make_shared<Person>(firstName, lastName, middleName, gender,
        birthPlace, birthDate, occupation, bio);
    tree.addFamilyMember(person);
    cout << "Человек успешно добавлен: " << person->getFullName() << endl;
}


void removePerson(GenealogicalTree& tree) {
    auto person = selectPerson(tree);
    if (!person) return;

    // Удаляем все связи с этим человеком
    auto relSystem = tree.getRelationshipSystem();
    auto relationships = relSystem->getAllRelationships();

    for (const auto& rel : relationships) {
        if (get<0>(rel) == person || get<1>(rel) == person) {
            relSystem->removeRelationship(get<0>(rel), get<1>(rel), get<2>(rel));
        }
    }

    // Удаляем человека из списка
    auto& members = const_cast<vector<shared_ptr<Person>>&>(tree.getFamilyMembers());
    members.erase(remove(members.begin(), members.end(), person), members.end());

    cout << "Человек успешно удален: " << person->getFullName() << endl;
}

void addRelationship(GenealogicalTree& tree) {
    cout << "Выберите первого человека:" << endl;
    auto person1 = selectPerson(tree);
    if (!person1) return;

    cout << "Выберите второго человека:" << endl;
    auto person2 = selectPerson(tree);
    if (!person2) return;

    printRelationshipTypes();
    int choice;
    cin >> choice;
    cin.ignore();

    string relationType;
    switch (choice) {
    case 1: relationType = "parent-child"; break;
    case 2: relationType = "spouse"; break;
    case 3: relationType = "sibling"; break;
    case 4: relationType = "ex-spouse"; break;
    default:
        cout << "Неверный выбор." << endl;
        return;
    }

    tree.getRelationshipSystem()->addRelationship(person1, person2, relationType);
    cout << "Связь успешно добавлена: " << person1->getFullName() << " -> "
        << person2->getFullName() << " (" << relationType << ")" << endl;
}

void removeRelationship(GenealogicalTree& tree) {
    cout << "Выберите первого человека:" << endl;
    auto person1 = selectPerson(tree);
    if (!person1) return;

    cout << "Выберите второго человека:" << endl;
    auto person2 = selectPerson(tree);
    if (!person2) return;

    // Получаем все связи между этими людьми
    auto relationships = tree.getRelationshipSystem()->getRelationshipsFor(person1);
    vector<string> availableRelations;

    for (const auto& rel : relationships) {
        if (get<0>(rel) == person2) {
            availableRelations.push_back(get<1>(rel));
        }
    }

    if (availableRelations.empty()) {
        cout << "Между этими людьми нет связей." << endl;
        return;
    }

    cout << "Доступные связи:" << endl;
    for (size_t i = 0; i < availableRelations.size(); ++i) {
        cout << i + 1 << ". " << availableRelations[i] << endl;
    }
    cout << "Выберите связь для удаления: ";

    int choice;
    cin >> choice;
    cin.ignore();

    if (choice > 0 && choice <= static_cast<int>(availableRelations.size())) {
        tree.getRelationshipSystem()->removeRelationship(person1, person2, availableRelations[choice - 1]);
        cout << "Связь успешно удалена." << endl;
    }
    else {
        cout << "Неверный выбор." << endl;
    }
}

void printFamilyTree(const GenealogicalTree& tree, const shared_ptr<Person>& person, int level = 0) {
    if (!person) return;

    for (int i = 0; i < level; ++i) cout << "  ";
    cout << "- " << person->getFullName() << endl;

    auto relationships = tree.getRelationshipSystem()->getRelationshipsFor(person);
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "parent-child") {
            printFamilyTree(tree, get<0>(rel), level + 1);
        }
    }
}

void viewFamilyTree(GenealogicalTree& tree) {
    auto person = selectPerson(tree);
    if (!person) return;

    cout << "\nГенеалогическое древо для " << person->getFullName() << ":" << endl;
    printFamilyTree(tree, person);
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    // Создаем древо и наблюдателя
    auto tree = make_shared<GenealogicalTree>();
    auto logger = make_shared<LoggerObserver>();

    // Загрузка данных из файла
    tree->loadFromFile("family_data.txt");

    int choice;
    do {
        printMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: tree->printAllMembers(); break;
        case 2: {
            auto p = selectPerson(*tree);
            if (p) p->printInfo();
            break;
        }
        case 3: {
            auto p = selectPerson(*tree);
            if (p) tree->printRelationships(p->getFullName());
            break;
        }
        case 4:
            tree->printAllRelationships();
            break;
        case 5:
            addPerson(*tree);
            tree->saveToFile("family_data.txt");
            break;
        case 6:
            removePerson(*tree);
            tree->saveToFile("family_data.txt");
            break;
        case 7:
            addRelationship(*tree);
            tree->saveToFile("family_data.txt");
            break;
        case 8:
            removeRelationship(*tree); tree->saveToFile("family_data.txt"); break;
        case 9:
            triggerEvent(tree); tree->saveToFile("family_data.txt"); break;
        case 10:
            tree->saveToFile("family_data.txt"); break;
        case 0:
            cout << "Пока!" << endl;
            break;
        default:
            cout << "Неверный выбор." << endl;
        }
    } while (choice != 0);

    return 0;
}