#include "tree_builder.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <unordered_set>
#include <memory>
#include <unordered_set>
#include "genealogical_tree.h"
#include "tree_node.h"

TreeBuilder::TreeBuilder(std::shared_ptr<GenealogicalTree> t)
    : tree(t), relSystem(t->getRelationshipSystem()) {
}

std::shared_ptr<TreeNode> TreeBuilder::buildTree(std::shared_ptr<Person> startPerson) {
    std::unordered_set<std::shared_ptr<Person>> visited;
    return buildTreeRecursive(startPerson, visited);
}

std::shared_ptr<TreeNode> TreeBuilder::buildTreeRecursive(
    std::shared_ptr<Person> person,
    std::unordered_set<std::shared_ptr<Person>>& visited) {

    if (visited.count(person)) {
        return nullptr;
    }
    visited.insert(person);

    auto node = std::make_shared<CompositeNode>(person);

    // Добавляем супруга
    auto relationships = relSystem->getRelationshipsFor(person);
    for (const auto& rel : relationships) {
        if (get<1>(rel) == "spouse") {
            node->addSpouse(get<0>(rel));
            break;
        }
    }

    // Добавляем детей
    auto children = relSystem->getChildren(person);
    for (const auto& child : children) {
        auto childNode = buildTreeRecursive(child, visited);
        if (childNode) {
            node->addChild(childNode);
        }
    }

    return node;
}