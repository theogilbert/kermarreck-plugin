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
	addInParameter<int>("nbReturnTimes","Number of return times for every node", "30", false);
	addInParameter<int>("tickLimit","Number ticks per thread before stopping if no convergence", "10000", false);
	addInParameter<int>("numberOfThreads","Number of random walks", "8", false);
}

// The run method is the main method :
//     - It will be called out if the pre-condition method (bool check (..)) returned true.
//     - It is the starting point of your algorithm.
// The returned value must be true if your algorithm succeeded.
bool KermarreckAlgorithm::run() {

	result->setAllNodeValue(0.0);

	int numberOfThreads = 0;
	int nbReturnTimes = 0;
	int tickLimit = 0;
	if (dataSet!=NULL) {
		dataSet->get("nbReturnTimes", nbReturnTimes);
		dataSet->get("tickLimit", tickLimit);
		dataSet->get("numberOfThreads", numberOfThreads);
	}

    cout << "Nb return times : " << nbReturnTimes << endl;
	Iterator<node> * itNode = graph->getNodes();
    while(itNode->hasNext()){
        node n = itNode->next();
			
		if (graph->deg(n) > 0)
			nodeWalkInfos[n.id] = NodeWalkInfo(numberOfThreads, (unsigned int) nbReturnTimes, n.id);
    }

	delete itNode;
	
	vector<future<int>> futures;	
	for (int i = 0; i < numberOfThreads; i++){		
		futures.push_back(async(& KermarreckAlgorithm::randomWalk, this, nbReturnTimes, tickLimit, i));				
	}
	
    for(auto &e : futures) {
		e.get();
	}

	
    // Once the walk is over, we set the property for all nodes
    itNode = graph->getNodes();
    while(itNode->hasNext()){
        node n = itNode->next();

        NodeWalkInfo& walkInfo = nodeWalkInfos[n.id];

        result->setNodeValue(n, walkInfo.getStandardDeviation());		
    }
    delete itNode;

    return true;
}

int KermarreckAlgorithm::randomWalk(double requiredReturnTime, int tickLimit, int numThread)
{

    uniform_real_distribution<double> real_distribution(0.0, 1.0);

    unsigned int nodeCount = (unsigned int) (nodeWalkInfos.size() * 0.9);
    set<unsigned int> convergedNodes;

	unsigned int tick = 0;	
	
	// find valids nodes
	node currentNode;
	bool nodeTrouvee = false;
	while (nodeTrouvee == false){
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
			walkInfo.submitTick(numThread, tick);

            if(nodeWalkInfos[currentNode.id].hasConverged()) {
                convergedNodes.insert(currentNode.id);
            }

            tick++;
        }
    }

    Iterator<node> * itNode = graph->getNodes();
    while(itNode->hasNext()){
        node n = itNode->next();

        if(nodeThreads[n.id].joinable()) {
            nodeThreads[n.id].join();
        }
    }
    delete itNode;

    cout << convergedNodes.size() << "/" << nodeCount << " node have converged after ";
    cout << tick << " steps." << endl;
	
	return 0;
}


