#ifndef PROJECT_KERMARRECKPLUGIN_H
#define PROJECT_KERMARRECKPLUGIN_H

#include <tulip/TulipPluginHeaders.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <future>

#include "NodeWalkInfo.h"

class KermarreckAlgorithm:public tlp::DoubleAlgorithm {
public:

    // This line is used to pass information about the current plug-in.
    PLUGININFORMATION("Kermarreck centrality",
                      "Thomas Correia, Théo Gilbert, Simon Mercier",
                      "10/06/17",
                      "Calculates the centrality of the nodes in the graph in a distributed manner. Uses OpenMP to do so.",
                      "0.1",
                      "Centrality")

    KermarreckAlgorithm(const tlp::PluginContext* context);

    bool run();
protected:
    /*
     * Precondition : the map nodeWalkInfos has been filled for each node in the graph
     */
    int randomWalk(double nbReturnTime, int tickLimit, int numThread);

    std::unordered_map<unsigned int, NodeWalkInfo> nodeWalkInfos;
    std::unordered_map<unsigned int, std::thread> nodeThreads;

    std::random_device rand_dev;
    std::mt19937 generator;
};


#endif //PROJECT_KERMARRECKPLUGIN_H