#ifndef PROJECT_KERMARRECKPLUGIN_H
#define PROJECT_KERMARRECKPLUGIN_H

#include <tulip/TulipPluginHeaders.h>

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
};


#endif //PROJECT_KERMARRECKPLUGIN_H
