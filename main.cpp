#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <algorithm>
#include <windows.h>

using namespace std;

// Предварительное объявление классов
class Relationship;
class FamilyEvent;

// Класс человека
class Person {
private:
    string firstName;
    string lastName;
    string middleName;
    string gender;
    string birthPlace;
    string deathPlace;
    string occupation;
    bool isAlive;

public:
    Person(string fName, string lName, string mName, string gender,
        string bPlace, string occupation)
        : firstName(fName), lastName(lName), middleName(mName), gender(gender),
        birthPlace(bPlace), deathPlace(""), occupation(occupation), isAlive(true) {
    }

    // Геттеры
    string getFullName() const { return lastName + " " + firstName + " " + middleName; }
    string getGender() const { return gender; }
    string getBirthPlace() const { return birthPlace; }
    string getDeathPlace() const { return deathPlace; }
    string getOccupation() const { return occupation; }
    bool isAlive() const { return isAlive; }

    // Сеттеры
    void setDeath(string place) {
        deathPlace = place;
        isAlive = false;
    }

    void printInfo() const {
        cout << "ФИО: " << getFullName() << endl;
        cout << "Пол: " << gender << endl;
        cout << "Место рождения: " << birthPlace << endl;
        if (!isAlive) {
            cout << "Место смерти: " << deathPlace << endl;
        }
        cout << "Профессия: " << occupation << endl;
        cout << "Статус: " << (isAlive ? "Жив" : "Умер") << endl;
    }
};

// Класс отношений
class Relationship {
private:
    vector<tuple<shared_ptr<Person>, shared_ptr<Person>, string>> relationships;

public:
    // Добавление отношения
    void addRelationship(shared_ptr<Person> person1, shared_ptr<Person> person2, string relationType) {
        relationships.emplace_back(person1, person2, relationType);

        // Автоматически добавляем обратное отношение для родитель-ребенок
        if (relationType == "parent-child") {
            relationships.emplace_back(person2, person1, "child-parent");
        }
        // Автоматически добавляем братские отношения, если у людей общий родитель
        else if (relationType == "sibling") {
            relationships.emplace_back(person2, person1, "sibling");
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
    }

    // Проверка наличия отношения
    bool hasRelationship(shared_ptr<Person> person1, shared_ptr<Person> person2, string relationType) const {
        return any_of(relationships.begin(), relationships.end(),
            [&](const auto& rel) {
                return get<0>(rel) == person1 && get<1>(rel) == person2 && get<2>(rel) == relationType;
            });
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

// Абстрактный класс семейного события
class FamilyEvent {
protected:
    shared_ptr<Relationship> relationshipSystem;

public:
    FamilyEvent(shared_ptr<Relationship> relSystem) : relationshipSystem(relSystem) {}
    virtual ~FamilyEvent() = default;
    virtual void execute() = 0;
};

// Класс события рождения
class BirthEvent : public FamilyEvent {
private:
    shared_ptr<Person> mother;
    shared_ptr<Person> father;
    shared_ptr<Person> child;

public:
    BirthEvent(shared_ptr<Relationship> relSystem, shared_ptr<Person> mother,
        shared_ptr<Person> father, shared_ptr<Person> child)
        : FamilyEvent(relSystem), mother(mother), father(father), child(child) {
    }

    void execute() override {
        // Добавляем отношения родитель-ребенок
        relationshipSystem->addRelationship(mother, child, "parent-child");
        relationshipSystem->addRelationship(father, child, "parent-child");

        // Проверяем и добавляем братские отношения
        auto motherChildren = relationshipSystem->getRelationshipsFor(mother);
        for (const auto& rel : motherChildren) {
            if (get<1>(rel) == "parent-child" && get<0>(rel) != child) {
                relationshipSystem->addRelationship(get<0>(rel), child, "sibling");
            }
        }

        cout << "Родился новый член семьи: " << child->getFullName() << endl;
    }
};

// Класс события смерти
class DeathEvent : public FamilyEvent {
private:
    shared_ptr<Person> person;
    string deathPlace;

public:
    DeathEvent(shared_ptr<Relationship> relSystem, shared_ptr<Person> person, string dPlace)
        : FamilyEvent(relSystem), person(person), deathPlace(dPlace) {
    }

    void execute() override {
        person->setDeath(deathPlace);
        cout << "Член семьи " << person->getFullName() << " умер в " << deathPlace << endl;
    }
};

// Класс события развода
class DivorceEvent : public FamilyEvent {
private:
    shared_ptr<Person> spouse1;
    shared_ptr<Person> spouse2;

public:
    DivorceEvent(shared_ptr<Relationship> relSystem, shared_ptr<Person> spouse1, shared_ptr<Person> spouse2)
        : FamilyEvent(relSystem), spouse1(spouse1), spouse2(spouse2) {
    }

    void execute() override {
        relationshipSystem->removeRelationship(spouse1, spouse2, "spouse");
        relationshipSystem->removeRelationship(spouse2, spouse1, "spouse");

        cout << spouse1->getFullName() << " и " << spouse2->getFullName() << " развелись" << endl;
    }
};

// Класс события свадьбы
class MarriageEvent : public FamilyEvent {
private:
    shared_ptr<Person> spouse1;
    shared_ptr<Person> spouse2;

public:
    MarriageEvent(shared_ptr<Relationship> relSystem, shared_ptr<Person> spouse1, shared_ptr<Person> spouse2)
        : FamilyEvent(relSystem), spouse1(spouse1), spouse2(spouse2) {
    }

    void execute() override {
        relationshipSystem->addRelationship(spouse1, spouse2, "spouse");
        relationshipSystem->addRelationship(spouse2, spouse1, "spouse");

        cout << spouse1->getFullName() << " и " << spouse2->getFullName() << " поженились" << endl;
    }
};


int main() {
    SetConsoleOutputCP(1251); // Настраиваем консоль для русского текста

    // Создаем систему отношений
    auto relationshipSystem = make_shared<Relationship>();

    // Создаем людей
    auto ivan = make_shared<Person>("Иван", "Иванов", "Иванович", "мужской", "Москва", "Инженер");
    auto maria = make_shared<Person>("Мария", "Иванова", "Петровна", "женский", "Санкт-Петербург", "Учитель");
    auto petr = make_shared<Person>("Петр", "Иванов", "Иванович", "мужской", "Москва", "Студент");

    // Выводим информацию о людях
    cout << "=== Информация о людях ===" << endl;
    ivan->printInfo();
    cout << endl;
    maria->printInfo();
    cout << endl;

    // Событие свадьбыvvv
    cout << "\n=== Событие: Свадьба ===" << endl;
    MarriageEvent wedding(relationshipSystem, ivan, maria);
    wedding.execute();

    // Событие рождения ребенка
    cout << "\n=== Событие: Рождение ребенка ===" << endl;
    BirthEvent birth(relationshipSystem, maria, ivan, petr);
    birth.execute();
    petr->printInfo();
    cout << endl;

    // Событие смерти
    cout << "\n=== Событие: Смерть ===" << endl;
    DeathEvent death(relationshipSystem, ivan, "Сочи");
    death.execute();
    ivan->printInfo();

    return 0;
}