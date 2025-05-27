#ifndef FAMILY_EVENTS_H
#define FAMILY_EVENTS_H

#include <memory>
#include <string>
#include "genealogical_tree.h"
#include "observer.h"

class FamilyEvent {
protected:
    std::shared_ptr<GenealogicalTree> tree;

public:
    FamilyEvent(std::shared_ptr<GenealogicalTree> tree);
    virtual ~FamilyEvent() = default;
    virtual void execute() = 0;
};

class BirthEvent : public FamilyEvent {
private:
    std::shared_ptr<Person> mother;
    std::shared_ptr<Person> father;
    std::shared_ptr<Person> child;

public:
    BirthEvent(std::shared_ptr<GenealogicalTree> tree,
        std::shared_ptr<Person> mother,
        std::shared_ptr<Person> father,
        const std::string& fName, const std::string& lName, const std::string& mName,
        const std::string& gender, const std::string& bPlace, const std::string& occupation);
    void execute() override;
};

class DeathEvent : public FamilyEvent {
private:
    std::shared_ptr<Person> person;
    std::string deathPlace;

public:
    DeathEvent(std::shared_ptr<GenealogicalTree> tree,
        std::shared_ptr<Person> person,
        const std::string& deathPlace);
    void execute() override;
};

class MarriageEvent : public FamilyEvent {
private:
    std::shared_ptr<Person> person1;
    std::shared_ptr<Person> person2;

public:
    MarriageEvent(std::shared_ptr<GenealogicalTree> tree,
        std::shared_ptr<Person> person1,
        std::shared_ptr<Person> person2);
    void execute() override;
};

class DivorceEvent : public FamilyEvent {
private:
    std::shared_ptr<Person> person1;
    std::shared_ptr<Person> person2;

public:
    DivorceEvent(std::shared_ptr<GenealogicalTree> tree,
        std::shared_ptr<Person> person1,
        std::shared_ptr<Person> person2);
    void execute() override;
};

class LoggerObserver : public IObserver {
public:
    void onPersonChanged(const Person& person, const std::string& changeType) override;
    void onRelationshipChanged(const Relationship& rel,
        const std::shared_ptr<Person>& p1,
        const std::shared_ptr<Person>& p2,
        const std::string& changeType) override;
};

#endif // FAMILY_EVENTS_H