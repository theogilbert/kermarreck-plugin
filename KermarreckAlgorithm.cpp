#include "KermarreckAlgorithm.h"

/** Algorithm documentation */

// This line will be used to register your algorithm in tulip using the information given above.
PLUGIN(KermarreckAlgorithm)

// The constructor below has to be defined,
// it is the right place to declare the parameters
// needed by the algorithm,
//   addInParameter<ParameterType>("Name","Help string","Default value");
// and declare the algorithm dependencies too.
//   addDependency("name", "version");
KermarreckAlgorithm::KermarreckAlgorithm(const tlp::PluginContext* context) :
        DoubleAlgorithm(context) {
}

// The run method is the main method :
//     - It will be called out if the pre-condition method (bool check (..)) returned true.
//     - It is the starting point of your algorithm.
// The returned value must be true if your algorithm succeeded.
bool KermarreckAlgorithm::run() {
    return true;
}