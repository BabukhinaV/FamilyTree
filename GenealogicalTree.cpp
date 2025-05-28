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
    std::cout << "=== ��� ����� ����� ===" << std::endl;
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
        std::cout << "������� � ������ " << fullName << " �� ������ � �����." << std::endl;
    }
}

void GenealogicalTree::printAllRelationships() const {
    std::cout << "=== ��� ����� ===" << std::endl;
    auto relationships = relationshipSystem->getAllRelationships();
    if (relationships.empty()) {
        std::cout << "������ �� �������." << std::endl;
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
        cout << "������� �� ������." << endl;
        return;
    }

    cout << "=== ����������� ����� ��� " << fullName << " ===" << endl;
    auto relationships = relationshipSystem->getRelationshipsFor(person);

    if (relationships.empty()) {
        cout << "����� �� �������." << endl;
        return;
    }

    // ������� ������� ����� (���� ����)
    bool hasChildren = false;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "parent-child") {
            if (!hasChildren) {
                cout << "����:" << endl;
                hasChildren = true;
            }
            cout << "- " << get<0>(rel)->getFullName() << endl;
        }
    }

    // ����� ������� �������� (���� ����)
    bool hasSpouses = false;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "spouse") {
            if (!hasSpouses) {
                cout << "�������:" << endl;
                hasSpouses = true;
            }
            cout << "- " << get<0>(rel)->getFullName() << endl;
        }
    }

    // ����� ������� ��������� (���� ����)
    bool hasParents = false;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "child-parent") {
            if (!hasParents) {
                cout << "��������:" << endl;
                hasParents = true;
            }
            cout << "- " << get<0>(rel)->getFullName() << endl;
        }
    }

    // ����� ������� ������� � ������ (���� ����)
    bool hasSiblings = false;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "sibling") {
            if (!hasSiblings) {
                cout << "��������:" << endl;
                hasSiblings = true;
            }
            cout << "- " << get<0>(rel)->getFullName() << endl;
        }
    }

    // ������� ������ (���� ����)
    bool hasGrandchildren = false;
    for (const auto& childRel : relationships) {
        if (get<1>(childRel) == "parent-child") {
            auto child = get<0>(childRel);
            auto childRelationships = relationshipSystem->getRelationshipsFor(child);
            for (const auto& grandchildRel : childRelationships) {
                if (get<1>(grandchildRel) == "parent-child") {
                    if (!hasGrandchildren) {
                        cout << "�����:" << endl;
                        hasGrandchildren = true;
                    }
                    cout << "- " << get<0>(grandchildRel)->getFullName() << endl;
                }
            }
        }
    }

    // ������� ����� � ��� (���� ����)
    bool hasUnclesAunts = false;
    for (const auto& parentRel : relationships) {
        if (get<1>(parentRel) == "child-parent") {
            auto parent = get<0>(parentRel);
            auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
            for (const auto& uncleAuntRel : parentRelationships) {
                if (get<1>(uncleAuntRel) == "sibling" && get<0>(uncleAuntRel) != person) {
                    if (!hasUnclesAunts) {
                        cout << "���� � ���:" << endl;
                        hasUnclesAunts = true;
                    }
                    cout << "- " << get<0>(uncleAuntRel)->getFullName() << endl;
                }
            }
        }
    }

    // ������� ����������� (���� ����)
    bool hasNephews = false;
    for (const auto& siblingRel : relationships) {
        if (get<1>(siblingRel) == "sibling") {
            auto sibling = get<0>(siblingRel);
            auto siblingRelationships = relationshipSystem->getRelationshipsFor(sibling);
            for (const auto& nephewRel : siblingRelationships) {
                if (get<1>(nephewRel) == "parent-child") {
                    if (!hasNephews) {
                        cout << "����������:" << endl;
                        hasNephews = true;
                    }
                    cout << "- " << get<0>(nephewRel)->getFullName() << endl;
                }
            }
        }
    }

    // ������� ������� � ������� (������ � ������ ��������) (���� ����)
    bool hasInLawsSiblings = false;
    for (const auto& spouseRel : relationships) {
        if (get<1>(spouseRel) == "spouse") {
            auto spouse = get<0>(spouseRel);
            auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
            for (const auto& inLawRel : spouseRelationships) {
                if (get<1>(inLawRel) == "sibling") {
                    if (!hasInLawsSiblings) {
                        cout << "������ � ������ �������:" << endl;
                        hasInLawsSiblings = true;
                    }
                    cout << "- " << get<0>(inLawRel)->getFullName() << endl;
                }
            }
        }
    }

    // ������� ����������� �� ������� ������� (���� ����)
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
                                cout << "���������� �� ������� �������:" << endl;
                                hasSpouseNephews = true;
                            }
                            cout << "- " << get<0>(nephewRel)->getFullName() << endl;
                        }
                    }
                }
            }
        }
    }

    // ������� ������ �������� (���� ����)
    bool hasExSpouses = false;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "ex-spouse") {
            if (!hasExSpouses) {
                cout << "������ �������:" << endl;
                hasExSpouses = true;
            }
            cout << "- " << get<0>(rel)->getFullName() << endl;
        }
    }

    // �������� ������� (��������, ������ / ���, �����) (���� ����)
    bool hasInLawParents = false;
    for (const auto& spouseRel : relationships) {
        if (get<1>(spouseRel) == "spouse") {
            auto spouse = get<0>(spouseRel);
            auto spouseRelationships = relationshipSystem->getRelationshipsFor(spouse);
            for (const auto& inLawParentRel : spouseRelationships) {
                if (get<1>(inLawParentRel) == "child-parent") {
                    if (!hasInLawParents) {
                        cout << "�������� �������/������� (��������, ������ / ���, �����):" << endl;
                        hasInLawParents = true;
                    }
                    cout << "- " << get<0>(inLawParentRel)->getFullName()
                        << " (�������� " << spouse->getFullName() << ")" << endl;
                }
            }
        }
    }

    // ������� � ������� (�������� ���������) (���� ����)
    bool hasGrandParents = false;
    for (const auto& parentRel : relationships) {
        if (get<1>(parentRel) == "child-parent") {
            auto parent = get<0>(parentRel);
            auto parentRelationships = relationshipSystem->getRelationshipsFor(parent);
            for (const auto& grandParentRel : parentRelationships) {
                if (get<1>(grandParentRel) == "child-parent") {
                    if (!hasGrandParents) {
                        cout << "������� � �������:" << endl;
                        hasGrandParents = true;
                    }
                    cout << "- " << get<0>(grandParentRel)->getFullName()
                        << " (�������� " << parent->getFullName() << ")" << endl;
                }
            }
        }
    }

    // ������� � ���������� (���� ����)
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
                                cout << "������� � ����������:" << endl;
                                hasGreatGrandParents = true;
                            }
                            cout << "- " << get<0>(greatGrandRel)->getFullName()
                                << " (�������� " << grandParent->getFullName() << ")" << endl;
                        }
                    }
                }
            }
        }
    }

    // ���������� � ������������� (���� ����)
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
                                        cout << "���������� � �������������:" << endl;
                                        hasGreatGreatGrandParents = true;
                                    }
                                    cout << "- " << get<0>(greatGreatGrandRel)->getFullName()
                                        << " (�������� " << greatGrand->getFullName() << ")" << endl;
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
        std::cerr << "�� ������� ������� ���� ��� ������: " << filename << std::endl;
        return;
    }

    // ��������� �����
    outFile << "[People]" << std::endl;
    for (const auto& person : familyMembers) {
        outFile << person->serialize() << std::endl;
    }

    // ��������� ���������
    outFile << "[Relationships]" << std::endl;
    auto relationships = relationshipSystem->getAllRelationships();
    for (const auto& rel : relationships) {
        outFile << std::get<0>(rel)->getFullName() << "|"
            << std::get<1>(rel)->getFullName() << "|"
            << std::get<2>(rel) << std::endl;
    }

    outFile.close();
    std::cout << "������ ������� ��������� � ����: " << filename << std::endl;
}

bool GenealogicalTree::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "�� ������� ������� ���� ��� ������: " << filename << std::endl;
        return false;
    }

    // ������� ������� ������
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
    std::cout << "������ ������� ��������� �� �����: " << filename << std::endl;
    return true;
}
