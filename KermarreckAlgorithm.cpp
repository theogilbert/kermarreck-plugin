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
        DoubleAlgorithm(context) {
	addInParameter<double>("nbReturnTimes","Number of return times for every node", "30", false);
}

// The run method is the main method :
//     - It will be called out if the pre-condition method (bool check (..)) returned true.
//     - It is the starting point of your algorithm.
// The returned value must be true if your algorithm succeeded.
bool KermarreckAlgorithm::run() {
    result->setAllNodeValue(0.0);
	srand(time(NULL));
	node n;  
	double nbReturnTimes;
	if (dataSet!=NULL) {
		dataSet->get("nbReturnTimes", nbReturnTimes);
	}
	
	Iterator<node> * itNode = graph->getNodes();
	for (int i = 0; i < graph->numberOfNodes(); i++){
		std::vector<std::future<double>> futures;		
		//Launch a group of threads
		vector<node> nodes;
		int j = 0;		
		while(itNode->hasNext() && j < graph->numberOfNodes()){
			n = itNode->next();
			nodes.push_back(n);
			futures.push_back (async(& KermarreckAlgorithm::randomWalk, this, n, nbReturnTimes));
			j++;
		}	
		int c = 0;
		for(auto &e : futures) {
			result->setNodeValue(nodes.at(c), e.get());
			c++;
		}
	}
	
	delete itNode;
    return true;
}

double KermarreckAlgorithm::randomWalk(node n, double nbReturnTimes)
{
	int tick = 0;
	node currentNode = n;
	//node previousNode = n;
	vector<int> ticks;
	if (graph->deg(currentNode) > 0){
		while(ticks.size() < nbReturnTimes)
		{
			Iterator<node> *itNode = graph->getInOutNodes(currentNode);
			node nextNode;
			int randNode = rand() % graph->deg(currentNode);
			int i = 0;
			while(itNode->hasNext() && i < randNode)
			{
				itNode->next();
				i++;
			}
			nextNode = itNode->next();		
			delete itNode;
			if(rand()/(double)RAND_MAX < (double)((double)(graph->deg(currentNode)) / graph->deg(nextNode)))
			{
				//previousNode = currentNode;
				currentNode = nextNode;
			}
			
			if(currentNode.id == n.id)
			{
				ticks.push_back(tick);
			}
			tick++;
		}
		double sumTicks = 0;
		for(int i = 0; i < ticks.size(); i++)
		{
			sumTicks += ticks.at(i);
		}
		sumTicks /= (ticks.size());
		double sumTicksCarre = 0;
		for(int i = 0; i < ticks.size(); i++)
		{
			sumTicksCarre += ticks.at(i)*ticks.at(i);
		}
		sumTicksCarre /= (ticks.size());
		
		
		return sqrt(sumTicksCarre - (sumTicks*sumTicks));
	}
	return 0;
}
