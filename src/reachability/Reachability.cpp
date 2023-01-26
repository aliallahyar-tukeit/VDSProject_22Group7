#include "Reachability.h"
using namespace ClassProject;


void Reachability::setInitState(const std::vector<bool> &stateVector) {

}

void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {

}

bool Reachability::isReachable(const std::vector<bool> &stateVector) {
    return false;
}

const std::vector<BDD_ID> &Reachability::getStates() const {
    return states;
}

Reachability::Reachability(unsigned int i) : ReachabilityInterface(i) {

}
