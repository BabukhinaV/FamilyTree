#include "GenealogicalTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "Person.h"
#include "Relationship.h"
#include "Person.h"
#include "Relationship.h"


using namespace std;

GenealogicalTree::GenealogicalTree() : relationshipSystem(std::make_shared<Relationship>()) {}

void GenealogicalTree::addFamilyMember(std::shared_ptr<Person> person) {
    familyMembers.push_back(person);
}

std::shared_ptr<Relationship> GenealogicalTree::getRelationshipSystem() const {
    return relationshipSystem;
}

const std::vector<std::shared_ptr<Person>>& GenealogicalTree::getFamilyMembers() const {
    return familyMembers;
}

std::shared_ptr<Person> GenealogicalTree::findPerson(const std::string& fullName) const {
    for (const auto& person : familyMembers) {
        if (person->getFullName() == fullName) {
            return person;
        }
    }
    return nullptr;
}

void GenealogicalTree::printAllMembers() const {
    std::cout << "=== Все члены семьи ===" << std::endl;
    for (size_t i = 0; i < familyMembers.size(); ++i) {
        std::cout << i + 1 << ". " << familyMembers[i]->getFullName() << std::endl;
    }
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

void GenealogicalTree::printAllRelationships() const {
    std::cout << "=== Все связи ===" << std::endl;
    auto relationships = relationshipSystem->getAllRelationships();
    if (relationships.empty()) {
        std::cout << "Связей не найдено." << std::endl;
        return;
    }
    for (const auto& rel : relationships) {
        std::cout << std::get<0>(rel)->getFullName()
            << " -> "
            << std::get<1>(rel)->getFullName()
            << " (" << std::get<2>(rel) << ")" << std::endl;
    }
}


void GenealogicalTree::printRelationships(const string& fullName) const {
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

void GenealogicalTree::saveToFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
        return;
    }

    // Сохраняем людей
    outFile << "[People]" << std::endl;
    for (const auto& person : familyMembers) {
        outFile << person->serialize() << std::endl;
    }

    // Сохраняем отношения
    outFile << "[Relationships]" << std::endl;
    auto relationships = relationshipSystem->getAllRelationships();
    for (const auto& rel : relationships) {
        outFile << std::get<0>(rel)->getFullName() << "|"
            << std::get<1>(rel)->getFullName() << "|"
            << std::get<2>(rel) << std::endl;
    }

    outFile.close();
    std::cout << "Данные успешно сохранены в файл: " << filename << std::endl;
}

bool GenealogicalTree::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Не удалось открыть файл для чтения: " << filename << std::endl;
        return false;
    }

    // Очищаем текущие данные
    familyMembers.clear();
    relationshipSystem->clear();

    std::string line;
    bool readingPeople = false;
    bool readingRelationships = false;

    while (std::getline(inFile, line)) {
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
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string person1Name = line.substr(0, pos1);
                std::string person2Name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string relationType = line.substr(pos2 + 1);

                auto person1 = findPerson(person1Name);
                auto person2 = findPerson(person2Name);

                if (person1 && person2) {
                    relationshipSystem->addRelationship(person1, person2, relationType);
                }
            }
        }
    }

    inFile.close();
    std::cout << "Данные успешно загружены из файла: " << filename << std::endl;
    return true;
}
