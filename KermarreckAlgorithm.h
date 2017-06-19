#ifndef PROJECT_KERMARRECKPLUGIN_H
#define PROJECT_KERMARRECKPLUGIN_H

#include <tulip/TulipPluginHeaders.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <future>
#include <omp.h>

#include "NodeWalkInfo.h"

/**
 * Main class of the Kermarreck plugin
 * This plugin's job is to run multiple random walk (each on a separate thread) and to use NodeWalkInfo instances to
 * run the walk until a certain amount of nodes have converged.
 */
class KermarreckAlgorithm:public tlp::DoubleAlgorithm {
public:

    // This line is used to pass information about the current plug-in.
    PLUGININFORMATION("Kermarreck centrality",
                      "Thomas Correia, Théo Gilbert, Simon Mercier",
                      "10/06/17",
                      "Calculates the centrality of the nodes in the graph in a distributed manner. Uses OpenMP to do so.",
                      "1.0",
                      "Centrality")

    KermarreckAlgorithm(const tlp::PluginContext* context);

    /*
     * Checks that the right conditions are met to run the algorithm
     */
    bool check(std::string& errorMsg);

    /*
     * Runs the algorithm
     */
    bool run();
protected:

    /*
     * Runs a random walk over the graph
     *
     * tickLimit : The maximum amount of step this random walk should execute before stopping (regardless of the amount
     * of converged nodes)
     * numThread : The number of the thread running this walk
     *
     * Precondition : the map nodeWalkInfos has been filled for each node in the graph
     */
    int randomWalk(int tickLimit, int numThread);

    /*
     * Map containing the key-value pairs (node_id, NodeWalkInfo), associating for each node its corresponding
     * instance of NodeWalkInfo
     */
    std::unordered_map<unsigned int, NodeWalkInfo> nodeWalkInfos;

    /*
     * Objects used to generate uniform random numbers
     */
    std::random_device rand_dev;
    std::mt19937 generator;

    /*
     * Set used to keep track of the nodes that have converged
     * If a node has converged, it is put in this set. This way, it is easy to check how many nodes have converged
     */
	std::set<unsigned int> convergedNodes;

    /*
     * Lock used to not modify the convergedNodes set at the same time over different threads
     */
    omp_lock_t convergedNodesLock;
};


#endif //PROJECT_KERMARRECKPLUGIN_H