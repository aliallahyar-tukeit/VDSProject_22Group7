#include <iostream>
#include "Manager.h"
#include <inttypes.h>

using namespace ClassProject;
constexpr BDD_ID BDD_UNIMPLEMENTED = 101;
constexpr BDD_ID BDD_ERROR = 404;
constexpr BDD_ID BDD_FALSE = 0;
constexpr BDD_ID BDD_TRUE = 1;

Manager::Manager() {
    nodes.push_back(Node::False());
    nodes.push_back(Node::True());
}

bool operator==(const Node &firstNode, const Node &secondNode) // Check the equality of two nodes
{
    return (firstNode.high == secondNode.high) &&
           (firstNode.low == secondNode.low) &&
           (firstNode.top_var == secondNode.top_var);
}

bool operator!=(const Node &firstNode, const Node &secondNode) // Check the inequality of two nodes
{
    return (firstNode.high != secondNode.high) ||
           (firstNode.low != secondNode.low) ||
           (firstNode.top_var != secondNode.top_var);
}

BDD_ID Manager::createVar(const std::string &label) // Single variable functions
{
    BDD_ID new_index = nodes.size();
    Node n = {1, 0, new_index, label};
    nodes.push_back(n);
    return new_index;
}

const BDD_ID &Manager::True() {
    return BDD_TRUE;
}

const BDD_ID &Manager::False() {
    return BDD_FALSE;
}

bool Manager::isConstant(BDD_ID f) {
    return (nodes[f] == Node::True() || nodes[f] == Node::False());
}

bool Manager::isVariable(BDD_ID x) {
    return (nodes[x].high <= 1 && nodes[x].low <= 1 && nodes[x].top_var >= 2);
}

BDD_ID Manager::topVar(BDD_ID f) { // Returns the top variable ID of the given node
    return nodes[f].top_var;
}

BDD_ID Manager::topVar(BDD_ID x, BDD_ID y, BDD_ID z) {
    BDD_ID id, top0, top1, top2;
    top0 = topVar(x);
    top1 = topVar(y);
    top2 = topVar(z);

    if (isConstant(top0)) {
        if ((isConstant(top1) && isVariable(top2)) || (isVariable(top1) && isConstant(top2))) {
            id = std::max(top1, top2);
        } else if ((isConstant(top1) && isConstant(top2))) {
            id = std::min(top0, top1);
            id = std::min(id, top2);
        } else {
            id = std::min(top1, top2);
        }
    } else {
        if ((isConstant(top1) && isVariable(top2)) || (isVariable(top1) && isConstant(top2))) {
            id = std::max(top1, top2);
            id = std::min(id, top0);
        } else if ((isConstant(top1) && isConstant(top2))) {
            id = top0;
        } else {
            id = std::min(top0, top1);
            id = std::min(id, top2);
        }
    }

    return id;
}

BDD_ID Manager::getHighSuccessor(BDD_ID f) {
    return nodes[f].high;
}

BDD_ID Manager::getLowSuccessor(BDD_ID f) {
    return nodes[f].low;
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    if (i == 1)
        return t;
    else if (i == 0)
        return e;
    else if (t == e)
        return t;
    else if ((t == 1) && (e == 0))
        return i;

    auto key = generateKey(i, t, e);
    auto iter = computed_table.find(key);

    if (iter != computed_table.end()) {
        return iter->second;
    }

    BDD_ID topVariable = topVar(i, t, e);

    auto idHigh = ite(
            coFactorTrue(i, topVariable),
            coFactorTrue(t, topVariable),
            coFactorTrue(e, topVariable)
    );

    auto idLow = ite(
            coFactorFalse(i, topVariable),
            coFactorFalse(t, topVariable),
            coFactorFalse(e, topVariable)
    );

    if (idLow == idHigh)
        return idLow;

    Node newNode = Node{idHigh, idLow, topVariable, ""};

    auto id = findOrAdd(newNode);
    computed_table[key] = id;

    return id;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    if (isConstant(f) || isConstant(x) || nodes[f].top_var > x) {
        return f;
    } else {
        if (topVar(f) == x) {
            return nodes[f].high; // f.high
        } else {
            BDD_ID T = coFactorTrue(nodes[f].high, x);
            BDD_ID F = coFactorTrue(nodes[f].low, x);
            return ite(nodes[f].top_var, T, F);
        }
    }
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
    if (isConstant(f) || isConstant(x) || nodes[f].top_var > x) {
        return f;
    } else {
        if (topVar(f) == x) {
            return nodes[f].low; // f.low
        } else {
            BDD_ID T = coFactorFalse(nodes[f].high, x);
            BDD_ID F = coFactorFalse(nodes[f].low, x);
            return ite(nodes[f].top_var, T, F);
        }
    }
}

BDD_ID Manager::coFactorTrue(BDD_ID f) {
    if (!isConstant(f))
        return nodes[f].high;

    return f;
}

BDD_ID Manager::coFactorFalse(BDD_ID f) {
    if (!isConstant(f))
        return nodes[f].low;

    return f;
}

BDD_ID Manager::neg(BDD_ID a) {
    return ite(a, 0, 1);
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b) { // a and b are not variables, they are functions
    return ite(a, b, BDD_FALSE);
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
    return ite(a, BDD_TRUE, b);
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
    BDD_ID bPrime = neg(b);
    return ite(a, bPrime, b);
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
    BDD_ID bPrime = neg(b);
    return ite(a, bPrime, 1);
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
    BDD_ID bPrime = neg(b);
    return ite(a, BDD_FALSE, bPrime);
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
    BDD_ID bPrime = neg(b);
    return ite(a, b, bPrime);
}

std::string Manager::getTopVarName(const BDD_ID &root) {
    const BDD_ID topVarIndex = topVar(root);
    return nodes[topVarIndex].label;
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
    auto &node = nodes[root];
    auto &high = node.high;
    auto &low = node.low;

    nodes_of_root.insert(root);

    if (!isConstant(root)) {
        findNodes(high, nodes_of_root);
        findNodes(low, nodes_of_root);
    }
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
    auto set = std::set<ClassProject::BDD_ID>();
    findNodes(root, set);

    for (auto &iter: set) {
        if (!isConstant(iter))
            vars_of_root.insert(topVar(iter));
    }
}

size_t Manager::uniqueTableSize() {
    return nodes.size();
}

void Manager::printNodes() {
    std::cout << "BDD_ID\tLabel\tHigh\tLow \tTopVar" << std::endl;
    for (auto id = 0; id < nodes.size(); id++) {
        auto &node = nodes[id];
        std::cout << id << "\t\t" << node.label;
        if (node.label.length() < 4)
            std::cout << "\t\t";
        else if (node.label.length() >= 4)
            std::cout << '\t';
        std::cout << node.high << "\t\t" << node.low << "\t\t" << node.top_var << std::endl;
    }

    std::cout << "\n\n--------------------------\n\n" << std::endl;
    std::cout << "Here is the computed table:\n" << std::endl;

}

uint64_t Manager::generateKey(BDD_ID i, BDD_ID t, BDD_ID e) {
    return (i << 42) ^ (t << 21) ^ e;
}

size_t Manager::findOrAdd(Node node) {
    auto key = generateKey(node.high, node.low, node.top_var);
    auto it = unique_table.find(key);

    if (it != unique_table.end())
        return it->second;
    auto id = nodes.size();
    unique_table.insert({key, id});
    nodes.push_back(node);
    return id;
}

Node Node::True() {
    return Node{1, 1, 1, "True"};
}

Node Node::False() {
    return Node{0, 0, 0, "False"};
}