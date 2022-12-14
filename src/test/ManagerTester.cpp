//
// Created by eloi on 12/1/22.
//

#include "ManagerTester.h"

ClassProject::Group7Manager *ManagerTester::manager;

void ManagerTester::SetUpTestSuite() {
    if (manager == nullptr) {
        manager = new ClassProject::Group7Manager;
    }
}

void ManagerTester::TearDownTestSuite() {
    if (manager != nullptr)
        delete manager;
    manager = nullptr;
}

void ManagerTester::SetUp() {
    if (manager != nullptr)
        delete manager;
    manager = new ClassProject::Group7Manager;
}

void ManagerTester::TearDown() {
}

TEST_F(ManagerTester, fn_True) {
    ClassProject::BDD_ID expected_id = 1;
    auto id = manager->True();

    ASSERT_EQ(expected_id, id);
}

TEST_F(ManagerTester, fn_False) {
    ClassProject::BDD_ID expected_id = 0;
    auto id = manager->False();

    ASSERT_EQ(expected_id, id);
}

TEST_F(ManagerTester, fn_uniqueTableSize) {
    size_t expected_result = 2;
    size_t result = manager->uniqueTableSize();

    EXPECT_EQ(result, expected_result);
}


TEST_F(ManagerTester, fn_isConstant) {
    ClassProject::BDD_ID id = 1;
    bool isConstant = manager->isConstant(id);

    ASSERT_EQ(isConstant, true);
}

TEST_F(ManagerTester, fn_isVariableFirstVersion) { // It does not exist
    ClassProject::BDD_ID id = 2;
    bool isVariable = manager->isVariable(id);

    ASSERT_EQ(isVariable, false);
}

TEST_F(ManagerTester, fn_isVariableSecondVersion) { // It should exist and be a variable since we are creating one
    ClassProject::BDD_ID createdId = manager->createVar("a");
    bool isVariable = manager->isVariable(createdId);

    ASSERT_EQ(isVariable, true);
}

TEST_F(ManagerTester, fn_checkMapSize) { // It should be 3
    manager->createVar("a");
    int size = manager->nodes.size();

    ASSERT_EQ(size, 3);
}

TEST_F(ManagerTester, fn_topVarIdForVariable) { // Should equal 2
    ClassProject::BDD_ID createdA = manager->createVar("a");
    ClassProject::BDD_ID topVarId = manager->topVar(createdA);
    EXPECT_EQ(topVarId, 2);
}

TEST_F(ManagerTester, fn_topVarIdForFalse) {
    ClassProject::Node falseNode = ClassProject::Node::False();
    ClassProject::BDD_ID topVariableIdForFalse = falseNode.top_var;
    EXPECT_EQ(topVariableIdForFalse, 0);
}

TEST_F(ManagerTester, fn_topVarIdForTrue) {
    ClassProject::Node trueNode = ClassProject::Node::True();
    ClassProject::BDD_ID topVariableIdForTrue = trueNode.top_var;
    EXPECT_EQ(topVariableIdForTrue, 1);
}

TEST_F(ManagerTester, fn_OR_TopVariable) { // Should be 2
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 4
    ClassProject::BDD_ID topVariableIdForOR = manager->topVar(createdNodeOR_AB);
    EXPECT_EQ(topVariableIdForOR, 2);
}

TEST_F(ManagerTester, fn_OR_LowId) { // Should be 3
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 4
    ClassProject::BDD_ID lowIdForOR = manager->nodes.find(createdNodeOR_AB)->second.low;
    EXPECT_EQ(lowIdForOR, 3);
}

TEST_F(ManagerTester, fn_OR_HighId) { // Should be 1
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 4
    ClassProject::BDD_ID highIdForOR = manager->nodes.find(createdNodeOR_AB)->second.high;
    EXPECT_EQ(highIdForOR, 1);
}

TEST_F(ManagerTester, fn_OR_Three_Variables_LowId) { // Should be 4
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 5
    // auto creation of b + c => id is 6
    ClassProject::BDD_ID createdNodeOR_AB_C = manager->or2(createdNodeOR_AB, createdC); // id is 7
    ClassProject::BDD_ID lowIdForOR = manager->nodes.find(createdNodeOR_AB_C)->second.low;
    EXPECT_EQ(lowIdForOR, 6); // => a low => b + c
}

