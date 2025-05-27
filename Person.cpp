#include <string>
#include "observable.h"

Person::Person(std::string fName, std::string lName, std::string mName,
    std::string gender, std::string bPlace, std::string occupation)
    : firstName(fName), lastName(lName), middleName(mName), gender(gender),
    birthPlace(bPlace), deathPlace(""), occupation(occupation), alive(true) {
}

std::string Person::getFullName() const {
    return lastName + " " + firstName + " " + middleName;
}

std::string Person::getShortName() const {
    return lastName + " " + firstName.substr(0, 1) + "." + middleName.substr(0, 1) + ".";
}

std::string Person::getFirstName() const { return firstName; }
std::string Person::getLastName() const { return lastName; }
std::string Person::getMiddleName() const { return middleName; }
std::string Person::getGender() const { return gender; }
std::string Person::getBirthPlace() const { return birthPlace; }
std::string Person::getDeathPlace() const { return deathPlace; }
std::string Person::getOccupation() const { return occupation; }
bool Person::isAlive() const { return alive; }

void Person::setDeath(std::string place) {
    deathPlace = place;
    alive = false;
    notifyPersonChanged(*this, "death");
}

void Person::printInfo() const {
    std::cout << "ФИО: " << getFullName() << std::endl;
    std::cout << "Пол: " << gender << std::endl;
    std::cout << "Место рождения: " << birthPlace << std::endl;
    if (!alive) {
        std::cout << "Место смерти: " << deathPlace << std::endl;
    }
    std::cout << "Профессия: " << occupation << std::endl;
    std::cout << "Статус: " << (alive ? "Жив" : "Умер") << std::endl;
}