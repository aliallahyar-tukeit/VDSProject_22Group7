//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_EXTRATESTS_H
#define VDSPROJECT_EXTRATESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

struct ManagerTest : testing::Test {
    std::unique_ptr<ClassProject::Manager> manager = std::make_unique<ClassProject::Manager>();

    ClassProject::BDD_ID false_id = manager->False();
    ClassProject::BDD_ID true_id = manager->True();

    ClassProject::BDD_ID a_id = manager->createVar("a");
    ClassProject::BDD_ID b_id = manager->createVar("b");
    ClassProject::BDD_ID c_id = manager->createVar("c");
    ClassProject::BDD_ID d_id = manager->createVar("d");

    ClassProject::BDD_ID neg_a_id = manager->neg(a_id);
    ClassProject::BDD_ID neg_b_id = manager->neg(b_id);

    ClassProject::BDD_ID a_and_b_id = manager->and2(a_id, b_id);
    ClassProject::BDD_ID a_or_b_id = manager->or2(a_id, b_id);
    ClassProject::BDD_ID a_xor_b_id = manager->xor2(a_id, b_id);
    ClassProject::BDD_ID a_nand_b_id = manager->nand2(a_id, b_id);
    ClassProject::BDD_ID a_nor_b_id = manager->nor2(a_id, b_id);
    ClassProject::BDD_ID a_xnor_b_id = manager->xnor2(a_id, b_id);

    ClassProject::BDD_ID c_or_d_id = manager->or2(c_id, d_id);

    // f1 = a*b+c+d
    ClassProject::BDD_ID f1_id = manager->or2(a_and_b_id, c_or_d_id);
};

TEST_F(ManagerTest, And2Test) /* NOLINT */
{
    EXPECT_EQ(manager->and2(false_id, false_id), false_id);
    EXPECT_EQ(manager->and2(false_id, true_id), false_id);
    EXPECT_EQ(manager->and2(true_id, false_id), false_id);
    EXPECT_EQ(manager->and2(true_id, true_id), true_id);

    EXPECT_EQ(manager->and2(a_id, false_id), false_id);
    EXPECT_EQ(manager->and2(a_id, true_id), a_id);
    EXPECT_EQ(manager->and2(false_id, a_id), false_id);
    EXPECT_EQ(manager->and2(true_id, a_id), a_id);

    EXPECT_EQ(manager->topVar(a_and_b_id), a_id);
    EXPECT_EQ(manager->coFactorTrue(a_and_b_id), b_id);
    EXPECT_EQ(manager->coFactorFalse(a_and_b_id), false_id);
}


TEST_F(ManagerTest, Xor2Test) /* NOLINT */
{
    EXPECT_EQ(manager->xor2(false_id, false_id), false_id);
    EXPECT_EQ(manager->xor2(false_id, true_id), true_id);
    EXPECT_EQ(manager->xor2(true_id, false_id), true_id);
    EXPECT_EQ(manager->xor2(true_id, true_id), false_id);

    EXPECT_EQ(manager->xor2(a_id, false_id), a_id);
    EXPECT_EQ(manager->xor2(a_id, true_id), neg_a_id);
    EXPECT_EQ(manager->xor2(false_id, a_id), a_id);
    EXPECT_EQ(manager->xor2(true_id, a_id), neg_a_id);

    EXPECT_EQ(manager->topVar(a_xor_b_id), a_id);
    EXPECT_EQ(manager->coFactorTrue(a_xor_b_id), neg_b_id);
    EXPECT_EQ(manager->coFactorFalse(a_xor_b_id), b_id);
}



TEST_F(ManagerTest, FindNodesTest) /* NOLINT */
{
    std::set<ClassProject::BDD_ID> a_and_b_nodes;
    std::set<ClassProject::BDD_ID> c_or_d_nodes;
    std::set<ClassProject::BDD_ID> f1_nodes;

    manager->findNodes(a_and_b_id, a_and_b_nodes);
    manager->findNodes(c_or_d_id, c_or_d_nodes);
    manager->findNodes(f1_id, f1_nodes);

    EXPECT_EQ(a_and_b_nodes.size(), 4);
    EXPECT_TRUE(a_and_b_nodes.find(false_id) != a_and_b_nodes.end());
    EXPECT_TRUE(a_and_b_nodes.find(true_id) != a_and_b_nodes.end());
    EXPECT_TRUE(a_and_b_nodes.find(b_id) != a_and_b_nodes.end());
    EXPECT_TRUE(a_and_b_nodes.find(a_and_b_id) != a_and_b_nodes.end());

    EXPECT_EQ(c_or_d_nodes.size(), 4);
    EXPECT_TRUE(c_or_d_nodes.find(false_id) != c_or_d_nodes.end());
    EXPECT_TRUE(c_or_d_nodes.find(true_id) != c_or_d_nodes.end());
    EXPECT_TRUE(c_or_d_nodes.find(d_id) != c_or_d_nodes.end());
    EXPECT_TRUE(c_or_d_nodes.find(c_or_d_id) != c_or_d_nodes.end());

    EXPECT_EQ(f1_nodes.size(), 6);
    EXPECT_TRUE(f1_nodes.find(false_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(true_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(d_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(c_or_d_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(manager->or2(b_id, c_or_d_id)) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(f1_id) != f1_nodes.end());
}

TEST_F(ManagerTest, FindVarsTest) /* NOLINT */
{
    std::set<ClassProject::BDD_ID> const_nodes;
    std::set<ClassProject::BDD_ID> a_and_b_nodes;
    std::set<ClassProject::BDD_ID> c_or_d_nodes;
    std::set<ClassProject::BDD_ID> f1_nodes;

    manager->findVars(false_id, const_nodes);
    manager->findVars(a_and_b_id, a_and_b_nodes);
    manager->findVars(c_or_d_id, c_or_d_nodes);
    manager->findVars(f1_id, f1_nodes);

    EXPECT_EQ(const_nodes.size(), 0);

    EXPECT_EQ(a_and_b_nodes.size(), 2);
    EXPECT_TRUE(a_and_b_nodes.find(a_id) != a_and_b_nodes.end());
    EXPECT_TRUE(a_and_b_nodes.find(b_id) != a_and_b_nodes.end());

    EXPECT_EQ(c_or_d_nodes.size(), 2);
    EXPECT_TRUE(c_or_d_nodes.find(c_id) != c_or_d_nodes.end());
    EXPECT_TRUE(c_or_d_nodes.find(d_id) != c_or_d_nodes.end());

    EXPECT_EQ(f1_nodes.size(), 4);
    EXPECT_TRUE(f1_nodes.find(a_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(b_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(c_id) != f1_nodes.end());
    EXPECT_TRUE(f1_nodes.find(d_id) != f1_nodes.end());
}

#endif