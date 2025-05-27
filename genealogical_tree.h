#ifndef GENEALOGICAL_TREE_H
#define GENEALOGICAL_TREE_H

#include <vector>
#include <memory>
#include "person.h"
#include "relationship.h"

class GenealogicalTree {
private:
    std::vector<std::shared_ptr<Person>> familyMembers;
    std::shared_ptr<Relationship> relationshipSystem;

public:
    GenealogicalTree();
    void addFamilyMember(std::shared_ptr<Person> person);
    std::shared_ptr<Relationship> getRelationshipSystem() const;
    std::shared_ptr<Person> findPerson(const std::string& fullName) const;
    void printPersonInfo(const std::string& fullName) const;
    void printAllMembers() const;
    void printRelationships(const std::string& fullName) const;
    std::vector<std::shared_ptr<Person>> getAllMembers() const;
};

#endif // GENEALOGICAL_TREE_H