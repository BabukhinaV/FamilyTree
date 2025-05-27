#ifndef TREE_VISUALIZER_H
#define TREE_VISUALIZER_H

#include <memory>
#include <vector>
#include "tree_node.h"

class TreeVisualizer {
public:
    static void visualize(std::shared_ptr<TreeNode> root);

private:
    static void printNode(std::shared_ptr<TreeNode> node, int level,
        std::vector<bool> lastFlags, bool isLast);
    static void printPerson(std::shared_ptr<TreeNode> node, int level,
        const std::vector<bool>& lastFlags, bool isLast);
};

#endif // TREE_VISUALIZER_H