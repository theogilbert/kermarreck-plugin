#include <iostream>
#include <limits>
#include "NodeWalkInfo.h"


NodeWalkInfo::NodeWalkInfo() :
    NodeWalkInfo(0, 0)
{}

// A lower standard deviation means the node is more central. So a node that is never walked on needs to have a big
// value as its default standard deviation value
NodeWalkInfo::NodeWalkInfo(unsigned int threadCount, unsigned int requiredReturns) :
    standardDeviation(std::numeric_limits<double>::max()), threadCount(threadCount),
    requiredReturns(requiredReturns), lastThreadTick(threadCount, -1)
{}

NodeWalkInfo::NodeWalkInfo(const NodeWalkInfo &other) {
    *this = other;
}

NodeWalkInfo &NodeWalkInfo::operator=(const NodeWalkInfo &other) {

    standardDeviation = other.standardDeviation;

    requiredReturns = other.requiredReturns;
    returnTimes = other.returnTimes;

    threadCount = other.threadCount;
    lastThreadTick = other.lastThreadTick;

    return *this;
}

void NodeWalkInfo::submitTick(unsigned int threadId, unsigned int tick) {

    if(threadId >= 0 && threadId < threadCount) {

        int lastTick = lastThreadTick[threadId];

        mutex.lock();

        if(lastTick > 0) {

            returnTimes.push_back(tick - lastTick);

            if(returnTimes.size() >= 3) {
                calculateStandardDeviation();
            }
        }

        lastThreadTick[threadId] = tick;
        
        mutex.unlock();
    }
    else {
        std::cerr << "Error - wrong thread id " << threadId << std::endl;
    }
}

void NodeWalkInfo::calculateStandardDeviation() {

    // On large graphs, we need a large number to store the sum of the squares of offsets to avoid an overflow
    unsigned long long int sumTicks = 0;
    unsigned long long int sumTicksCarre = 0;

    for(int i = 0; i < returnTimes.size(); i++)
    {
        unsigned long long int returnTime = (unsigned long long int) (returnTimes[i]);
        sumTicks += returnTime;
        sumTicksCarre += returnTime * returnTime;
    }

    sumTicks /= (returnTimes.size() - 1);
    sumTicksCarre /= (returnTimes.size() - 1);

    standardDeviation = sqrt((double) (sumTicksCarre - (sumTicks*sumTicks)));
}

bool NodeWalkInfo::hasConverged() const {
    // returnTimes.size() >= requiredReturns + 1
    return returnTimes.size() > requiredReturns;
}
