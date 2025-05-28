#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <unordered_set>

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
    bool alive;

public:
    Person(string fName, string lName, string mName, string gender,
        string bPlace, string occupation)
        : firstName(fName), lastName(lName), middleName(mName), gender(gender),
        birthPlace(bPlace), deathPlace(""), occupation(occupation), alive(true) {
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

    // Поиск человека по ФИО
    shared_ptr<Person> findPerson(const string& fullName) const {
        for (const auto& person : familyMembers) {
            if (person->getFullName() == fullName) {
                return person;
            }
        }
        return nullptr;
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
        for (const auto& person : familyMembers) {
            cout << "- " << person->getFullName() << endl;
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
    };
};
// Базовый класс семейного события
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
private:
    shared_ptr<Person> mother;
    shared_ptr<Person> father;
    shared_ptr<Person> child;

public:
    BirthEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> mother,
        shared_ptr<Person> father,
        const string& fName, const string& lName, const string& mName,
        const string& gender, const string& bPlace, const string& occupation)
        : FamilyEvent(tree), mother(mother), father(father),
        child(make_shared<Person>(fName, lName, mName, gender, bPlace, occupation)) {
    }

    void execute() override {
        auto relSystem = tree->getRelationshipSystem();

        // Добавляем ребенка в древо
        tree->addFamilyMember(child);

        // Устанавливаем отношения родитель-ребенок
        relSystem->addRelationship(mother, child, "parent-child");
        relSystem->addRelationship(father, child, "parent-child");

        // Добавляем братские отношения с другими детьми
        auto motherChildren = relSystem->getRelationshipsFor(mother);
        for (const auto& rel : motherChildren) {
            if (get<1>(rel) == "parent-child" && get<0>(rel) != child) {
                relSystem->addRelationship(get<0>(rel), child, "sibling");
            }
        }

        cout << "Зарегистрировано рождение: " << child->getFullName() << endl;
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

int main() {
    SetConsoleOutputCP(1251); // Настраиваем консоль для русского текста
    // Создаем древо и наблюдателя
    auto tree = make_shared<GenealogicalTree>();
    auto logger = make_shared<LoggerObserver>();

    // 1. Создаем бабушку и дедушку (родителей Ивана)
    cout << "\n=== Создаем бабушку и дедушку ===" << endl;
    auto grandfather = make_shared<Person>("Петр", "Иванов", "Сергеевич", "мужской",
        "Москва, 1925", "Военный");
    grandfather->addObserver(logger);
    tree->addFamilyMember(grandfather);

    auto grandmother = make_shared<Person>("Анна", "Иванова", "Михайловна", "женский",
        "Москва, 1930", "Учитель");
    grandmother->addObserver(logger);
    tree->addFamilyMember(grandmother);

    // 2. Регистрируем их брак
    cout << "\n=== Регистрируем брак бабушки и дедушки ===" << endl;
    MarriageEvent grandMarriage(tree, grandfather, grandmother);
    grandMarriage.execute();

    // 3. Рождение отца (Ивана) и его брата (деверь)
    cout << "\n=== Рождение Ивана и его брата (деверя) ===" << endl;
    BirthEvent fatherBirth(tree, grandmother, grandfather, "Иван", "Иванов", "Петрович",
        "мужской", "Москва, 1950", "Инженер");
    fatherBirth.execute();

    BirthEvent uncleBirth(tree, grandmother, grandfather, "Сергей", "Иванов", "Петрович",
        "мужской", "Москва, 1952", "Врач");
    uncleBirth.execute();

    // 4. Иван женится на Марии
    cout << "\n=== Иван создает свою семью ===" << endl;
    auto ivan = tree->findPerson("Иванов Иван Петрович");
    auto maria = make_shared<Person>("Мария", "Иванова", "Сергеевна", "женский",
        "Санкт-Петербург, 1952", "Учитель");
    maria->addObserver(logger);
    tree->addFamilyMember(maria);

    MarriageEvent marriage(tree, ivan, maria);
    marriage.execute();

    // 5. Рождение детей Ивана и Марии (один из них будет отцом племянника)
    cout << "\n=== Рождение детей Ивана и Марии ===" << endl;
    BirthEvent birth1(tree, maria, ivan, "Алексей", "Иванов", "Иванович",
        "мужской", "Москва, 1975", "Программист");
    birth1.execute();

    BirthEvent birth2(tree, maria, ivan, "Ольга", "Иванова", "Ивановна",
        "женский", "Москва, 1978", "Врач");
    birth2.execute();

    // 6. Сергей (деверь - брат мужа) женится на Елене
    cout << "\n=== Сергей (деверь) создает семью ===" << endl;
    auto sergey = tree->findPerson("Иванов Сергей Петрович");
    auto elena = make_shared<Person>("Елена", "Иванова", "Викторовна", "женский",
        "Москва, 1955", "Бухгалтер");
    elena->addObserver(logger);
    tree->addFamilyMember(elena);

    MarriageEvent uncleMarriage(tree, sergey, elena);
    uncleMarriage.execute();

    // 7. Рождение племянника (сына Сергея)
    cout << "\n=== Рождение племянника (сына Сергея) ===" << endl;
    BirthEvent nephewBirth(tree, elena, sergey, "Дмитрий", "Иванов", "Сергеевич",
        "мужской", "Москва, 1980", "Экономист");
    nephewBirth.execute();

    // 8. Выводим информацию о связях
    cout << "\n=== Информация о семье ===" << endl;
    tree->printAllMembers();
    cout << endl;

    cout << "=== Связи Марии (должен быть деверь - Сергей) ===" << endl;
    tree->printRelationships("Иванова Мария Сергеевна");
    cout << endl;

    cout << "=== Связи Алексея (должны быть дедушка, бабушка и племянник) ===" << endl;
    tree->printRelationships("Иванов Алексей Иванович");
    cout << endl;

    cout << "=== Связи Сергея (должны быть племянники - Алексей и Ольга) ===" << endl;
    tree->printRelationships("Иванов Сергей Петрович");
    cout << endl;

    cout << "=== Связи Дмитрия (должны быть дедушка и бабушка) ===" << endl;
    tree->printRelationships("Иванов Дмитрий Сергеевич");
    cout << endl;

    return 0;
}