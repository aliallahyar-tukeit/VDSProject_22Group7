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

TEST_F(ManagerTester, fn_initialNodesMapSize) {
    int size = manager->nodes.size();

    ASSERT_EQ(size, 2);
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