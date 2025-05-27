#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <memory>
#include <vector>
#include "person.h"

class TreeNode {
public:
    virtual ~TreeNode() = default;
    virtual void addChild(std::shared_ptr<TreeNode> child) = 0;
    virtual void addSpouse(std::shared_ptr<Person> spouse) = 0;
    virtual std::vector<std::shared_ptr<TreeNode>> getChildren() const = 0;
    virtual std::shared_ptr<Person> getPerson() const = 0;
    virtual std::shared_ptr<Person> getSpouse() const = 0;
    virtual std::string getName() const = 0;
    virtual bool isComposite() const = 0;
};

class CompositeNode : public TreeNode {
private:
    std::shared_ptr<Person> person;
    std::shared_ptr<Person> spouse;
    std::vector<std::shared_ptr<TreeNode>> children;

public:
    CompositeNode(std::shared_ptr<Person> p);
    void addChild(std::shared_ptr<TreeNode> child) override;
    void addSpouse(std::shared_ptr<Person> s) override;
    std::vector<std::shared_ptr<TreeNode>> getChildren() const override;
    std::shared_ptr<Person> getPerson() const override;
    std::shared_ptr<Person> getSpouse() const override;
    std::string getName() const override;
    bool isComposite() const override;
};

#endif // TREE_NODE_H