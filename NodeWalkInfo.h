//
// Created by z3tyop on 10/06/17.
//

#ifndef PROJECT_NODEWALKINFO_H
#define PROJECT_NODEWALKINFO_H


#include <vector>
#include <cmath>
#include <map>

class NodeWalkInfo {

public:

    NodeWalkInfo();
    NodeWalkInfo(unsigned int threadCount, unsigned int requiredReturns);
    NodeWalkInfo(const NodeWalkInfo& other);

    NodeWalkInfo& operator=(const NodeWalkInfo& other);

    void submitTick(unsigned int threadId, unsigned int tick);

    bool hasConverged() const;

    double getStandardDeviation() const { return standardDeviation; }

private:

    void calculateStandardDeviation();

    std::vector<unsigned int> returnTimes;

    // Threads will have an id between 0 and threadCount, might as well use a std::vector and use the index as the id
    // of a thread to save some computation time
    std::vector<int> lastThreadTick;

    double standardDeviation;

    unsigned int requiredReturns;

    unsigned int threadCount;

};


#endif //PROJECT_NODEWALKINFO_H
