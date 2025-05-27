#include "tree_node.h"

CompositeNode::CompositeNode(std::shared_ptr<Person> p) : person(p) {}

void CompositeNode::addChild(std::shared_ptr<TreeNode> child) {
    children.push_back(child);
}

void CompositeNode::addSpouse(std::shared_ptr<Person> s) {
    spouse = s;
}

std::vector<std::shared_ptr<TreeNode>> CompositeNode::getChildren() const {
    return children;
}

std::shared_ptr<Person> CompositeNode::getPerson() const {
    return person;
}

std::shared_ptr<Person> CompositeNode::getSpouse() const {
    return spouse;
}

std::string CompositeNode::getName() const {
    if (person) {
        return person->getShortName();
    }
    return "";
}

bool CompositeNode::isComposite() const {
    return true;
}