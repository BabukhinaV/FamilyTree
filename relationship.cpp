#include "Relationship.h"
#include <algorithm>

void Relationship::addRelationship(std::shared_ptr<Person> person1, std::shared_ptr<Person> person2, std::string relationType) {
    relationships.emplace_back(person1, person2, relationType);
    notifyRelationshipChanged(*this, person1, person2, "add:" + relationType);

    // Автоматически добавляем обратное отношение
    if (relationType == "parent-child") {
        relationships.emplace_back(person2, person1, "child-parent");
        notifyRelationshipChanged(*this, person2, person1, "add:child-parent");
    }
    else if (relationType == "sibling") {
        relationships.emplace_back(person2, person1, "sibling");
        notifyRelationshipChanged(*this, person2, person1, "add:sibling");
    }
    else if (relationType == "spouse") {
        relationships.emplace_back(person2, person1, "spouse");
        notifyRelationshipChanged(*this, person2, person1, "add:spouse");
    }
}

void Relationship::removeRelationship(std::shared_ptr<Person> person1, std::shared_ptr<Person> person2, std::string relationType) {
    relationships.erase(
        std::remove_if(relationships.begin(), relationships.end(),
            [&](const auto& rel) {
                return std::get<0>(rel) == person1 && std::get<1>(rel) == person2 && std::get<2>(rel) == relationType;
            }),
        relationships.end());
    notifyRelationshipChanged(*this, person1, person2, "remove:" + relationType);

    // Удаляем обратное отношение
    if (relationType == "spouse") {
        relationships.erase(
            std::remove_if(relationships.begin(), relationships.end(),
                [&](const auto& rel) {
                    return std::get<0>(rel) == person2 && std::get<1>(rel) == person1 && std::get<2>(rel) == "spouse";
                }),
            relationships.end());
        notifyRelationshipChanged(*this, person2, person1, "remove:spouse");
    }
}

std::vector<std::tuple<std::shared_ptr<Person>, std::string>> Relationship::getRelationshipsFor(std::shared_ptr<Person> person) const {
    std::vector<std::tuple<std::shared_ptr<Person>, std::string>> result;
    for (const auto& rel : relationships) {
        if (std::get<0>(rel) == person) {
            result.emplace_back(std::get<1>(rel), std::get<2>(rel));
        }
    }
    return result;
}

const std::vector<std::tuple<std::shared_ptr<Person>, std::shared_ptr<Person>, std::string>>& Relationship::getAllRelationships() const {
    return relationships;
}

void Relationship::clear() {
    relationships.clear();
}
