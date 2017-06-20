#ifndef PROJECT_NODEWALKINFO_H
#define PROJECT_NODEWALKINFO_H


#include <vector>
#include <cmath>
#include <map>
#include <mutex>
#include <omp.h>


/*
 * Class used to keep track of the times a node has been walked onto, and to calculate the standard deviation associated
 * with those return times. Each instance of this class is associated with a node, and can be accessed from multiple
 * threads.
 *
 * This class also let us know when a node's standard deviation has converged over time. For a node to converge, it has
 * to have been walked on at least {requiredReturns + 1} times. Only then will a node can be considered to have converged
 * if the difference between the standard deviation and the last calculated standard deviation is smaller than
 * {epsilon} * {standardDeviation} (with epsilon in the range [0, 1])
 */
class NodeWalkInfo {

public:

    // Default constructor
    NodeWalkInfo();
    /*
     * Main constructor
     *
     * threadCount : The amount of threads used in this algorithm (that can access this object)
     * requiredReturns : The amount of times a node has to be walked on before being able to know if it has converged
     * epsilon : The amount (in percent) of the standard deviation that can be considered as a margin of error to check
     *              if a node has converged
     * nodeId : The id of the node associated with this object
     */
    NodeWalkInfo(unsigned int threadCount, unsigned int requiredReturns, unsigned int epsilon, unsigned int nodeId);
    // Copy constructor
    NodeWalkInfo(const NodeWalkInfo& other);

    ~NodeWalkInfo();

    // Copy operator
    NodeWalkInfo& operator=(const NodeWalkInfo& other);

    /*
     * Function to be called when the associated node has been walked on
     * If this function has been called thrice already, then calculates the new standard deviation
     *
     * threadId : The id of the thread submitting the tick (the step count)
     * tick : The tick (step) at which the associated node has been walked on
     */
    void submitTick(unsigned int threadId, unsigned int tick);

    /*
     * Returns false if hasEnoughReturnTimes() returns false
     * Otherwise, returns true if the difference between {standardDeviation} and the last calculated standard deviation
     * is small enough (< epsilon * standardDeviation)
     */
    bool hasConverged() const;

    /*
     * Returns true if submitTick has been called {requiredReturns} + 1 times, that is, if the random walk has returned
     * to the node {requiredReturns} times
     */
    bool hasEnoughReturnTimes() const;

    /*
     * Returns the last calculated standard deviation
     */
    double getStandardDeviation() const { return standardDeviation; }

private:

    /*
     * Calculates the new standard deviation from the list of return times
     */
    void calculateStandardDeviation();

    /*
     * Contains the amount of step between each return times to this node
     *
     * For instance, if in a thread A, the associated node has been walked on at steps 10, 24 and 30, and in thread B
     * it has been walked on at steps 12, 14 and 45, then this list will contain the following values :
     * [14, 6, 2, 31]
     */
    std::vector<unsigned int> returnTimes;

    /*
     * List containing {threadCount} elements. Each element correspond to the last tick the associated node has been
     * walked on in a thread i (i being the index of the element in this list)
     */
    std::vector<int> lastThreadTick;

    // The previous value of standard deviation, used to know if a node has converged or not
    double lastStandardDeviation;

    // The current standard deviation of the associated node
    double standardDeviation;

    // The amount of returns required for a node to converge
    unsigned int requiredReturns;

    // The amount (in percent) of the standard deviation that can be considered as a margin of error to check if a node
    // has converged
	unsigned int epsilon;

    // The amount of threads used for the random walks
    unsigned int threadCount;

    // Lock used to not modify the attributes concurrently
    omp_lock_t writelock;

    // The id of the associated node
    unsigned int nodeId;

};


#endif //PROJECT_NODEWALKINFO_H