TEST_F(ManagerTester, fn_OR_Three_Variables_HighId) { // Should be 1
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 5
    ClassProject::BDD_ID createdNodeOR_AB_C = manager->or2(createdNodeOR_AB, createdC); // id is 6
    ClassProject::BDD_ID highIdForOR = manager->nodes.find(createdNodeOR_AB_C)->second.high;
    EXPECT_EQ(highIdForOR, 1);
}


TEST_F(ManagerTester, fn_AND_TopVariable) { // Should be 2
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 4
    ClassProject::BDD_ID topVariableIdForAND = manager->topVar(createdNodeAND_AB);
    EXPECT_EQ(topVariableIdForAND, 2);
}

TEST_F(ManagerTester, fn_AND_LowId) { // Should be 0
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 4
    ClassProject::BDD_ID lowIdForAND = manager->nodes.find(createdNodeAND_AB)->second.low;
    EXPECT_EQ(lowIdForAND, 0);
}

TEST_F(ManagerTester, fn_AND_HighId) { // Should be 3
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 4
    ClassProject::BDD_ID highIdForAND = manager->nodes.find(createdNodeAND_AB)->second.high;
    EXPECT_EQ(highIdForAND, 3);
}

TEST_F(ManagerTester, fn_AND_TopThreeVariable) { // The product of three variables: Should be 2
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 5
    ClassProject::BDD_ID createdNodeAND_AB_C = manager->and2(createdNodeAND_AB, createdC); // id is 6
    ClassProject::BDD_ID topVariableIdForAND = manager->topVar(createdNodeAND_AB_C);
    EXPECT_EQ(topVariableIdForAND, 2);
}

TEST_F(ManagerTester, fn_AND_Three_Variables_LowId) { // Should be 0
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 5
    // auto creation of b * c => id is 6
    ClassProject::BDD_ID createdNodeAND_AB_C = manager->and2(createdNodeAND_AB, createdC); // id is 7
    ClassProject::BDD_ID lowIdForAND = manager->nodes.find(createdNodeAND_AB_C)->second.low;
    EXPECT_EQ(lowIdForAND, 0);
}

TEST_F(ManagerTester, fn_AND_Three_Variables_HighId) { // Should be 4
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 5
    // auto creation of b * c => id is 6
    ClassProject::BDD_ID createdNodeAND_AB_C = manager->and2(createdNodeAND_AB, createdC); // id is 7
    ClassProject::BDD_ID highIdForAND = manager->nodes.find(createdNodeAND_AB_C)->second.high;
    EXPECT_EQ(highIdForAND, 6);
}

TEST_F(ManagerTester, fn_AND_OR_TopThreeVariable) { // The product of two variables and a sum by the third: Should be 2
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 5
    ClassProject::BDD_ID createdNodeAND_AB_OR_C = manager->and2(createdNodeAND_AB, createdC); // id is 6
    ClassProject::BDD_ID topVariableIdForAND_OR = manager->topVar(createdNodeAND_AB_OR_C);
    EXPECT_EQ(topVariableIdForAND_OR, 2);
}

TEST_F(ManagerTester, fn_AND_AB_OR_C_LowId) { // Should be 4
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 5
    ClassProject::BDD_ID createdNodeAND_AB_OR_C = manager->or2(createdNodeAND_AB, createdC); // id is 6
    ClassProject::BDD_ID lowIdForAND_OR = manager->nodes.find(createdNodeAND_AB_OR_C)->second.low;
    EXPECT_EQ(lowIdForAND_OR, 4);
}

TEST_F(ManagerTester, fn_AND_AB_OR_C_HighId) { // Should be 3
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 5
    // auto creation of b + c => id is 6
    ClassProject::BDD_ID createdNodeAND_AB_OR_C = manager->or2(createdNodeAND_AB, createdC); // id is 7
    ClassProject::BDD_ID highIdForAND_OR = manager->nodes.find(createdNodeAND_AB_OR_C)->second.high;
    EXPECT_EQ(highIdForAND_OR, 6);
}

TEST_F(ManagerTester, fn_OR_AB_AND_C_LowId) { // Should be 3
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 5
    // auto creation of b + c => id is 6
    ClassProject::BDD_ID createdNodeOR_AB_AND_C = manager->and2(createdNodeOR_AB, createdC); // id is 7
    ClassProject::BDD_ID lowIdForOR_AND = manager->nodes.find(createdNodeOR_AB_AND_C)->second.low;
    EXPECT_EQ(lowIdForOR_AND, 6);
}

