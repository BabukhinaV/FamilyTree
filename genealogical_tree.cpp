#include "genealogical_tree.h"
#include <vector>
#include <memory>
#include "person.h"
#include "relationship.h"

GenealogicalTree::GenealogicalTree() : relationshipSystem(std::make_shared<Relationship>()) {}

void GenealogicalTree::addFamilyMember(std::shared_ptr<Person> person) {
    familyMembers.push_back(person);
}

std::shared_ptr<Relationship> GenealogicalTree::getRelationshipSystem() const {
    return relationshipSystem;
}

std::shared_ptr<Person> GenealogicalTree::findPerson(const std::string& fullName) const {
    for (const auto& person : familyMembers) {
        if (person->getFullName() == fullName) {
            return person;
        }
    }
    return nullptr;
}

void GenealogicalTree::printPersonInfo(const std::string& fullName) const {
    auto person = findPerson(fullName);
    if (person) {
        person->printInfo();
    }
    else {
        std::cout << "Человек с именем " << fullName << " не найден в древе." << std::endl;
    }
}

void GenealogicalTree::printAllMembers() const {
    std::cout << "=== Все члены семьи ===" << std::endl;
    for (const auto& person : familyMembers) {
        std::cout << "- " << person->getFullName() << std::endl;
    }
}

void GenealogicalTree::printRelationships(const std::string& fullName) const {
    auto person = findPerson(fullName);
    if (!person) {
        std::cout << "Человек не найден." << std::endl;
        return;
    }

    std::cout << "=== Родственные связи для " << fullName << " ===" << std::endl;
    auto relationships = relationshipSystem->getRelationshipsFor(person);

    if (relationships.empty()) {
        std::cout << "Связи не найдены." << std::endl;
        return;
    }

    for (const auto& rel : relationships) {
        std::string relationType;
        if (get<1>(rel) == "parent-child") {
            relationType = "родитель";
        }
        else if (get<1>(rel) == "child-parent") {
            relationType = "ребенок";
        }
        else if (get<1>(rel) == "sibling") {
            relationType = "брат/сестра";
        }
        else if (get<1>(rel) == "spouse") {
            relationType = "супруг(а)";
        }
        else if (get<1>(rel) == "ex-spouse") {
            relationType = "бывший(ая) супруг(а)";
        }
        else {
            relationType = get<1>(rel);
        }

        std::cout << "- " << get<0>(rel)->getFullName() << " (" << relationType << ")" << std::endl;
    }
}

std::vector<std::shared_ptr<Person>> GenealogicalTree::getAllMembers() const {
    return familyMembers;
}