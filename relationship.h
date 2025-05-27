#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <vector>
#include <tuple>
#include <memory>
#include "observable.h"
#include "person.h"

class Relationship : public Observable {
private:
    std::vector<std::tuple<std::shared_ptr<Person>, std::shared_ptr<Person>, std::string>> relationships;

public:
    void addRelationship(std::shared_ptr<Person> person1, std::shared_ptr<Person> person2, std::string relationType);
    void removeRelationship(std::shared_ptr<Person> person1, std::shared_ptr<Person> person2, std::string relationType);
    std::vector<std::tuple<std::shared_ptr<Person>, std::string>> getRelationshipsFor(std::shared_ptr<Person> person) const;
    std::vector<std::shared_ptr<Person>> getParents(std::shared_ptr<Person> person) const;
    std::vector<std::shared_ptr<Person>> getChildren(std::shared_ptr<Person> parent) const;
    std::vector<std::shared_ptr<Person>> getSiblings(std::shared_ptr<Person> person) const;
};

#endif // RELATIONSHIP_H