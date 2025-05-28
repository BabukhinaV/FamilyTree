#pragma once

#include <string>
#include <memory>
#include "GenealogicalTree.h"

class MenuHandler {
public:
    static bool isValidDate(const std::string& date);
    static bool isValidGender(const std::string& gender);
    static void printMenu();
    static void printRelationshipTypes();
    static std::shared_ptr<Person> selectPerson(const GenealogicalTree& tree);
    static void triggerEvent(std::shared_ptr<GenealogicalTree> tree);
    static void addPerson(GenealogicalTree& tree);
    static void removePerson(GenealogicalTree& tree);
    static void addRelationship(GenealogicalTree& tree);
    static void removeRelationship(GenealogicalTree& tree);
};
