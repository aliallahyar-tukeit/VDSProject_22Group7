#include <iostream>
#include "Manager.h"

using namespace ClassProject;
constexpr BDD_ID BDD_UNIMPLEMENTED = 101;
constexpr BDD_ID BDD_ERROR = 404;
constexpr BDD_ID BDD_FALSE = 0;
constexpr BDD_ID BDD_TRUE = 1;

Manager::Manager() {
    nodes[0] = Node::False();
    nodes[1] = Node::True();
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
    nodes.insert({new_index, n});
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

BDD_ID Manager::highSuccessor(BDD_ID topVariable, BDD_ID iHigh, BDD_ID tHigh, BDD_ID eHigh) //
{
    if (!isTerminalCase(iHigh, tHigh, eHigh))
        return ite(iHigh, tHigh, eHigh);

    return leafNode(iHigh, tHigh, eHigh);
}

BDD_ID Manager::leafNode(BDD_ID i, BDD_ID t, BDD_ID e) //
{
    if (isConstant(i)) {

        if (i == BDD_TRUE) {
            return t;
        } else {
            return e;
        }

    } else {

        if (t == BDD_TRUE && e == BDD_TRUE) {
            return BDD_TRUE;
        } else if (t == BDD_TRUE && e == BDD_FALSE) {
            return i;
        } else if (t == BDD_FALSE && e == BDD_TRUE) {
            return neg(i);
        } else {
            return BDD_FALSE;
        }
    }
}

BDD_ID Manager::lowSuccessor(BDD_ID topVariable, BDD_ID iLow, BDD_ID tLow, BDD_ID eLow) //
{
    if (!isTerminalCase(iLow, tLow, eLow))
        return ite(iLow, tLow, eLow);

    return leafNode(iLow, tLow, eLow);
}


bool Manager::isTerminalCase(BDD_ID i, BDD_ID t, BDD_ID e) //
{
    return ((isConstant(i) && isConstant(t)) ||
            (isConstant(i) && isConstant(e)) ||
            (isConstant(t) && isConstant(e))
    );
}

BDD_ID Manager::topVar(BDD_ID x, BDD_ID y, BDD_ID z) //
{
    BDD_ID id = BDD_ERROR;
    BDD_ID top0, top1, top2;
    top0 = topVar(x);
    top1 = topVar(y);
    top2 = topVar(z);

    if (isConstant(top0)) {
        if ((isConstant(top1) && isVariable(top2)) || (isVariable(top1) && isConstant(top2))) {
            id = std::max(top1, top2);
        } else if ((isConstant(top1) && isConstant(top2))) {
            id = std::min(top0, top1); // 1
            id = std::min(id, top2); // 0
        } else {
            id = std::min(top1, top2);
        }
    } else { // x.top is a variable and not a constant
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

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) //
{
    if (i == 1)
        return t;
    else if (i == 0)
        return e;
    if (t == e)
        return t;

    BDD_ID topVariable = topVar(i, t, e);

    auto idHigh = highSuccessor(topVariable,
                           coFactorTrue(i, topVariable),
                           coFactorTrue(t, topVariable),
                           coFactorTrue(e, topVariable)
    );

    auto idLow = lowSuccessor(topVariable,
                         coFactorFalse(i, topVariable),
                         coFactorFalse(t, topVariable),
                         coFactorFalse(e, topVariable)
    );

    /* We now are sure that our ite is a terminal case, so
    * we now search through the map to find similar nodes,
    * and we don't find any, add the new node to the nodes map */

    Node newNode = Node{idHigh, idLow, topVariable, ""};

    for (const auto &[id, node]: nodes) {
        if (node == newNode)
            return id;
    }

    auto id = nodes.size();
    nodes.insert({id, newNode});

    return id;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) //
{
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

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) //
{
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

BDD_ID Manager::coFactorTrue(BDD_ID f) //
{
    if (isConstant(f))
        return f;
    else
        return nodes[f].high; // f.high
}

BDD_ID Manager::coFactorFalse(BDD_ID f) //
{
    if (isConstant(f)) {
        return f;
    } else {
        return nodes[f].low; // f.low
    }
}

BDD_ID Manager::neg(BDD_ID a)  //
{
//    BDD_ID low = isConstant(nodes[a].low) ? (nodes[a].low == True() ? False() : True()) :
//                 neg(nodes[a].low);
//    BDD_ID high = isConstant(nodes[a].high) ? (nodes[a].high == True() ? False() : True()) :
//                  neg(nodes[a].high);
//
//    auto id = nodes.size();
//    Node new_node = {high, low, topVar(a), ""};
//    nodes.insert({id, new_node});
//
//    return id;
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

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) //
{
    auto node = nodes.find(root);

    if (node == nodes.end())
        return;

    auto &low = node->second.low;
    auto &high = node->second.high;

    nodes_of_root.insert(root);
    nodes_of_root.insert(high);
    nodes_of_root.insert(low);

    if (!isConstant(high))
        findNodes(high, nodes_of_root);

    if (!isConstant(low))
        findNodes(low, nodes_of_root);
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) //
{
    auto set = std::set<ClassProject::BDD_ID>();
    findNodes(root, set);

    for (auto &iter: set) {
        if (isConstant(iter))
            continue;
        vars_of_root.insert(topVar(iter));
    }
}

size_t Manager::uniqueTableSize() {
    return nodes.size();
}

void Manager::printNodes() //
{
    std::cout << "BDD_ID\tLabel\tHigh\tLow \tTopVar" << std::endl;
    for (const auto &[id, node]: nodes) {
        std::cout << id << "\t\t" << node.label;
        if (node.label.length() <= 4)
            std::cout << "\t\t";
        else if (node.label.length() <= 7)
            std::cout << '\t';
        std::cout << node.high << "\t\t" << node.low << "\t\t" << node.top_var << std::endl;
    }
}

Node Node::True() {
    return Node{1, 1, 1};
}

Node Node::False() {
    return Node{0, 0, 0};
}