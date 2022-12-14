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
