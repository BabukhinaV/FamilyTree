#include "tree_visualizer.h"
#include <iomanip>

void TreeVisualizer::visualize(std::shared_ptr<TreeNode> root) {
    if (!root) return;

    std::cout << "\n=== Генеалогическое древо ===\n";
    printNode(root, 0, std::vector<bool>(), true);
}

void TreeVisualizer::printNode(std::shared_ptr<TreeNode> node, int level,
    std::vector<bool> lastFlags, bool isLast) {
    if (!node) return;

    // Вывод текущего узла
    printPerson(node, level, lastFlags, isLast);

    // Обновляем флаги для детей
    if (level >= lastFlags.size()) {
        lastFlags.push_back(false);
    }
    lastFlags[level] = isLast;

    // Вывод детей
    auto children = node->getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        bool lastChild = (i == children.size() - 1);
        printNode(children[i], level + 1, lastFlags, lastChild);
    }
}

void TreeVisualizer::printPerson(std::shared_ptr<TreeNode> node, int level,
    const std::vector<bool>& lastFlags, bool isLast) {
    // Отступы
    for (int i = 0; i < level; ++i) {
        if (i >= lastFlags.size()) {
            std::cout << "    ";
        }
        else {
            std::cout << (lastFlags[i] ? "    " : "│   ");
        }
    }

    // Линия
    if (level > 0) {
        std::cout << (isLast ? "└── " : "├── ");
    }

    // Имя
    std::cout << node->getName();

    // Супруг
    if (node->getSpouse()) {
        std::cout << " + " << node->getSpouse()->getShortName();
    }
    std::cout << std::endl;
}