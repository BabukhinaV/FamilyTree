#pragma once

#include <string>
#include <memory>

class Person;
class Relationship;

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onPersonChanged(const Person& person, const std::string& changeType) = 0;
    virtual void onRelationshipChanged(const Relationship& rel,
        const std::shared_ptr<Person>& p1,
        const std::shared_ptr<Person>& p2,
        const std::string& changeType) = 0;
};
