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
#include <set>


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

    // Дети
    bool hasChildren = false;
    std::set<std::string> printedChildren;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "parent-child") {
            auto name = get<0>(rel)->getFullName();
            if (printedChildren.count(name)) continue;
            printedChildren.insert(name);
            if (!hasChildren) {
                std::cout << "Дети:" << std::endl;
                hasChildren = true;
            }
            std::cout << "- " << name << std::endl;
        }
    }

    // Супруги
    bool hasSpouses = false;
    std::set<std::string> printedSpouses;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "spouse") {
            auto name = get<0>(rel)->getFullName();
            if (printedSpouses.count(name)) continue;
            printedSpouses.insert(name);
            if (!hasSpouses) {
                std::cout << "Супруги:" << std::endl;
                hasSpouses = true;
            }
            std::cout << "- " << name << std::endl;
        }
    }
    // Сиблинги
    bool hasSiblings = false;
    std::set<std::string> printedSiblings;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "sibling") {
            auto name = get<0>(rel)->getFullName();
            if (printedSiblings.count(name)) continue;
            printedSiblings.insert(name);
            if (!hasSiblings) {
                std::cout << "Сиблинги:" << std::endl;
                hasSiblings = true;
            }
            std::cout << "- " << name << std::endl;
        }
    }
    bool hasParents = false;
    std::set<std::string> printedParents;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "child-parent") {
            std::string name = get<0>(rel)->getFullName();
            if (printedParents.count(name)) continue;
            printedParents.insert(name);
            if (!hasParents) {
                std::cout << "Родители:" << std::endl;
                hasParents = true;
            }
            std::cout << "- " << name << std::endl;
        }
    }
    bool hasGrandParents = false;
    std::set<std::string> printedGrandParents;
    for (const auto& parentRel : relationships) {
        if (get<1>(parentRel) == "child-parent") {
            auto parent = get<0>(parentRel);
            auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
            for (const auto& grandParentRel : parentRelationships) {
                if (get<1>(grandParentRel) == "child-parent") {
                    std::string name = get<0>(grandParentRel)->getFullName();
                    if (printedGrandParents.count(name)) continue;
                    printedGrandParents.insert(name);
                    if (!hasGrandParents) {
                        std::cout << "Дедушки и бабушки:" << std::endl;
                        hasGrandParents = true;
                    }
                    std::cout << "- " << name << " (родитель " << parent->getFullName() << ")" << std::endl;
                }
            }
        }
    }

    bool hasGrandchildren = false;
    std::set<std::string> printedGrandchildren;
    for (const auto& childRel : relationships) {
        if (get<1>(childRel) == "parent-child") {
            auto child = get<0>(childRel);
            auto childRelationships = relationshipSystem->getRelationshipsFor(child);
            for (const auto& grandchildRel : childRelationships) {
                if (get<1>(grandchildRel) == "parent-child") {
                    std::string name = get<0>(grandchildRel)->getFullName();
                    if (printedGrandchildren.count(name)) continue;
                    printedGrandchildren.insert(name);
                    if (!hasGrandchildren) {
                        std::cout << "Внуки:" << std::endl;
                        hasGrandchildren = true;
                    }
                    std::cout << "- " << name << std::endl;
                }
            }
        }
    }
    bool hasGreatGrandParents = false;
    std::set<std::string> printedGreatGrandParents;
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
                            std::string name = greatGrand->getFullName();
                            if (printedGreatGrandParents.count(name)) continue;
                            printedGreatGrandParents.insert(name);
                            if (!hasGreatGrandParents) {
                                std::cout << "Прадеды и прабабушки:" << std::endl;
                                hasGreatGrandParents = true;
                            }
                            std::cout << "- " << name << " (родитель " << grandParent->getFullName() << ")" << std::endl;
                        }
                    }
                }
            }
        }
    }
    bool hasNephews = false;
    std::set<std::string> printedNephews;
    for (const auto& siblingRel : relationships) {
        if (get<1>(siblingRel) == "sibling") {
            auto sibling = get<0>(siblingRel);
            auto siblingRelationships = relationshipSystem->getRelationshipsFor(sibling);
            for (const auto& nephewRel : siblingRelationships) {
                if (get<1>(nephewRel) == "parent-child") {
                    std::string name = get<0>(nephewRel)->getFullName();
                    if (printedNephews.count(name)) continue;
                    printedNephews.insert(name);
                    if (!hasNephews) {
                        std::cout << "Племянники:" << std::endl;
                        hasNephews = true;
                    }
                    std::cout << "- " << name << std::endl;
                }
            }
        }
    }
    bool hasSpouseNephews = false;
    std::set<std::string> printedSpouseNephews;
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
                            std::string name = get<0>(nephewRel)->getFullName();
                            if (printedSpouseNephews.count(name)) continue;
                            printedSpouseNephews.insert(name);
                            if (!hasSpouseNephews) {
                                std::cout << "Племянники со стороны супруга:" << std::endl;
                                hasSpouseNephews = true;
                            }
                            std::cout << "- " << name << std::endl;
                        }
                    }
                }
            }
        }
    }
    bool hasInLawParents = false;
    std::set<std::string> printedInLawParents;
    for (const auto& spouseRel : relationships) {
        if (get<1>(spouseRel) == "spouse") {
            auto spouse = get<0>(spouseRel);
            auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
            for (const auto& inLawParentRel : spouseRelationships) {
                if (get<1>(inLawParentRel) == "child-parent") {
                    std::string name = get<0>(inLawParentRel)->getFullName();
                    if (printedInLawParents.count(name)) continue;
                    printedInLawParents.insert(name);
                    if (!hasInLawParents) {
                        std::cout << "Родители супруга/супруги (свекровь, свекор / тёща, тесть):" << std::endl;
                        hasInLawParents = true;
                    }
                    std::cout << "- " << name << " (родитель " << spouse->getFullName() << ")" << std::endl;
                }
            }
        }
    }
    bool hasInLawsSiblings = false;
    std::set<std::string> printedInLawSiblings;
    for (const auto& spouseRel : relationships) {
        if (get<1>(spouseRel) == "spouse") {
            auto spouse = get<0>(spouseRel);
            auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
            for (const auto& inLawRel : spouseRelationships) {
                if (get<1>(inLawRel) == "sibling") {
                    std::string name = get<0>(inLawRel)->getFullName();
                    if (printedInLawSiblings.count(name)) continue;
                    printedInLawSiblings.insert(name);
                    if (!hasInLawsSiblings) {
                        std::cout << "Братья и сестры супруга:" << std::endl;
                        hasInLawsSiblings = true;
                    }
                    std::cout << "- " << name << std::endl;
                }
            }
        }
    }
    bool hasUnclesAunts = false;
    std::set<std::string> printedUnclesAunts;

    for (const auto& parentRel : relationships) {
        if (get<1>(parentRel) == "child-parent") {
            auto parent = get<0>(parentRel);
            auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);

            for (const auto& siblingRel : parentRelationships) {
                if (get<1>(siblingRel) == "sibling") {
                    auto uncleAunt = get<0>(siblingRel);
                    std::string name = uncleAunt->getFullName();
                    if (!printedUnclesAunts.count(name)) {
                        printedUnclesAunts.insert(name);
                        if (!hasUnclesAunts) {
                            std::cout << "Дяди и тёти:" << std::endl;
                            hasUnclesAunts = true;
                        }
                        std::cout << "- " << name << " (сиблинг " << parent->getFullName() << ")" << std::endl;
                    }

                    // По браку
                    auto inLawRelationships = relationshipSystem->getRelationshipsFor(uncleAunt);
                    for (const auto& spouseRel : inLawRelationships) {
                        if (get<1>(spouseRel) == "spouse") {
                            auto spouse = get<0>(spouseRel);
                            std::string spouseName = spouse->getFullName();
                            if (!printedUnclesAunts.count(spouseName)) {
                                printedUnclesAunts.insert(spouseName);
                                if (!hasUnclesAunts) {
                                    std::cout << "Дяди и тёти:" << std::endl;
                                    hasUnclesAunts = true;
                                }
                                std::cout << "- " << spouseName << " (супруг(а) " << uncleAunt->getFullName() << ")" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }

    bool hasCousins = false;
    std::set<std::string> printedCousins;

    for (const auto& parentRel : relationships) {
        if (get<1>(parentRel) == "child-parent") {
            auto parent = get<0>(parentRel);
            auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);

            for (const auto& siblingRel : parentRelationships) {
                if (get<1>(siblingRel) == "sibling") {
                    auto uncleAunt = get<0>(siblingRel);
                    auto uncleAuntRelationships = relationshipSystem->getRelationshipsFor(uncleAunt);

                    for (const auto& cousinRel : uncleAuntRelationships) {
                        if (get<1>(cousinRel) == "parent-child") {
                            auto cousin = get<0>(cousinRel);
                            std::string name = cousin->getFullName();

                            if (printedCousins.count(name)) continue;
                            printedCousins.insert(name);

                            if (!hasCousins) {
                                std::cout << "Двоюродные братья и сестры:" << std::endl;
                                hasCousins = true;
                            }

                            std::cout << "- " << name << " (ребёнок " << uncleAunt->getFullName() << ")" << std::endl;
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
