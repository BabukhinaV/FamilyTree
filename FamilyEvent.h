#pragma once

#include <memory>
#include <string>

#include "GenealogicalTree.h"
#include "Person.h"

class FamilyEvent {
protected:
    std::shared_ptr<GenealogicalTree> tree;

public:
    FamilyEvent(std::shared_ptr<GenealogicalTree> tree);
    virtual ~FamilyEvent() = default;
    virtual void execute() = 0;
};

class BirthEvent : public FamilyEvent {
    std::shared_ptr<Person> mother, father, child;

public:
    BirthEvent(std::shared_ptr<GenealogicalTree> tree,
        std::shared_ptr<Person> m, std::shared_ptr<Person> f,
        const std::string& fn, const std::string& ln, const std::string& mn,
        const std::string& g, const std::string& bPlace, const std::string& bDate,
        const std::string& occ, const std::string& bio);

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
