
/*
Author: Azam
3/5/2022
This is an extension of code ....3ver, and 
version for 2-interval PCGs
// Generating PCG w.r.t edge density//
//The leaves of the trees are labelled from 0~n-1
//Warning: if mod 0 is applied then program stops, so be carefull
*/

#include <time.h>
#include <random>
#include <cstdio>
#include <set>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <sstream>
#include <exception>
#include <memory>
#include <fstream>
#include <math.h>
//#include <unistd.h>
#include <sys/types.h>
//#include <sys/stat.h>
//#include <string>
#include<stdio.h>
#include<dos.h>
#include <sys/stat.h>
#include <direct.h>
using namespace std;

#include "graph3.h"
#include "debug.h"
//#include "cross_timer.h"
#include "canon_graph.h"
//#include "pcgConfiguration.h"

//ILOSTLBEGIN
#define ALL(x) x.begin(), x.end()
typedef unordered_map<edge, bool> edgeBoolMap;
typedef edgeBoolMap::value_type eBoolMapValT;
typedef typename unordered_map < edge, unordered_set <edge> >::value_type etMapValT;
// Returns value of Binomial Coefficient C(n, k)
inline int binomialCoeff(int n, int k)
{
	// Base Cases 
	if (k == 0 || k == n)
		return 1;

	// Recur 
	return  binomialCoeff(n - 1, k - 1) + binomialCoeff(n - 1, k);
}

inline int nchoose2(int &n) {
	return (n * (n - 1)) >> 1;//divide by 2
}

// Now, preparing maps E_T (u, v), for all T and u, v \in V(T)
void Find_Pathedges(Graph<unordered_set> &T,
	vector <unordered_map < edge, unordered_set<edge> >> &Vec_Tree_paths,
	int &tree_index)

{
	unordered_set< vertex > leavesT;
	T.get_leaves(leavesT);
	for (const auto &u : leavesT) {
		for (const auto &v : leavesT) {
			// only add pairs for u < v
			if (u >= v) continue;
			//else
			unordered_set <edge> thePath, thePathU; //U -> undirected version
			T.getPathEdges(u, v, thePath);
			//DBG1D(thePath);
			for (const auto &ee : thePath) {
				edge e(ee.undirect());
				thePathU.insert(e);

			}
			//DBG1D(thePathU);
			edge e1(u, v);
			//etEdgeMap.insert(etMapValT(e1, thePathU));
			Vec_Tree_paths[tree_index].insert(etMapValT(e1, thePathU));
			//edge e2(v, u);
			//etEdgeMap.insert(etMapValT(e2, thePathU));
		}
	}

	/*for (const auto &u : leavesT) {
	for (const auto &v : leavesT) {
	if (u < v) {
	edge e(u, v);
	DBG1(e);
	DBG1D(Vec_Tree_paths[tree_index][e]);
	}
	}
	}*/

}

//Preparing a vector containing edges in common indexing form i.e., first n are external and remaining are internal edges
void Common_indexed_edges(Graph<unordered_set> &T,
	vector <vector<edge>> &VecTreeEdges,
	int &tree_index)
{
	unordered_set< vertex > leavesT;
	T.get_leaves(leavesT);
	for (const auto &e : T.edges) {
		vertex u = e.first;
		vertex v = e.second;
		if ((u < v) && ((leavesT.find(u) != leavesT.end()) ||
			(leavesT.find(v) != leavesT.end()))) {//external canon edge (we can use degree)
			VecTreeEdges[tree_index].push_back(e);
		}
	}

	for (const auto &e : T.edges) {
		vertex u = e.first;
		vertex v = e.second;
		if ((u < v) && (leavesT.find(u) == leavesT.end()) &&
			(leavesT.find(v) == leavesT.end())) {//internal canon edge
			VecTreeEdges[tree_index].push_back(e);
		}
	}

	//printing//
	/*for (const auto &ee : VecTreeEdges[tree_index]) {
	DBG1(ee);
	}*/
}

