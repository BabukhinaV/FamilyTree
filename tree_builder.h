#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include <memory>
#include <unordered_set>
#include "genealogical_tree.h"
#include "tree_node.h"

class TreeBuilder {
private:
    std::shared_ptr<GenealogicalTree> tree;
    std::shared_ptr<Relationship> relSystem;

    std::shared_ptr<TreeNode> buildTreeRecursive(std::shared_ptr<Person> person,
        std::unordered_set<std::shared_ptr<Person>>& visited);

public:
    TreeBuilder(std::shared_ptr<GenealogicalTree> t);
    std::shared_ptr<TreeNode> buildTree(std::shared_ptr<Person> startPerson);
};

#endif // TREE_BUILDER_H