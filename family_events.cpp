#include "family_events.h"
#include <iostream>

// FamilyEvent
FamilyEvent::FamilyEvent(std::shared_ptr<GenealogicalTree> tree) : tree(tree) {}

// BirthEvent
BirthEvent::BirthEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> mother,
    std::shared_ptr<Person> father,
    const std::string& fName, const std::string& lName, const std::string& mName,
    const std::string& gender, const std::string& bPlace, const std::string& occupation)
    : FamilyEvent(tree), mother(mother), father(father),
    child(std::make_shared<Person>(fName, lName, mName, gender, bPlace, occupation)) {
}

void BirthEvent::execute() {
    auto relSystem = tree->getRelationshipSystem();

    tree->addFamilyMember(child);
    relSystem->addRelationship(mother, child, "parent-child");
    relSystem->addRelationship(father, child, "parent-child");

    auto motherChildren = relSystem->getRelationshipsFor(mother);
    for (const auto& rel : motherChildren) {
        if (get<1>(rel) == "parent-child" && get<0>(rel) != child) {
            relSystem->addRelationship(get<0>(rel), child, "sibling");
        }
    }

    std::cout << "Зарегистрировано рождение: " << child->getFullName() << std::endl;
}

// DeathEvent
DeathEvent::DeathEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> person,
    const std::string& deathPlace)
    : FamilyEvent(tree), person(person), deathPlace(deathPlace) {
}

void DeathEvent::execute() {
    person->setDeath(deathPlace);
    std::cout << "Зарегистрирована смерть: " << person->getFullName()
        << ", место: " << deathPlace << std::endl;
}

// MarriageEvent
MarriageEvent::MarriageEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> person1,
    std::shared_ptr<Person> person2)
    : FamilyEvent(tree), person1(person1), person2(person2) {
}

void MarriageEvent::execute() {
    auto relSystem = tree->getRelationshipSystem();
    relSystem->addRelationship(person1, person2, "spouse");
    relSystem->addRelationship(person2, person1, "spouse");
    std::cout << "Зарегистрирован брак между: " << person1->getFullName()
        << " и " << person2->getFullName() << std::endl;
}

// DivorceEvent
DivorceEvent::DivorceEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> person1,
    std::shared_ptr<Person> person2)
    : FamilyEvent(tree), person1(person1), person2(person2) {
}

void DivorceEvent::execute() {
    auto relSystem = tree->getRelationshipSystem();
    relSystem->removeRelationship(person1, person2, "spouse");
    relSystem->removeRelationship(person2, person1, "spouse");

    relSystem->addRelationship(person1, person2, "ex-spouse");
    relSystem->addRelationship(person2, person1, "ex-spouse");

    std::cout << "Зарегистрирован развод между: " << person1->getFullName()
        << " и " << person2->getFullName() << std::endl;
}

// LoggerObserver
void LoggerObserver::onPersonChanged(const Person& person, const std::string& changeType) {
    std::cout << "[Лог] Изменение в Person: " << person.getFullName()
        << ", тип изменения: " << changeType << std::endl;
}

void LoggerObserver::onRelationshipChanged(const Relationship& rel,
    const std::shared_ptr<Person>& p1,
    const std::shared_ptr<Person>& p2,
    const std::string& changeType) {
    std::cout << "[Лог] Изменение в Relationship: "
        << p1->getFullName() << " -> " << p2->getFullName()
        << ", тип изменения: " << changeType << std::endl;
}