//Generating Weight vector randomly for all edges//
//first n values are weights of external edges and remaining are of internal//
void VECWEIGHT(vector<int> &VecWeight,
	int &num_leaves,
	mt19937 &mt)
{

	int rand_no;
	for (int i = 0; i <= num_leaves - 1; i++) {
		rand_no = mt() % 50 + 1; //[1, 50]
		VecWeight.push_back(rand_no);
	}
	for (int j = num_leaves; j <= ((2 * num_leaves) - 4); j++) {
		rand_no = mt() % 20 + 1; //[1, 20]
		VecWeight.push_back(rand_no);
	}

}


//getting weights for edges//
void MAP_EDGE_WEIGHT(vector<int> &Vecweight,
	vector<edge> &VecComEdge,
	unordered_map <edge, int> &mapEdgeWeight)
{
	for (int i = 0; i <= Vecweight.size() - 1; i++)
	{
		mapEdgeWeight[VecComEdge[i]] = Vecweight[i];

	}

}


// calculating weight of a gvien path//
inline void CalPathWeight(unordered_map <edge, int> &mapEdgeWeight,
	unordered_set <edge> &pathuv,
	int &pathweightuv) {
	//MSG("%%%%%%%%%%%%%%%");
	//DBG1(pathweightuv);
	for (const auto &e : pathuv) {
		vertex u = e.first;
		vertex v = e.second;
		if (u >= v) continue;//if edge is not canonical
							 //else//
							 //DBG1(e);
		else
		{
			pathweightuv += mapEdgeWeight[e];
		}
		//DBG1(pathweightuv);
	}

}

//Finding Set Dist (in strictly increasing order) and all classes[{a,b}] in a map//
void SetMapDist(int &num_leaves,
	unordered_map < edge, unordered_set<edge> > &mappaths,// paths between any two leaves
	unordered_map <edge, int> &mapEdgeWeight,//having weights of edges
	set <int> &SetDist,
	unordered_map<int, vector<edge>> &MapEdgesDist)
{
	for (int u = 0; u <= num_leaves - 1; u++) {
		for (int v = 0; v <= num_leaves - 1; v++) {
			if (u >= v) continue;
			else {//canon u, v pair
				int dist_uvwt = 0;
				edge uv(u, v);
				//DBG1(uv);
				//DBG1D(mappaths[uv]);
				CalPathWeight(mapEdgeWeight, mappaths[uv], dist_uvwt);//finding dist_uvwt
																	  //DBG1(dist_uvwt);
				SetDist.insert(dist_uvwt);//adding in the set
				MapEdgesDist[dist_uvwt].push_back(uv);// adding in the class.
			}
		}
	}
	//printing and testing//
	//DBG1D(SetDist);
	//DBG1(SetDist.size());
	//int sum = 0;
	//for (const auto &dist : SetDist) {
	//	//DBG1D(MapEdgesDist[dist]);
	//	//DBG1(MapEdgesDist[dist].size());
	//	sum = sum + MapEdgesDist[dist].size();
	//}
	////DBG1(sum);
	//if (sum != binomialCoeff(num_leaves, 2))
	//{
	//	cout << "MapEdgesDist is not calculated correctly" << endl;
	//	throw(-1);
	//}
	//DBG1D(MapEdgesDist);
}

//Writing in dreadnaut input format//
void Input_Nauty(Graph<unordered_set> &G,
	ostream &output_file,
	string &canon_output) {
	// for dreadnaut coloring all vertices = 0 
	for (const auto &v : G.vertices) {
		G.colorMap[v] = 0;
	}
	//DBG1D(G.colorMap);
	//// due to technical reasons convert T_sigma to another graph T_sigmaP (the original graph will not change)// 
	//CanonGraph<unordered_set> GP(G);
	string dreadnaut_input;
	dreadnaut_input = G.for_dreadnaut(false, canon_output);
	//dreadnaut_input = GP.for_dreadnaut(false, canon_output);
	//DBG1(dreadnaut_input);
	output_file << dreadnaut_input << endl;

}

// checking if a file exits//
bool File_exists(const string& name) {// 1 if file exists
	ifstream f(name.c_str());
	return f.good();
}


