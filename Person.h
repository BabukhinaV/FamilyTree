#pragma once

#include <string>
#include <memory>

#include "Observable.h"

class Person : public Observable {
private:
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string gender;
    std::string birthPlace;
    std::string deathPlace;
    std::string occupation;
    std::string birthDate;
    std::string biography;
    bool alive;

public:
    Person(std::string fName, std::string lName, std::string mName, std::string gender,
        std::string bPlace, std::string bDate, std::string occupation, std::string bio = "");

    std::string getFullName() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getMiddleName() const;
    std::string getGender() const;
    std::string getBirthPlace() const;
    std::string getDeathPlace() const;
    std::string getOccupation() const;
    std::string getBirthDate() const;
    std::string getBiography() const;
    bool isAlive() const;

    void setDeath(std::string place);
    void printInfo() const;

    std::string serialize() const;
    static std::shared_ptr<Person> deserialize(const std::string& data);
};
