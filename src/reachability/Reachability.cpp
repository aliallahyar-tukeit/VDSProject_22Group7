#include "Reachability.h"
#include <math.h>
#include <stdexcept>
#include <algorithm>

using namespace ClassProject;

void Reachability::setInitState(const std::vector<bool> &stateVector) {
    if (stateVector.size() != states.size()) {
        throw std::runtime_error("The size of the states vector does not match the number of state bits");
    }

    initial_state = stateVector;
    needs_update = true;
}

void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &functions) {
    if (functions.size() != states.size()) {
        throw std::runtime_error("Too few transition functions!");
    }

    if(*std::max_element(functions.begin(), functions.end()) >= Manager::uniqueTableSize()) {
        throw std::runtime_error("Invalid BDD_ID");
    }

    transition_functions = functions;

    if (initial_state.size() == transition_functions.size()) {
        needs_update = true;
    }
}

bool Reachability::isReachable(const std::vector<bool> &stateVector) {

    if (stateVector.size() != states.size())
        throw std::runtime_error("Too few bits of states for the states vector!");

    if (initial_state.size() != states.size())
        throw std::runtime_error("Size mismatch between initial states vector and states vector!");

    if (transition_functions.size() != states.size())
        throw std::runtime_error("Too few transition functions already defined!");

    if (needs_update) {
        getReachableStates();
        needs_update = false;
    }
    BDD_ID fixed_point = C_Relation;
    for (size_t i = 0; i < states.size(); i++) {
        fixed_point = (stateVector[i]) ? coFactorTrue(fixed_point, states[i]) : coFactorFalse(fixed_point, states[i]);
    }

    return (fixed_point == True());
}

void Reachability::getReachableStates() {
    BDD_ID C_R, C_R_iter, C_s;

    /**** Step 3: computing the BDD for the transition relation ****/
    BDD_ID tr_relation = xnor2(next_states[0], transition_functions[0]);
    for (size_t i = 1; i < states.size(); i++) {
        tr_relation = and2(tr_relation, xnor2(next_states[i], transition_functions[i]));
    }

    /**** Step 4: computing the characteristic function of the initial state ****/
    C_s = xnor2(states[0], (initial_state[0]) ? True() : False());
    for (size_t i = 1; i < states.size(); i++) {
        C_s = and2(C_s, xnor2(states[i], (initial_state[i]) ? True() : False()));
    }

    /**** Step 5: defining the characteristic relation iterator ****/
    C_R_iter = C_s;

    // Starting the iteration
    do {
        /**** Step 6 ****/
        C_R = C_R_iter;

        /**** Step 7: computing the image of state_prime ****/
        auto next_states_image = and2(C_R, tr_relation);
        for (size_t i = states.size() - 1; i != -1; i--) {
            next_states_image = or2(coFactorTrue(next_states_image, states[i]),
                                    coFactorFalse(next_states_image, states[i]));
        }


        /**** Step 8: computing the image ****/
        auto image = and2(xnor2(states[0], next_states[0]), next_states_image);
        for (size_t i = 1; i < states.size(); i++) {
            image = and2(image, xnor2(states[i], next_states[i]));
        }

        for (size_t i = states.size() - 1; i != -1; i--) {
            image = or2(coFactorTrue(image, next_states[i]), coFactorFalse(image, next_states[i]));
        }

        /**** Step 9: Computing the BDD_ID for the C_R_iter = C_R + image ****/
        C_R_iter = or2(C_R, image);

    } while (C_R_iter != C_R);

    // Now looking it in the whole state vector Computing the existential quantification
    std::set<BDD_ID> vars;
    findVars(C_R, vars);
    for (BDD_ID var: vars) {
        if (std::find(states.begin(), states.end(), var) == states.end())
            C_R = or2(coFactorTrue(C_R, var), coFactorFalse(C_R, var));
    }

    C_Relation = C_R;
}

const std::vector<BDD_ID> &Reachability::getStates() const {
    return states;
}

Reachability::Reachability(unsigned int size) : ReachabilityInterface(size), needs_update(true) {

    if (size == 0) {
        throw std::runtime_error("State vector cannot be empty");
    }

    for (int i = 0; i < size; i++) {
        states.push_back(createVar("state[" + std::to_string(i) + "]"));
        next_states.push_back(createVar("prime_state[" + std::to_string(i) + "]"));
    }

    // By this line of code we guarantee that the default transition function for each state bit is the identity function.
    transition_functions = states;

    /* Set all the bits of the initial state vector to false. */
    initial_state.resize(size, false);
}