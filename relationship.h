#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <memory>

#include "Observable.h"
#include "Person.h"

class Relationship : public Observable {
private:
    std::vector<std::tuple<std::shared_ptr<Person>, std::shared_ptr<Person>, std::string>> relationships;

public:
    void addRelationship(std::shared_ptr<Person> person1, std::shared_ptr<Person> person2, std::string relationType);
    void removeRelationship(std::shared_ptr<Person> person1, std::shared_ptr<Person> person2, std::string relationType);
    std::vector<std::tuple<std::shared_ptr<Person>, std::string>> getRelationshipsFor(std::shared_ptr<Person> person) const;
    const std::vector<std::tuple<std::shared_ptr<Person>, std::shared_ptr<Person>, std::string>>& getAllRelationships() const;
    void clear();
};
