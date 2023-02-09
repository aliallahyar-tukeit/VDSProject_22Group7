#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(2);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> transitionFunctions;

};

TEST_F(ReachabilityTest, HowTo_Example) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm2->neg(s1)); // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, ComplexExample) {
    //==================================================
    //
    // we will use this transition function:
    //   / s0' \ = /   !s0   \
    //   \ s1' / = \ s0 + s1 /
    //
    // it cycles like this:
    //   / 0 \  _  / 1 \  _  / 0 \  _  / 1 \  _  / 0 \
    //   \ 0 /     \ 0 /     \ 1 /     \ 1 /     \ 1 /
    //    (A)       (B)       (C)       (D)       (C)
    //
    // (A) is reachable only if we start from (A)
    // (B) is reachable only if we start from (A) or (B)
    // (C) and (D) are always reachable
    //
    //==================================================

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);
    transitionFunctions.push_back(fsm2->neg(s0));
    transitionFunctions.push_back(fsm2->or2(s0, s1));
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false}); // state (A)
    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_TRUE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));

    fsm2->setInitState({true,false}); // state (B)
    ASSERT_FALSE(fsm2->isReachable({false, false})); // (A) reachable only from (A)
    ASSERT_TRUE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));

    fsm2->setInitState({false,true}); // state (C)
    ASSERT_FALSE(fsm2->isReachable({false, false})); // (A) reachable only from (A)
    ASSERT_FALSE(fsm2->isReachable({true, false})); // (B) reachable only from (A) or (B)
    ASSERT_TRUE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));

    fsm2->setInitState({true,true}); // state (D)
    ASSERT_FALSE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, throws) {
    EXPECT_THROW(fsm2->setInitState(std::vector<bool>()), std::runtime_error); // no state vector provided
    EXPECT_THROW(fsm2->setInitState({false}), std::runtime_error); // too few states provided
    EXPECT_THROW(fsm2->setInitState({false, false, false}), std::runtime_error); // too many states provided

    EXPECT_THROW(fsm2->setTransitionFunctions(std::vector<BDD_ID>()), std::runtime_error); // no transition functions
    EXPECT_THROW(fsm2->setTransitionFunctions({fsm2->False()}), std::runtime_error); // too few transition functions
    EXPECT_THROW(fsm2->setTransitionFunctions({fsm2->True(), fsm2->False(), fsm2->True()}), std::runtime_error); // too many transition functions
    EXPECT_THROW(fsm2->setTransitionFunctions({fsm2->True(), 100}), std::runtime_error); // invalid ID

    EXPECT_THROW(fsm2->isReachable(std::vector<bool>()), std::runtime_error); // no state vector provided
    EXPECT_THROW(fsm2->isReachable({false}), std::runtime_error); // too few states provided
    EXPECT_THROW(fsm2->isReachable({false, false, false}), std::runtime_error); // too many states provided
}

#endif