int main(int argc, char **argv) {
	string treeFile, graphFile, canon_repFile;// tree file;
	treeFile = string(argv[1]);// tree file
	graphFile = string(argv[2]);// all graphs with 8 vertice after removal of supergraphs of NPCGs and non-biconnected
	canon_repFile = string(argv[3]);//canon_rep of graphFile
	int Graph_num_ver = atoi(argv[4]);
	int num_trees = atoi(argv[5]);//number of total trees
	size_t time_limit = atoi(argv[6]);

	cout << "Warning: Change values of weights in the file osfoundPCGsFile if necessary!!!!!!!!!!!!!!!!" << endl;

	//DBG1(time_limit);
	//return 0;
	MSG("Hello");
	//opening input files//
	ifstream ifsgraph, ifscanon_rep;
	try {
		ifsgraph.open(graphFile);
	}
	catch (const exception& e) {
		cerr << "Couldn't open Graph file '" << graphFile
			<< "' for reading!" << endl;
		throw(e);
	}
	if (!ifsgraph.is_open()) {
		cerr << "Error: Graph file is not initialized!" << endl;
		throw(-1);
	}
	try {
		ifscanon_rep.open(canon_repFile);
	}
	catch (const exception& e) {
		cerr << "Couldn't open canon file of input graphs '" << canon_repFile
			<< "' for reading!" << endl;
		throw(e);
	}
	if (!ifscanon_rep.is_open()) {
		cerr << "Error: Canon_rep file is not initialized!" << endl;
		throw(-1);
	}
	istream *isGraph = &ifsgraph;
	istream *isCanon_rep = &ifscanon_rep;//canon_rep of original graphs
	MSG("Partitioning graphs");
	cout << "Partitioning Input Canon Graphs" << endl;
	cout << "Here" << endl;
	//partitioning the graphs and their canon_rep w.r.t no of edge//
	//int total_num_edges = binomialCoeff(Graph_num_ver, 2);
	int total_num_edges = nchoose2(Graph_num_ver);
	DBG1(total_num_edges);
	//MSG("");
	//DBG1(total_num_edges);
	//vector <vector <Graph<unordered_set>>> Graph_partition(total_num_edges + 1);//Graph_partition[m].size() = Candidate[m]
	//vector <vector <CanonGraph<unordered_set>>> Canon_rep_partition(total_num_edges + 1);
	//vector <unordered_map<int, Graph<unordered_set>>> VecMapGraphPartition(total_num_edges + 1);
	vector <unordered_map<CanonGraph<unordered_set>, int>> VecMapCanonPartition(total_num_edges + 1);
	//Graph<unordered_set> G; 
	CanonGraph<unordered_set> canon_G;
	int counter_part = 0;//total number of input graphs
						 //int total_input_graphs = 0;
	MSG("Before part while");
	while (canon_G.read_canon_graph(isCanon_rep)) {
		MSG("inside the while");
		//while (G.read_nauty_g(isGraph) == true) {
		//DBG1(G);
		++counter_part;
		cout << counter_part << endl;
		//DBG1(counter_part);
		int num_undir_edge = (canon_G.edges).size() / 2;
		DBG1(canon_G);
		DBG1(canon_G.edges.size());
		DBG1(num_undir_edge);
		//DBG1(num_undir_edge);
		//Graph_partition[num_undir_edge].push_back(G);
		//VecMapGraphPartition[num_undir_edge][counter_part] = G;
		//if (canon_G.read_canon_graph(isCanon_rep)) {
		//	//Canon_rep_partition[num_undir_edge].push_back(canon_G);
		VecMapCanonPartition[num_undir_edge][canon_G] = counter_part;
		//}
		//else {
		//	MSG("Error in partitioning");
		//	throw(-1);
		//}

	}//while of input graphs
	 /*int counter_edge = 0;
	 int total_canon_graphs = 0;
	 for (const auto & m : VecMapCanonPartition) {
	 total_canon_graphs += m.size();
	 DBG1(counter_edge);
	 DBG1(m.size());
	 ++counter_edge;
	 }*/
	DBG1(counter_part);
	//DBG1(total_canon_graphs);
	//return 0;

	//DBG1(Graph_partition[6].size()); DBG1(Canon_rep_partition[6].size());
	ifsgraph.close();//input graph file
	ifscanon_rep.close();//canon file of input graphs
	MSG("Done with partitioning");
	cout << "Done with partitioning" << endl;
	//folder where all files created during this code will be stored//
	ostringstream foldername;
	foldername << Graph_num_ver << "PCG_Gen_Time"
		<< time_limit;
	mkdir(foldername.str().c_str());

	//creating output files (PCG generated and the graphs which needs further investigation)//
	string sfoundPCGsFile, sNotfoundgraphFile;
	sfoundPCGsFile = foldername.str() + "/" + to_string(Graph_num_ver) +
		"PCG_Gen_time" + to_string(time_limit) + ".txt";
	sNotfoundgraphFile = foldername.str() + "/" + to_string(Graph_num_ver) +
		"Further_invest_time" + to_string(time_limit) + ".txt";
	ofstream ofsfoundPCGsFile, ofsNotfoundgraphFile;
	try {
		ofsfoundPCGsFile.open(sfoundPCGsFile);
	}
	catch (const exception& e) {
		cerr << "Couldn't open PCG found '" << sfoundPCGsFile
			<< "' for reading!" << endl;
		throw(e);
	}
	if (!ofsfoundPCGsFile.is_open()) {
		cerr << "Error: PCG found file is not initialized!" << endl;
		throw(-1);
	}

	try {
		ofsNotfoundgraphFile.open(sNotfoundgraphFile);
	}
	catch (const exception& e) {
		cerr << "Couldn't open '" << sNotfoundgraphFile
			<< "' for reading!" << endl;
		throw(e);
	}
	if (!ofsNotfoundgraphFile.is_open()) {
		cerr << "Error:  File of graphs for further investigation is not initialized!" << endl;
		throw(-1);
	}
	ostream *osfoundPCGsFile = &ofsfoundPCGsFile;
	ostream *osNotfoundgraphFile = &ofsNotfoundgraphFile;
	//opening tree file//
	ifstream ifsTree;
	try {
		ifsTree.open(treeFile);
	}
	catch (const exception& e) {
		cerr << "Couldn't open Tree file '" << treeFile
			<< "' for reading!" << endl;
		throw(e);
	}
	if (!ifsTree.is_open()) {
		cerr << "Error, input tree not initialized!" << endl;
		throw(-1);
	}
	istream *isTree = &ifsTree;

	/// creating maps to store paths between any two vertices of a tree
	///also storing edges in common indexing form i.e., first n are external and remaining are internal edges
	Graph<unordered_set> T;
	int tree_counter = 0;//zero corresponds to 1st tree

	vector <unordered_map < edge, unordered_set<edge> >> Vec_Tree_paths(num_trees);
	//	DBG1(Vec_Tree_paths.size());
	vector <Graph<unordered_set>> VecTrees;// preparing a vector of trees
	vector <unordered_set<vertex>> VecTreeLeaves(num_trees);//vector of leaves of all trees
	vector <vector<edge>> VecTreeEdges(num_trees);//for edges of all trees external first and then internal
	while (T.read_nauty_g(isTree) == true) {


		//DBG1(tree_counter);
		//unordered_set<vertex> T_leaves;
		T.get_leaves(VecTreeLeaves[tree_counter]);
		VecTrees.push_back(T);
		Find_Pathedges(T, Vec_Tree_paths, tree_counter);
		Common_indexed_edges(T, VecTreeEdges, tree_counter);
		++tree_counter;
		DBG1(tree_counter);
		//return 0;
	}//creating path maps
	ifsTree.close();
	DBG1(VecTrees.size());
	DBG1(VecTreeLeaves.size());
	DBG1(VecTreeEdges.size());
	//return 0;
	//return 0;
	/////PCG Generation ////
	mt19937 mt;//better to use this
			   //mt19937 mt((int)time(0));//for random generation
			   //ifsTree.clear();
			   //ifsTree.seekg(0, ios::beg);// going back to the begining of the tree file
			   //if (!ifsTree.seekg(0, ios::beg))
			   //{
			   //	cout << "Error: seek error" << endl;
			   //	//return;
			   //}
	*osfoundPCGsFile << "weight interval for generator: for non-leaves [1, 50] and for leaves [1, 20]" << endl;
	*osfoundPCGsFile << "-------------------------------------------------------------------------------------" << endl;
	int Pcg_found_counter = 0;//total number of non-iso Pcgs generated
	int total_PCGs_generated = 0;
	int round_counter = 0;
	double time_spent = 0;
	clock_t start_t = clock();
	bool time_breaker = false;// when all input graphs are found and still time left then don't go for next round
	while (time_spent / CLOCKS_PER_SEC <= time_limit) {//while for time//
													   //A: creating input Nauty file for each round//

		MSG("~~~~~~~~~~NEXT ROUND ~~~~~~~~");
		cout << "~~~~~~~~~~NEXT ROUND ~~~~~~~~" << endl;
		++round_counter;
		string sInputNauty;
		sInputNauty = foldername.str() + "/Nauty_Input.txt";
		ofstream ofsInputNauty;
		ofsInputNauty.open(sInputNauty);
		if (!ofsInputNauty.is_open()) {
			cerr << "Error, Input Nauty for PCGs generated not initialized!"
				<< endl;
			throw(-1);
		}
		ostream *osInputNauty = &ofsInputNauty;
		string sCanonFileName;//the string needs to be written in input nauty string
		sCanonFileName = foldername.str() + "/Canon_round_PCGs.txt";
		vector <Graph<unordered_set>> VecPCGGenRound;
		vector <Graph<unordered_set>> VecWitnessRound;
		vector <int> WitnessIndex;
		vector <double> Dmin, Dmax, Dminp, Dmaxp;
		//preparing weight vector//
		vector <int> VecWeight;
		VECWEIGHT(VecWeight, Graph_num_ver, mt);
		vector<unordered_map <edge, int>> MAPEdgeWeight;
		vector<unordered_map<int, vector<edge>>> MAPEdgesDist;
		//DBG1D(VecWeight); DBG1(VecWeight.size());

		//Graph<unordered_set> T;

		int round_tree_counter = 0;
		int total_pcg_gen_round = 0;
		for (const auto &T : VecTrees) {
			MSG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			MSG("             NEW Tree");
			MSG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			//DBG1(T);
			//while (T.read_nauty_g(isTree) == true) {
			//DBG1(T);

			//MSG("While tree");
			//getting weights for edges//
			unordered_map <edge, int> mapEdgeWeight;
			MAP_EDGE_WEIGHT(VecWeight, VecTreeEdges[round_tree_counter],
				mapEdgeWeight);
			//DBG1D(VecTreeEdges[round_tree_counter]);
			//DBG1D(mapEdgeWeight);
			//return 0;
			set <int> SetDist; //containing set of dist(a,b,t,w) in strically increasing order(d_1<d_2...<d_k see line 7 of the algo)
			unordered_map<int, vector<edge>> MapEdgesDist;//map containing class[{a,b}]
														  ////MapEdgesDist[i].size() = p_i///
			SetMapDist(Graph_num_ver, Vec_Tree_paths[round_tree_counter],
				mapEdgeWeight, SetDist, MapEdgesDist);
			//preparing q//
			unordered_map<int, int> MapCumSum;// ((SetDist.size() + 1));//cumulative sum = q
											  //MSG("dd");
			MapCumSum[0] = 0;
			int CumSum_counter = 0;

			for (const auto &dist : SetDist) {

				++CumSum_counter;
				MapCumSum[CumSum_counter] = MapCumSum[CumSum_counter - 1] + (MapEdgesDist[dist].size());
				//DBG1(dist); DBG1(MapCumSum[CumSum_counter]);
			}
			//return 0;
			//DBG1D(MapCumSum);
			vector <int> VecSetDist(ALL(SetDist));
			DBG1D(VecSetDist);
			//return 0;

			for (int i = 0; i <= VecSetDist.size() - 1; i++) {//const auto &dist_1 : SetDist) {
				double d_min;
				d_min = VecSetDist[i] - 0.5;//(MapEdgesDist[dist_1].size()) - (1 / 2);
				for (int j = i; j <= VecSetDist.size() - 1; j++) {//(const auto &dist_2 : SetDist) {
																  //if (VecSetDist[j] < VecSetDist[i]) continue;
																  //else {//dist_2 >= dist_1
					DBG1(i); DBG1(j);
					double d_max;
					d_max = VecSetDist[j] + 0.5;//(MapEdgesDist[dist_2].size()) + 1 / 2;
					if (j != VecSetDist.size() - 1) {
						for (int ip = j + 1; ip <= VecSetDist.size() - 1; ip++) {//second interval
							double d_minp;
							d_minp = VecSetDist[ip] - 0.5;
							for (int jp = ip; jp <= VecSetDist.size() - 1; jp++) {
								double d_maxp;
								d_maxp = VecSetDist[jp] + 0.5;
								int num_PcgEdges; //m
								num_PcgEdges = MapCumSum[j + 1] - MapCumSum[i] + MapCumSum[jp + 1] - MapCumSum[ip];
								DBG1(num_PcgEdges);
								//if (Graph_partition[num_PcgEdges].size() >= 1) {//generate the PCG(T,w,dmin,dmax)
								if (VecMapCanonPartition[num_PcgEdges].size() >= 1) {	//DBG1(Graph_partition[num_PcgEdges].size());
									//DBG1(num_PcgEdges);
									//MSG("!!!!!!!Constructing PCG!!!!!!!");
									//unordered_set<vertex> Tleaves;//better to use Vector of leaves of all trees
									//T.get_leaves(Tleaves);

									//Graph<unordered_set> Pcg(Tleaves);//vertices same as leaves
									//adding edges
									// for (const auto &dist_3 : SetDist) {
									Graph<unordered_set> Pcg(VecTreeLeaves[round_tree_counter]); //
									for (int k = i; k <= j; k++) {
										for (const auto &ee : MapEdgesDist[VecSetDist[k]])
										{
											//DBG1(ee);
											Pcg.add_undirected_edge(ee);
										}
									}//k
									for (int kp = ip; kp <= jp; kp++) {
										for (const auto &ee : MapEdgesDist[VecSetDist[kp]])
										{
											Pcg.add_undirected_edge(ee);
										}
									}//kp
									MSG("One PCG generated");

									DBG1(Pcg);
									VecPCGGenRound.push_back(Pcg);
									++total_PCGs_generated;
									++total_pcg_gen_round;
									DBG1(total_PCGs_generated);
									DBG1(total_pcg_gen_round);
									VecWitnessRound.push_back(T);
									WitnessIndex.push_back(round_tree_counter);
									Dmin.push_back(d_min); Dmax.push_back(d_max);
									Dminp.push_back(d_minp); Dmaxp.push_back(d_maxp);
									MAPEdgeWeight.push_back(mapEdgeWeight);
									MAPEdgesDist.push_back(MapEdgesDist);
									

									//MSG("%%%%% Nauty Section%%%%%%%%");
									//input for nauty//
									Input_Nauty(Pcg, ofsInputNauty, sCanonFileName);

									//return 0;




								}
								else {
									MSG("Partition size = 0");
								}// continue;//VecMapGraphPartition[num_PcgEdges].size() = 0

								 //}
								 //DBG1(dist_2);
								 //if (time_breaker) break;
							}//for jp
						}//for ip

					}//if j not VecSetDist.size() - 1
				}//dist_2; j
				MSG("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
				MSG("             J ends");
				MSG("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
				DBG1(i);
				//if (time_breaker) break;
			}//dist_1; i

			 //if (time_breaker) break;
			 //return 0;

			++round_tree_counter;
			MSG("After i and j");
			DBG1(round_tree_counter);
			mapEdgeWeight.clear();
			SetDist.clear();
			MapEdgesDist.clear();



			//}//trees while
		}//tree for

		DBG1(VecPCGGenRound.size());

		clock_t end_t = clock();
		time_spent = double(end_t - start_t);
		///round ends///
		// going back to the begining of the tree file//
		//ifsTree.clear();
		////DBG1(round_tree_counter);
		//round_tree_counter = 0;//resetting the tree counter
		//					   //DBG1(round_tree_counter);

		//ifsTree.seekg(0, ios::beg);
		//if (!ifsTree.seekg(0, ios::beg))
		//{
		//	cout << "Error: seek error" << endl;
		//	//return;
		//}
		//MSG("clearing");


		
						  //VecPcgGenerated.clear();//clearing Pcg generated vector for next round
						  //ofsInputNauty.clear();//clearing input nauty file for next round
		ofsInputNauty.close();
		DBG1(total_pcg_gen_round);
		//return 0;
		if (total_pcg_gen_round != 0) {//some graphs are generated
									   ////////Duplication section/////
			MSG("Canon section");
			cout << "Canon section" << endl;
			///Generating canonical rep of InputNauty//
			string for_dreanaut;
			for_dreanaut = "dreadnaut.exe < " + sInputNauty;
			////cout << " string dreadnaut" << for_dreanaut << endl;
			//MSG("///////// Generating Canon Representations //////////");
			////if exists then first delete//
			if (File_exists(sCanonFileName)) remove(sCanonFileName.c_str());
			system(for_dreanaut.c_str());
			MSG("////////Checking Duplication section/////");
			cout << "////////Checking Duplication section/////" << endl;
			////////Checking Duplication section/////

			ifstream ifsCanonFileName;
			try {
				ifsCanonFileName.open(sCanonFileName);
			}
			catch (const exception& e) {
				cerr << "Couldn't open canon file of PCGs generated  '" << sCanonFileName
					<< "' for reading!" << endl;
				throw(e);
			}
			if (!ifsCanonFileName.is_open()) {
				cerr << "Error: Canon_rep file of PCGs generated is not initialized!" << endl;
				throw(-1);
			}

			istream *isCanonFileName = &ifsCanonFileName;
			CanonGraph<unordered_set> PCG_graph_rep;// representation of a graph from original file containing all graphs with Graph_num_ver
			int canon_index = 0;// 
			typedef unordered_set< CanonGraph<unordered_set> > graphSet;
			typedef graphSet::iterator graphSetIter;
			//int Pcg_found_counter = 0;
			while (PCG_graph_rep.read_canon_graph(isCanonFileName)) {
				int num_edges;
				num_edges = (VecPCGGenRound[canon_index].edges).size() / 2;//number of edges of the Pcg at index canon_index
																		   //choosing one Pcg and its canon

				int num_edges_1 = (PCG_graph_rep.edges).size() / 2;
				if (DEBUG) {
					PCG_graph_rep.pretty_print();
					VecPCGGenRound[canon_index].pretty_print();
				}
				if (num_edges != num_edges_1) {
					DBG1(num_edges); DBG1(num_edges_1);
					cout << "Incorrectly read from partition and input rep" << endl;
					MSG("Incorrectly read from partition and input rep");
					throw(-1);
				}
				//if (Canon_rep_partition[num_edges].size() != 0) {//if non empty
				//if (VecMapCanonPartition[num_edges].size() != 0) {
				auto It = VecMapCanonPartition[num_edges].find(PCG_graph_rep);//using an iterator
				if (It != VecMapCanonPartition[num_edges].end()) {//PCG_graph_rep is a rep of new graph
					++Pcg_found_counter;
					DBG1(It->first.signature);
					DBG1(It->first);
					DBG1(PCG_graph_rep.signature);
					DBG1(PCG_graph_rep);
					//char c;
					//cin >> c;//good

					//int p = It->second;// to get the image of unordered map at It->first ; this will be an integer which represents the counter_part value of the corresponding graph in VecMapGraphPartition
					DBG1(VecMapCanonPartition[num_edges].size());
					//It->first.pretty_print(osfoundPCGsFile, Pcg_found_counter); // the graphs in the given files
					//*osfoundPCGsFile << endl;
			
					VecPCGGenRound[canon_index].pretty_print(osfoundPCGsFile, Pcg_found_counter);
					//an isomorphic PCG that also exists in the given file
					*osfoundPCGsFile << endl;

					VecMapCanonPartition[num_edges].erase(It);//removing the found graph's canon_rep
					DBG1(VecMapCanonPartition[num_edges].size());

					*osfoundPCGsFile << "Witness tree:  ";
					*osfoundPCGsFile << endl;
					VecWitnessRound[canon_index].pretty_print_tree(osfoundPCGsFile, WitnessIndex[canon_index] + 1);
					*osfoundPCGsFile << endl;
					/**osfoundPCGsFile << "Weight vector:  " ;
					for (const auto &w: VecWeight) {
						*osfoundPCGsFile << w << ", ";
					}
					*osfoundPCGsFile << endl;				
					
					*osfoundPCGsFile << endl;*/

					*osfoundPCGsFile << "Intervals: " << "[" << Dmin[canon_index] << ", " << Dmax[canon_index] << "]; "
						<< "[" << Dminp[canon_index] << ", " << Dmaxp[canon_index] << "] " << endl;
					*osfoundPCGsFile << endl;

					//*osfoundPCGsFile << "dmin: " << Dmin[canon_index] << ", " << "dmax: " << Dmax[canon_index] << ", " 
					//	<< "dminp: " << Dminp[canon_index] << ", "
					//	<< "dmaxp: " << Dmaxp[canon_index] << endl;
					//*osfoundPCGsFile << endl;

					*osfoundPCGsFile << "Edge : Weight  " << endl;
					for (const auto& elem : MAPEdgeWeight[canon_index])
					{
						*osfoundPCGsFile << elem.first << " : " << elem.second << "\n";
					}
					*osfoundPCGsFile << endl;

					*osfoundPCGsFile << "Pairs : Distance " << endl;
					for (const auto& elem : MAPEdgesDist[canon_index])
					{

						int x = 1;
						for (const auto& cc : elem.second){
							if (x != elem.second.size()) *osfoundPCGsFile << cc << "; ";
							else *osfoundPCGsFile << cc << " : ";
							++x;
						}
						*osfoundPCGsFile << elem.first << " ";
						*osfoundPCGsFile << endl;
					}
					*osfoundPCGsFile << endl;
					//VecMapGraphPartition[num_edges][p].pretty_print(osfoundPCGsFile, Pcg_found_counter);//writing in Pcg found file
					//DBG1(VecMapGraphPartition[num_edges].size());
					//VecMapGraphPartition[num_edges].erase(p);//removing the found graph
					//DBG1(VecMapGraphPartition[num_edges].size());

					// can use another iterator for the above line//
					MSG(">>>>>>>>>>>>>>>>>>>>>");
					//cout << ">>>>>>>>>>>>>>>>>>>>>" << endl;
					cout << "Pcg_found_counter= " << Pcg_found_counter << endl;
					DBG1(Pcg_found_counter);
					if (Pcg_found_counter == counter_part) {
						time_breaker = true;// all
						MSG("All input graphs found to be PCGs");
						cout << "All input graphs found to be PCGs" << endl;
						cout << "Time taken: " << time_spent << endl;
					}
					MSG("NEW PCG FOUND");
					//cout << "NEW PCG FOUND" << endl;
				}
				if (time_breaker) break; //if all input graphs found then no need to further check
				++canon_index;
			}//duplication check
			ifsCanonFileName.close();
		}//if number of graphs generated is greater than 0

		 //DBG1(total_pcg_gen_round);
		 //total_pcg_gen_round = 0;
		DBG1(total_pcg_gen_round);
		VecWeight.clear();//clearing weight vector for the next round
		VecWitnessRound.clear();
		WitnessIndex.clear();
		Dmin.clear(); Dmax.clear(); Dminp.clear(); Dmax.clear();
		MAPEdgesDist.clear(); MAPEdgeWeight.clear();
		VecPCGGenRound.clear();//clearing the vector of Pcg generated in this round
							   //////to remove last input nauty file
							   //if (time_spent / CLOCKS_PER_SEC <= time_limit) {
							   //remove(sInputNauty.c_str());//check after generation
							   //}

		if (time_breaker) break;//if all input graphs are found then no need of further checking
	}//while for time
	cout << "round counter = " << round_counter << endl;
	DBG1(round_counter);
	DBG1(total_PCGs_generated);
	//cout << "total_PCGs_generated = " << total_PCGs_generated << endl;

	//ofsInputNauty.close();//input file for nauty for each round of the tree file
	ifsTree.close();//input tree file closed
	ofsfoundPCGsFile.close();//PCGS_generated file
							 ///The graphs left in Graph_partition need further investigation//
	int total_graph_left = 0;
	for (int num_edges = 0; num_edges <= VecMapCanonPartition.size() - 1; num_edges++) {//loop on no of edges
																						//int graph_left_num_edges = 0;
		for (const auto &iter : VecMapCanonPartition[num_edges]) {//loop on graphs with num_edges
			++total_graph_left;
			iter.first.pretty_print(osNotfoundgraphFile, total_graph_left);//writing in file of not found
																		   //++graph_left_num_edges;
		}
	}

	DBG1(total_graph_left);
	ofsNotfoundgraphFile.close();//graphs need further investigation
	DBG1(Pcg_found_counter);
	cout << "total graphs left = " << total_graph_left << endl;
	cout << "PCGs found counter = " << Pcg_found_counter << endl;
	cout << "total_PCGs_generated = " << total_PCGs_generated << endl;
}