TEST_F(ManagerTester, fn_OR_AB_AND_C_HighId) { // Should be 4
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdC = manager->createVar("c"); // id is 4
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 5
    ClassProject::BDD_ID createdNodeOR_AB_AND_C = manager->and2(createdNodeOR_AB, createdC); // id is 6
    ClassProject::BDD_ID highIdForOR_AND = manager->nodes.find(createdNodeOR_AB_AND_C)->second.high;
    EXPECT_EQ(highIdForOR_AND, 4);
}

TEST_F(ManagerTester, fn_NEG_TopVariable) { // Should be 2
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdNodeNEG_A = manager->neg(createdA); // id is 3
    ClassProject::BDD_ID topVariableIdForNEG = manager->topVar(createdNodeNEG_A);
    EXPECT_EQ(topVariableIdForNEG, 2);
}

TEST_F(ManagerTester, fn_NEG_LowId) { // Should be 1
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdNodeNEG_A = manager->neg(createdA); // id is 3
    ClassProject::BDD_ID lowIdForNEG = manager->nodes.find(createdNodeNEG_A)->second.low;
    EXPECT_EQ(lowIdForNEG, 1);
}

TEST_F(ManagerTester, fn_NEG_HighId) { // Should be 0
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdNodeNEG_A = manager->neg(createdA); // id is 3
    ClassProject::BDD_ID highIdForNEG = manager->nodes.find(createdNodeNEG_A)->second.high;
    EXPECT_EQ(highIdForNEG, 0);
}

TEST_F(ManagerTester, fn_NEG_AND_AB_TopVariable) { // Should be 2
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 4
    ClassProject::BDD_ID createdNodeNEG_AND_AB = manager->neg(createdNodeAND_AB); // id is 5
    ClassProject::BDD_ID topVariableIdForNEG_AND = manager->topVar(createdNodeNEG_AND_AB);
    EXPECT_EQ(topVariableIdForNEG_AND, 2);
}

TEST_F(ManagerTester, fn_NEG_AND_AB_LowId) { // Should be 1
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 4
    ClassProject::BDD_ID createdNodeNEG_AND_AB = manager->neg(createdNodeAND_AB); // id is 5
    ClassProject::BDD_ID lowIdForNEG_AND = manager->nodes.find(createdNodeNEG_AND_AB)->second.low;
    EXPECT_EQ(lowIdForNEG_AND, 1);
}

TEST_F(ManagerTester, fn_NEG_AND_AB_HighId) { // Should be 5
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeAND_AB = manager->and2(createdA, createdB); // id is 4
    // not b => id is 5
    ClassProject::BDD_ID createdNodeNEG_AND_AB = manager->neg(createdNodeAND_AB); // id is 6
    ClassProject::BDD_ID highIdForNEG_AND = manager->nodes.find(createdNodeNEG_AND_AB)->second.high;
    EXPECT_EQ(highIdForNEG_AND, 5);
}

TEST_F(ManagerTester, fn_NEG_OR_AB_LowId) { // Should be 3
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 4
    ClassProject::BDD_ID createdNodeNEG_OR_AB = manager->neg(createdNodeOR_AB); // id is 5
    ClassProject::BDD_ID lowIdForNEG_OR = manager->nodes.find(createdNodeNEG_OR_AB)->second.low;
    EXPECT_EQ(lowIdForNEG_OR, 5);
}

TEST_F(ManagerTester, fn_NEG_OR_AB_HighId) { // Should be 0
    ClassProject::BDD_ID createdA = manager->createVar("a"); // id is 2
    ClassProject::BDD_ID createdB = manager->createVar("b"); // id is 3
    ClassProject::BDD_ID createdNodeOR_AB = manager->or2(createdA, createdB); // id is 4
    ClassProject::BDD_ID createdNodeNEG_OR_AB = manager->neg(createdNodeOR_AB); // id is 5
    ClassProject::BDD_ID highIdForNEG_OR = manager->nodes.find(createdNodeNEG_OR_AB)->second.high;
    EXPECT_EQ(highIdForNEG_OR, 0);
}