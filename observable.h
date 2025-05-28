#pragma once

#include <string>
#include <unordered_set>
#include <memory>

#include "Observer.h"

class Person;
class Relationship;

class Observable {
protected:
    std::unordered_set<std::shared_ptr<IObserver>> observers;

public:
    void addObserver(const std::shared_ptr<IObserver>& observer);
    void removeObserver(const std::shared_ptr<IObserver>& observer);

    void notifyPersonChanged(const Person& person, const std::string& changeType);
    void notifyRelationshipChanged(const Relationship& rel,
        const std::shared_ptr<Person>& p1,
        const std::shared_ptr<Person>& p2,
        const std::string& changeType);
};
