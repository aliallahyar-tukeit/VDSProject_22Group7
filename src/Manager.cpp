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

BDD_ID Group7Manager::highSuccessor(BDD_ID topVariable, BDD_ID iHigh, BDD_ID tHigh, BDD_ID eHigh) {

    if (!isTerminalCase(iHigh, tHigh, eHigh)) {
        ite(iHigh, tHigh, eHigh);
        return nodes.size() - 1;
    }

    return leafNode(iHigh, tHigh, eHigh);
}

BDD_ID Group7Manager::leafNode(BDD_ID i, BDD_ID t, BDD_ID e) {

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

BDD_ID Group7Manager::lowSuccessor(BDD_ID topVariable, BDD_ID iLow, BDD_ID tLow, BDD_ID eLow) {

    if (!isTerminalCase(iLow, tLow, eLow)) {
        ite(iLow, tLow, eLow);
        return nodes.size() - 1;
    }

    return leafNode(iLow, tLow, eLow);
}


bool Group7Manager::isTerminalCase(BDD_ID i, BDD_ID t, BDD_ID e) {

    if ( ( isConstant(i) && isConstant(t) ) ||
         ( isConstant(i) && isConstant(e) ) ||
         ( isConstant(t) && isConstant(e) )
            )
        return true;

    return false;
}

BDD_ID Group7Manager::topVar(BDD_ID x, BDD_ID y, BDD_ID z) {

    BDD_ID id = BDD_UNIMPLEMENTED;

    std::map<BDD_ID, Node>::iterator iter0, iter1, iter2;
    BDD_ID top0, top1, top2;

    iter0 = nodes.find(x);
    iter1 = nodes.find(y);
    iter2 = nodes.find(z);

    if (iter0 != nodes.end() && iter1 != nodes.end() && iter2 != nodes.end()) {

        top0 = topVar(x);
        top1 = topVar(y);
        top2 = topVar(z);

        if (isConstant(top0)) {
            if ( ( isConstant(top1) && isVariable(top2) ) || ( isVariable(top1) && isConstant(top2) ) ) {
                id = std::max(top1, top2);
            } else if ( ( isConstant(top1) && isConstant(top2) ) ) {
                id = std::min(top0, top1); // 1
                id = std::min(id, top2); // 0
            } else {
                id = std::min(top1, top2);
            }
        } else { // x.top is a variable and not a constant
            if ( ( isConstant(top1) && isVariable(top2) ) || ( isVariable(top1) && isConstant(top2) ) ) {
                id = std::max(top1, top2);
                id = std::min(id, top0);
            } else if ( ( isConstant(top1) && isConstant(top2) ) ) {
                id = top0;
            } else {
                id = std::min(top0, top1);
                id = std::min(id, top2);
            }
        }
    }


    return id;
}

BDD_ID Group7Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {

    static BDD_ID idToBeRemoved = BDD_UNIMPLEMENTED;
    BDD_ID idHigh, idLow, id = BDD_UNIMPLEMENTED;
    bool isFound = false; // Denotes if the final node generated by ite is found in the table

    std::map<BDD_ID, Node>::iterator iter;

    if (nodes.find(i) != nodes.end() &&
        nodes.find(t) != nodes.end() &&
        nodes.find(e) != nodes.end()
            ) {

        BDD_ID topVariable = topVar(i, t, e);

        idHigh = highSuccessor(topVariable,
                               coFactorTrue(i, topVariable),
                               coFactorTrue(t, topVariable),
                               coFactorTrue(e, topVariable)
        );

        idLow = lowSuccessor(topVariable,
                             coFactorFalse(i, topVariable),
                             coFactorFalse(t, topVariable),
                             coFactorFalse(e, topVariable)
        );

        /* We now are sure that our ite is a terminal case, so
        * we now search through the map to find similar nodes,
        * and we don't find any, add the new node to the nodes map */

        Node newNode = Node{idHigh, idLow, topVariable, ""};

        for (iter = nodes.begin(); iter != nodes.end(); iter++) {
            if (iter->second == newNode) {
                id = iter->first;
                isFound = true;
//                    std::cout << "We have duplicate at " << nodes.size() << ": Node " << iter->first << " { " << iter->second.high << " , " << iter->second.low << " , " << iter->second.top_var << " }\n";
                idToBeRemoved = nodes.size();
                break;
            }
        }

        if (!isFound) {

            if (idToBeRemoved != BDD_UNIMPLEMENTED) {
                auto iter = nodes.find(idToBeRemoved);
                if (iter != nodes.end()) {
//                        std::cout << "Id to be removed is found! id is " << idToBeRemoved << "\n";
//                        std::cout << "That is Node { " << iter->second.high << " , " << iter->second.low << " , " << iter->second.top_var << " }";

                    nodes.erase(nodes.find(idToBeRemoved));
                    // We reset the index to ensure consistency with the map size growth
                    idToBeRemoved = BDD_UNIMPLEMENTED;
                    /* Since we removed the node, we should decrease the next node's
                    high or low successor which is referencing to the removed node */
                    if (idHigh == nodes.size()) {
                        newNode.high--;
                    } else if (idLow == nodes.size()) {
                        newNode.low--;
                    }
                }
            }

            id = nodes.size();
            nodes.insert({id, newNode});
        }
    }

    return id;
}

BDD_ID Group7Manager::coFactorTrue(BDD_ID f, BDD_ID x) {

    std::map<BDD_ID, Node>::iterator iter;
    iter = nodes.find(f);

    if (iter != nodes.end()) {
        if ( isConstant(f) || isConstant(x) ||  iter->second.top_var > x) {
            return f;
        } else {
            if (topVar(f) == x) {
                return iter->second.high; // f.high
            } else {
                BDD_ID T = coFactorTrue(iter->second.high, x);
                BDD_ID F = coFactorTrue(iter->second.low, x);
                return ite(iter->second.top_var, T, F);
            }
        }
    }

    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorFalse(BDD_ID f, BDD_ID x) {

    std::map<BDD_ID, Node>::iterator iter;
    iter = nodes.find(f);

    if (iter != nodes.end()) {
        if ( isConstant(f) || isConstant(x) ||  iter->second.top_var > x) {
            return f;
        } else {
            if (topVar(f) == x) {
                return iter->second.low; // f.low
            } else {
                BDD_ID T = coFactorFalse(iter->second.high, x);
                BDD_ID F = coFactorFalse(iter->second.low, x);
                return ite(iter->second.top_var, T, F);
            }
        }
    }

    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorTrue(BDD_ID f) {

    std::map<BDD_ID, Node>::iterator iter;
    iter = nodes.find(f);

    if (iter != nodes.end()) {
        if ( isConstant(f) ) {
            return f;
        } else {
            return iter->second.high; // f.high
        }
    }

    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::coFactorFalse(BDD_ID f) {

    std::map<BDD_ID, Node>::iterator iter;
    iter = nodes.find(f);

    if (iter != nodes.end()) {
        if ( isConstant(f) ) {
            return f;
        } else {
            return iter->second.low; // f.low
        }
    }

    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::neg(BDD_ID a) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::and2(BDD_ID a, BDD_ID b) {
    return BDD_UNIMPLEMENTED;
}

BDD_ID Group7Manager::or2(BDD_ID a, BDD_ID b) {
    return ite(a, BDD_TRUE, b);
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
