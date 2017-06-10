#ifndef PROJECT_KERMARRECKPLUGIN_H
#define PROJECT_KERMARRECKPLUGIN_H

#include <tulip/TulipPluginHeaders.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <future>

class KermarreckAlgorithm:public tlp::DoubleAlgorithm {
public:

    // This line is used to pass information about the current plug-in.
    PLUGININFORMATION("Kermarreck centrality",
                      "Thomas Correia, Théo Gilbert, Simon Mercier",
                      "10/06/17",
                      "Calculates the centrality of the nodes in the graph in a distributed manner",
                      "0.1",
                      "Centrality")

    KermarreckAlgorithm(const tlp::PluginContext* context);

    bool run();
protected:	
	double randomWalk(tlp::node n, double nbReturnTime);
};


#endif //PROJECT_KERMARRECKPLUGIN_H
