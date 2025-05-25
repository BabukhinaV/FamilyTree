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
        birthPlace(bPlace), deathPlace(""), occupation(occupation), alive(true) {}

    // Геттеры
    string getFullName() const { return lastName + " " + firstName + " " + middleName; }
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

// Реализация методов Observable после определения Person и Relationship
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
    BirthEvent(shared_ptr<Relationship> relSystem, 
              shared_ptr<Person> mother, 
              shared_ptr<Person> father, 
              shared_ptr<Person> child)
        : FamilyEvent(relSystem), mother(mother), father(father), child(child) {}

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
        child->notifyPersonChanged(*child, "birth");
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

    // Создаем наблюдателя
    auto logger = make_shared<LoggerObserver>();

    // Создаем систему отношений
    auto relationshipSystem = make_shared<Relationship>();
    relationshipSystem->addObserver(logger);

    // Создаем людей и подписываем на наблюдателя
    auto ivan = make_shared<Person>("Иван", "Иванов", "Иванович", "мужской", "Москва", "Инженер");
    ivan->addObserver(logger);
    
    auto maria = make_shared<Person>("Мария", "Иванова", "Петровна", "женский", "Санкт-Петербург", "Учитель");
    maria->addObserver(logger);
    
    auto petr = make_shared<Person>("Петр", "Иванов", "Иванович", "мужской", "Москва", "Студент");
    petr->addObserver(logger);

    // Выводим информацию о людях
    cout << "=== Информация о людях ===" << endl;
    ivan->printInfo();
    cout << endl;
    maria->printInfo();
    cout << endl;

    // Событие свадьбы
    cout << "\n=== Событие: Свадьба ===" << endl;
    relationshipSystem->addRelationship(ivan, maria, "spouse");
    relationshipSystem->addRelationship(maria, ivan, "spouse");

    // Событие рождения ребенка
    cout << "\n=== Событие: Рождение ребенка ===" << endl;
    BirthEvent birth(relationshipSystem, maria, ivan, petr);
    birth.execute();
    petr->printInfo();
    cout << endl;

    // Событие смерти
    cout << "\n=== Событие: Смерть ===" << endl;
    ivan->setDeath("Сочи");
    ivan->printInfo();

    return 0;
}