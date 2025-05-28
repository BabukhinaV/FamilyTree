#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Person.h"
#include "Relationship.h"

class GenealogicalTree {
private:
    std::vector<std::shared_ptr<Person>> familyMembers;
    std::shared_ptr<Relationship> relationshipSystem;

public:
    GenealogicalTree();

    void addFamilyMember(std::shared_ptr<Person> person);
    std::shared_ptr<Relationship> getRelationshipSystem() const;
    const std::vector<std::shared_ptr<Person>>& getFamilyMembers() const;
    std::shared_ptr<Person> findPerson(const std::string& fullName) const;

    void printAllMembers() const;
    void printPersonInfo(const std::string& fullName) const;
    void printAllRelationships() const;
    void printRelationships(const std::string& fullName) const;

    void saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
};
