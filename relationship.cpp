#include "relationship.h"

void Relationship::addRelationship(std::shared_ptr<Person> person1,
    std::shared_ptr<Person> person2,
    std::string relationType) {
    relationships.emplace_back(person1, person2, relationType);
    notifyRelationshipChanged(*this, person1, person2, "add:" + relationType);

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

void Relationship::removeRelationship(std::shared_ptr<Person> person1,
    std::shared_ptr<Person> person2,
    std::string relationType) {
    relationships.erase(
        std::remove_if(relationships.begin(), relationships.end(),
            [&](const auto& rel) {
                return get<0>(rel) == person1 && get<1>(rel) == person2 && get<2>(rel) == relationType;
            }),
        relationships.end());
    notifyRelationshipChanged(*this, person1, person2, "remove:" + relationType);

    if (relationType == "spouse") {
        relationships.erase(
            std::remove_if(relationships.begin(), relationships.end(),
                [&](const auto& rel) {
                    return get<0>(rel) == person2 && get<1>(rel) == person1 && get<2>(rel) == "spouse";
                }),
            relationships.end());
        notifyRelationshipChanged(*this, person2, person1, "remove:spouse");
    }
}

std::vector<std::tuple<std::shared_ptr<Person>, std::string>>
Relationship::getRelationshipsFor(std::shared_ptr<Person> person) const {
    std::vector<std::tuple<std::shared_ptr<Person>, std::string>> result;
    for (const auto& rel : relationships) {
        if (get<0>(rel) == person) {
            result.emplace_back(get<1>(rel), get<2>(rel));
        }
    }
    return result;
}

std::vector<std::shared_ptr<Person>>
Relationship::getParents(std::shared_ptr<Person> person) const {
    std::vector<std::shared_ptr<Person>> parents;
    for (const auto& rel : relationships) {
        if (get<1>(rel) == person && get<2>(rel) == "parent-child") {
            parents.push_back(get<0>(rel));
        }
    }
    return parents;
}

std::vector<std::shared_ptr<Person>>
Relationship::getChildren(std::shared_ptr<Person> parent) const {
    std::vector<std::shared_ptr<Person>> children;
    for (const auto& rel : relationships) {
        if (get<0>(rel) == parent && get<2>(rel) == "parent-child") {
            children.push_back(get<1>(rel));
        }
    }
    return children;
}

std::vector<std::shared_ptr<Person>>
Relationship::getSiblings(std::shared_ptr<Person> person) const {
    std::vector<std::shared_ptr<Person>> siblings;
    auto parents = getParents(person);

    for (const auto& parent : parents) {
        auto children = getChildren(parent);
        for (const auto& child : children) {
            if (child != person &&
                std::find(siblings.begin(), siblings.end(), child) == siblings.end()) {
                siblings.push_back(child);
            }
        }
    }

    return siblings;
}