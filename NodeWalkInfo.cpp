#include <iostream>
#include <limits>
#include "NodeWalkInfo.h"


NodeWalkInfo::NodeWalkInfo() :
    NodeWalkInfo(0, 0, 0, 1)
{}

/*
 * A lower standard deviation means the node is more central. So a node that is never walked on needs to have a big
 * value as its default standard deviation value
 */
NodeWalkInfo::NodeWalkInfo(unsigned int threadCount, unsigned int requiredReturns, unsigned int epsilon, unsigned int nodeId) :
    standardDeviation(std::numeric_limits<double>::max()), threadCount(threadCount),
    requiredReturns(requiredReturns), epsilon(epsilon), lastThreadTick(threadCount, -1), lastStandardDeviation(0),
    nodeId(nodeId)
{
    omp_init_lock(&writelock);
}

NodeWalkInfo::NodeWalkInfo(const NodeWalkInfo &other) {

    omp_init_lock(&writelock);
    *this = other;
}

NodeWalkInfo::~NodeWalkInfo() {
    omp_destroy_lock(&writelock);
}

NodeWalkInfo &NodeWalkInfo::operator=(const NodeWalkInfo &other) {

    nodeId = other.nodeId;
    standardDeviation = other.standardDeviation;
    lastStandardDeviation = other.lastStandardDeviation;

    requiredReturns = other.requiredReturns;
	epsilon = other.epsilon;
    returnTimes = other.returnTimes;

    threadCount = other.threadCount;
    lastThreadTick = other.lastThreadTick;

    return *this;
}

void NodeWalkInfo::submitTick(unsigned int threadId, unsigned int tick) {

    if(threadId >= 0 && threadId < threadCount) {

        int lastTick = lastThreadTick[threadId];

        omp_set_lock(&writelock);

        if(lastTick > 0) {

            returnTimes.push_back(tick - lastTick);

            if(returnTimes.size() >= 3) {
                calculateStandardDeviation();
            }
        }

        lastThreadTick[threadId] = tick;

        omp_unset_lock(&writelock);
    }
    else {
        std::cerr << "Error - wrong thread id " << threadId << std::endl;
    }
}

void NodeWalkInfo::calculateStandardDeviation() {

    long long int sum = 0;
    for (int i = 0; i < returnTimes.size(); i++)	{
        sum += returnTimes[i];
    }
    double mean = (double) sum / returnTimes.size();

    double variance = 0;
    for (int i = 0; i < returnTimes.size(); i++)
    {
        variance += pow(returnTimes[i] - mean, 2);
    }

    lastStandardDeviation = standardDeviation;
    standardDeviation = sqrt(variance / returnTimes.size());
}

bool NodeWalkInfo::hasConverged() const {
    double epsilonResult = (lastStandardDeviation * (double) epsilon) / 100.0;

    if(!hasEnoughReturnTimes()) {
        return false;
    }

    return standardDeviation >= lastStandardDeviation - epsilonResult && standardDeviation <= lastStandardDeviation + epsilonResult;
	//return returnTimes.size() >= requiredReturns;
}

bool NodeWalkInfo::hasEnoughReturnTimes() const {
    // returnTimes.size() >= requiredReturns + 1
    return returnTimes.size() > requiredReturns;
}
