// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include "ManagerInterface.h"
#include <map>
#include <unordered_map>
#include <inttypes.h>
#include <vector>

namespace ClassProject {
    struct Node {
        BDD_ID high;
        BDD_ID low;
        BDD_ID top_var;
        std::string label;

        static Node True();

        static Node False();


    };

    class HashFunc {
    public:
        size_t operator()(const BDD_ID &i, const BDD_ID &t, const BDD_ID &e) const {
            return std::hash<BDD_ID>{}(i << 21) ^
                    std::hash<BDD_ID>{}(t >> 21) ^
                    std::hash<BDD_ID>{}(e << 21);
        }
    };

    class Manager : public ClassProject::ManagerInterface {
    public:
        Manager();

        ~Manager() = default;

        // Creates a new variable with the given label and returns its ID
        BDD_ID createVar(const std::string &label) override;

        // Returns the ID of the True node
        const BDD_ID &True() override;

        // Returns the ID of the False node
        const BDD_ID &False() override;

        // Returns true, if the given ID represents a leaf node
        bool isConstant(BDD_ID f) override;

        // Returns true, if the given ID represents a variable
        bool isVariable(BDD_ID x) override;

        // Returns the top variable ID of the given node
        BDD_ID topVar(BDD_ID f) override;

        // Returns the high successor ID of the given node
        BDD_ID getHighSuccessor(BDD_ID f) override;

        // Returns the low successor ID of the given node
        BDD_ID getLowSuccessor(BDD_ID f) override;

        // Implements the if-then-else algorithm, which most of the following functions are based on. Returns the
        // existing or new node that represents the given expression. Please refer to the lecture slides for a detailed
        // description.
        BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;

        // Returns the positive co-factor of the function represented by ID f w.r.t. variable x. The second parameter is
        // optional. If x is not specified, the co-factor is determined w.r.t. the top variable of f.
        // Example: f = a + (b ∗ c) with alphabetical variable order
        // coFactorTrue(f ) = 1 = coFactorTrue(f, a)
        // coFactorTrue(f, c) = a + b
        BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;

        BDD_ID coFactorTrue(BDD_ID f) override;

        // Returns the negative co-factor of the function represented by ID f w.r.t. variable x. The second parameter
        // is optional. If x is not specified, the co-factor is determined w.r.t. the top variable of f.
        // Example: f = a + (b ∗ c) with alphabetical variable order
        // coFactorFalse(f ) = b ∗ c = coFactorFalse(f, a)
        // coFactorFalse(f, c) = a
        BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;

        BDD_ID coFactorFalse(BDD_ID f) override;

        // Returns the ID representing the negation of the given function
        BDD_ID neg(BDD_ID a) override;

        // Returns the ID representing the resulting function of a ∗ b
        BDD_ID and2(BDD_ID a, BDD_ID b) override;

        // returns the ID representing the resulting function of a + b
        BDD_ID or2(BDD_ID a, BDD_ID b) override;

        // returns the ID representing the resulting function of a XORS b
        BDD_ID xor2(BDD_ID a, BDD_ID b) override;

        // returns the ID representing the resulting function of a NAND b
        BDD_ID nand2(BDD_ID a, BDD_ID b) override;

        // returns the ID representing the resulting function of a NOR b
        BDD_ID nor2(BDD_ID a, BDD_ID b) override;

        // returns the ID representing the resulting function of a XNOR b
        BDD_ID xnor2(BDD_ID a, BDD_ID b) override;

        // Returns the label of the top variable of root
        std::string getTopVarName(const BDD_ID &root) override;

        // This function takes a node root and an empty set nodes of root. It returns the set of all nodes which are
        // reachable from root including itself
        void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;

        // This function takes a node root and an empty set nodes of root. It returns the set of all variables which are
        // reachable from root including itself. Hint: It essentially returns the set of top variables of findNodes
        void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;

        // Returns the number of nodes currently existing in the unique table of the Manager class
        size_t uniqueTableSize() override;


        // CUSTOM FUNCTIONS //////////////////////////////////////////

        // Returns the top variable among the boolean functions x, y and z. This method is primarily used in
        // finding the top variable for an ite operator.
        BDD_ID topVar(BDD_ID x, BDD_ID y, BDD_ID z);

        // Prints all the nodes
        void printNodes();

        void printTable();

        static uint64_t generateKey(BDD_ID a, BDD_ID b, BDD_ID c);

    private:
        std::vector<Node> nodes;
        //std::unordered_map<const unsigned long, BDD_ID , HashFunc> unique_table;

        std::unordered_map<uint64_t, size_t> unique_table;
        std::unordered_map<uint64_t, BDD_ID> computed_table;

        size_t findOrAdd(Node node);
    };
}

#endif
