#include "Manager.h"

using namespace ClassProject;
constexpr BDD_ID BDD_UNIMPLEMENTED = 101;
constexpr BDD_ID BDD_FALSE = 0;
constexpr BDD_ID BDD_TRUE = 1;

Group7Manager::Group7Manager() {
    nodes[0] = Node::False();
    nodes[1] = Node::True();
}

bool operator==(const Node& firstNode, const Node& secondNode) // Check the equality of two nodes
{
    return (firstNode.high == secondNode.high) &&
           (firstNode.low == secondNode.low) &&
           (firstNode.top_var == secondNode.top_var);
}

bool operator!=(const Node& firstNode, const Node& secondNode) // Check the inequality of two nodes
{
    return (firstNode.high != secondNode.high) ||
           (firstNode.low != secondNode.low) ||
           (firstNode.top_var != secondNode.top_var);
}

BDD_ID Group7Manager::createVar(const std::string &label) { // Single variable functions
    BDD_ID new_index = nodes.size();
    Node n = {1, 0, new_index, label};
    nodes.insert({new_index, n});
    return new_index;
}

const BDD_ID &Group7Manager::True() {
    return BDD_TRUE;
}

const BDD_ID &Group7Manager::False() {
    return BDD_FALSE;
}

bool Group7Manager::isConstant(BDD_ID f) {

    std::map<BDD_ID, Node>::iterator iter = nodes.find(f);

    if (iter != nodes.end())
        if (iter->second == Node::True() || iter->second == Node::False())
            return true;

    return false;
}

bool Group7Manager::isVariable(BDD_ID x) {

    std::map<BDD_ID, Node>::iterator iter = nodes.find(x);

    if (iter != nodes.end())
        if (iter->second.high <= 1 && iter->second.low <= 1 && iter->second.top_var >= 2)
            return true;

    return false;
}

BDD_ID Group7Manager::topVar(BDD_ID f) { // Returns the top variable ID of the given node
    BDD_ID id = BDD_UNIMPLEMENTED;
    std::map<BDD_ID, Node>::iterator iter = nodes.find(f);
    if (iter != nodes.end())
        id = iter->second.top_var;

    return id;
}

BDD_ID Group7Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorTrue(BDD_ID f) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorFalse(BDD_ID f) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::neg(BDD_ID a) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::and2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::or2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::xor2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::nand2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::nor2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::xnor2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

std::string Group7Manager::getTopVarName(const BDD_ID &root) {
    return std::string();
}

void Group7Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
}

void Group7Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
}

size_t Group7Manager::uniqueTableSize() {
    return nodes.size();
}

Node Node::True() {
    return Node{1, 1, 1};
}

Node Node::False() {
    return Node{0, 0, 0};
}
