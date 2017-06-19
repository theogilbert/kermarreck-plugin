#include "KermarreckAlgorithm.h"

using namespace std;
using namespace tlp;
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
        DoubleAlgorithm(context), generator(rand_dev()) {
	addInParameter<int>("k","Number of return times for every node", "30", false);
	addInParameter<int>("limit","Number ticks per thread before stopping if no convergence", "10000000", false);
	addInParameter<int>("threads","Number of random walks", "8", false);
	addInParameter<int>("epsilon","Percentage of variation for convergence", "1", false);
}

// The run method is the main method :
//     - It will be called out if the pre-condition method (bool check (..)) returned true.
//     - It is the starting point of your algorithm.
// The returned value must be true if your algorithm succeeded.
bool KermarreckAlgorithm::run() {

    omp_init_lock(&convergedNodesLock);

	result->setAllNodeValue(0.0);

	int numberOfThreads = 0;
	int nbReturnTimes = 0;
	int tickLimit = 0;
	int epsilon = 0;
	if (dataSet!=NULL) {
		dataSet->get("k", nbReturnTimes);
		dataSet->get("limit", tickLimit);
		dataSet->get("threads", numberOfThreads);
		dataSet->get("epsilon", epsilon);
	}

	Iterator<node> * itNode = graph->getNodes();
    while(itNode->hasNext()){
        node n = itNode->next();
			
		if (graph->deg(n) > 0)
			nodeWalkInfos[n.id] = NodeWalkInfo((unsigned int) numberOfThreads, (unsigned int) nbReturnTimes, (unsigned int) epsilon, n.id);
    }
	delete itNode;

    #pragma omp parallel for
	for (int i = 0; i < numberOfThreads; i++){
        randomWalk(tickLimit, i);
	}

    // Once the walk is over, we set the property for all nodes
    itNode = graph->getNodes();
    while(itNode->hasNext()){
        node n = itNode->next();

        NodeWalkInfo& walkInfo = nodeWalkInfos[n.id];

        result->setNodeValue(n, walkInfo.getStandardDeviation());		
    }
    delete itNode;

    omp_destroy_lock(&convergedNodesLock);

    return true;
}

int KermarreckAlgorithm::randomWalk(int tickLimit, int numThread)
{
    uniform_real_distribution<double> real_distribution(0.0, 1.0);

    unsigned int nodeCount = (unsigned int) (nodeWalkInfos.size() * 0.9);

	unsigned int tick = 0;
	
	// find valids nodes
	node currentNode;
	bool nodeTrouvee = false;
	while (!nodeTrouvee){
		currentNode = graph->getRandomNode();
		if (graph->deg(currentNode) > 0){
			nodeTrouvee = true;
		}
	}
	
    while(convergedNodes.size() < nodeCount && tick < tickLimit)
    {
        Iterator<node> *itNode = graph->getInOutNodes(currentNode);
        node nextNode;

		int randNode = uniform_int_distribution<int>(0, graph->deg(currentNode) - 1)(generator);
		int i = 0;
		while(i < randNode)
		{
			itNode->next();
			i++;
		}
		nextNode = itNode->next();			

        delete itNode;

        if(real_distribution(generator) <= ((double)(graph->deg(currentNode)) / graph->deg(nextNode)))
        {
            currentNode = nextNode;

			NodeWalkInfo& walkInfo = nodeWalkInfos[currentNode.id];
			walkInfo.submitTick((unsigned int) numThread, tick);

            if(nodeWalkInfos[currentNode.id].hasConverged() && convergedNodes.find(currentNode.id) == convergedNodes.end()) {

                omp_set_lock(&convergedNodesLock);
                convergedNodes.insert(currentNode.id);
                omp_unset_lock(&convergedNodesLock);
            }

            tick++;
        }
    }

	#pragma omp critical (thread_result)
	{
		cout << "Thread " << numThread << " - " << convergedNodes.size() << "/" << nodeCount << " node have converged after ";
		cout << tick << " steps." << endl;
	}
	
	return 0;
}


