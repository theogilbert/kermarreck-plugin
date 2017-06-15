#ifndef PROJECT_NODEWALKINFO_H
#define PROJECT_NODEWALKINFO_H


#include <vector>
#include <cmath>
#include <map>
#include <mutex>
#include <omp.h>


/*
 * Class used to keep track of the times a node has been walked onto, and to calculate the standard deviation associated
 * with those return times
 */
class NodeWalkInfo {

public:

    NodeWalkInfo();
    NodeWalkInfo(unsigned int threadCount);
    NodeWalkInfo(unsigned int threadCount, unsigned int requiredReturns, unsigned int nodeId);
    NodeWalkInfo(const NodeWalkInfo& other);

    ~NodeWalkInfo();

    NodeWalkInfo& operator=(const NodeWalkInfo& other);

    void submitTick(unsigned int threadId, unsigned int tick);

    bool hasConverged() const;

    bool hasEnoughReturnTimes() const;

    double getStandardDeviation() const { return standardDeviation; }

private:

    void calculateStandardDeviation();

    std::vector<unsigned int> returnTimes;

    // Threads will have an id between 0 and threadCount, might as well use a std::vector and use the index as the id
    // of a thread to save some computation time
    std::vector<int> lastThreadTick;

    double lastStandardDeviation;

    double standardDeviation;

    unsigned int requiredReturns;

    unsigned int threadCount;

    omp_lock_t writelock;

};


#endif //PROJECT_NODEWALKINFO_H
