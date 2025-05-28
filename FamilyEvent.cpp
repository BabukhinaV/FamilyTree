#include "FamilyEvent.h"
#include <iostream>

// --- ������� ����� ---
FamilyEvent::FamilyEvent(std::shared_ptr<GenealogicalTree> tree) : tree(tree) {}

// --- �������� ---
BirthEvent::BirthEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> m, std::shared_ptr<Person> f,
    const std::string& fn, const std::string& ln, const std::string& mn,
    const std::string& g, const std::string& bPlace, const std::string& bDate,
    const std::string& occ, const std::string& bio)
    : FamilyEvent(tree), mother(m), father(f) {
    child = std::make_shared<Person>(fn, ln, mn, g, bPlace, bDate, occ, bio);
}

void BirthEvent::execute() {
    tree->addFamilyMember(child);
    auto rel = tree->getRelationshipSystem();
    rel->addRelationship(mother, child, "parent-child");
    rel->addRelationship(father, child, "parent-child");
    std::cout << "������ ��������: " << child->getFullName() << std::endl;
}

// --- ������ ---
DeathEvent::DeathEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> person,
    const std::string& deathPlace)
    : FamilyEvent(tree), person(person), deathPlace(deathPlace) {
}

void DeathEvent::execute() {
    person->setDeath(deathPlace);
    std::cout << "���������������� ������: " << person->getFullName()
        << ", �����: " << deathPlace << std::endl;
}

// --- ���� ---
MarriageEvent::MarriageEvent(std::shared_ptr<GenealogicalTree> tree,
    std::shared_ptr<Person> person1,
    std::shared_ptr<Person> person2)
    : FamilyEvent(tree), person1(person1), person2(person2) {
}

void MarriageEvent::execute() {
    auto relSystem = tree->getRelationshipSystem();
    relSystem->addRelationship(person1, person2, "spouse");
    relSystem->addRelationship(person2, person1, "spouse");
    std::cout << "��������������� ���� �����: " << person1->getFullName()
        << " � " << person2->getFullName() << std::endl;
}

// --- ������ ---
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

    std::cout << "��������������� ������ �����: " << person1->getFullName()
        << " � " << person2->getFullName() << std::endl;
}
