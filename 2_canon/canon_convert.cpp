
//Getting canonical forms of graphs
////leaves are always labeled from 0,1,...
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
// checking if a file exits//
inline bool File_exists(const string& name) {// 1 if file exists
	ifstream f(name.c_str());
	return f.good();
}


int main(int argc, char **argv) {
	string graphFile;// canongraphFile;
	graphFile = string(argv[1]);
	int Graph_num_ver = atoi(argv[2]);
	//canongraphFile = string(argv[2]);
	//sig_piFile = string(argv[3]);//

	MSG("Hello");

	ifstream ifsGraph;
	// open filestream	
	try {
		ifsGraph.open(graphFile);
	}
	catch (const exception& e) {
		cerr << "Couldn't open '" << graphFile
			<< "' for reading!" << endl;
		throw(e);
	}

	if (!ifsGraph.is_open()) {
		cerr << "Error, infile not initialized!" << endl;
		throw(-1);
	}
	istream *isGraph = &ifsGraph;

	// creating an output file containing input for dreadnaut//
	string ext(".txt");
	string graphtree_no;
	//graphtree_no = to_string(G_num_ver) + "Graph#" + to_string(graph_counter) + "Tree#" + to_string(tree_counter);
	stringstream ss;
	ss << "input_nauty_" << to_string(Graph_num_ver) << "Graphs" << ext;
	ofstream ofsfile_out;// (ss.str());// file where to write
	ofsfile_out.open(ss.str());
	if (!ofsfile_out.is_open()) {
		cerr << "Error, input_nauty_" << to_string(Graph_num_ver) 
			<< "Graphs file not initialized!" << endl;
		throw(-1);
	}
	ostream *osfile_out = &ofsfile_out;

	//size_t graph_counter = 0; // counter for the index of the graph in the input file
	Graph<unordered_set> G;// since G changes
	while (G.read_nauty_g(isGraph) == true) {// getting a graph
		//DBG1(G);
		int G_num_ver = G.getN();//size of G
			//trivial coloring//
		for (const auto &v : G.vertices) {
			G.colorMap[v] = 0;
		}
		
		//// due to technical reasons convert T_sigma to another graph T_sigmaP (the original graph will not change)// 
		CanonGraph<unordered_set> GP(G);
		//writing T_sigmaP as input for nauty//
		string dreadnaut_input;
		//DBG1D(T_sigmaP.colorMap);
		dreadnaut_input = GP.for_dreadnaut(false, "canon_" + to_string(Graph_num_ver) + "All_Graph.txt");
		//DBG1(dreadnaut_input);
		*osfile_out << dreadnaut_input << endl;
	}
		ofsfile_out.close();
		////Phase II: generating a file containing canoncial representation of all graphs in above file
		string for_dreanaut;

		for_dreanaut = "dreadnaut.exe < " + ss.str();
		//cout << " string dreadnaut" << for_dreanaut << endl;
		MSG("//////////////////////////// Phase-II///////////////////////");
		// be carefull while calling this command again, 
		///first remove/clear the file where canon represetation will be saved
		string canon_output = "canon_" + to_string(Graph_num_ver) + "All_Graph.txt";
		if (File_exists(canon_output)) remove(canon_output.c_str());
		system(for_dreanaut.c_str());

		///Extra///////
		ifstream ifscanon_rep;
		try {
			ifscanon_rep.open(canon_output);
		}
		catch (const exception& e) {
			cerr << "Couldn't open '" << canon_output
				<< "' for reading!" << endl;
			throw(e);
		}
		if (!ifscanon_rep.is_open()) {
			cerr << "Error: Canon_rep file is not initialized!" << endl;
			throw(-1);
		}
		
		istream *isCanon_rep = &ifscanon_rep;//canon_rep of original graphs
		typedef unordered_set< CanonGraph<unordered_set> > graphSet;
		typedef graphSet::iterator graphSetIter;
		graphSet uniqGraphstest;
		CanonGraph<unordered_set> graph_rep;
		size_t test_counter = 0;
		size_t total_rep = 0;


		//Graph<unordered_set> graph;
		while (graph_rep.read_canon_graph(isCanon_rep)) {
			//++All_graph_rep_counter;
			//DBG1(All_graph_rep_counter);
			//test//
			//unordered_set< CanonGraph<unordered_set> >::const_iterator got = uniqGraphstest.find(graph_rep);
			++total_rep;
			//uniqGraphstest.insert(graph_rep);
			if (uniqGraphstest.find(graph_rep) == uniqGraphstest.end()) {
				++test_counter;
				uniqGraphstest.insert(graph_rep);

			}
			else {
				
				//DBG1(graph_rep);
				//DBG1(total_rep);
				cout << "duplication at: " << total_rep << endl;
				//return 0;

			}
		}
		DBG1(test_counter);
		cout << "number of non isomorphic graphs: " << test_counter << endl;
		DBG1(total_rep);
		//DBG1(uniqGraphstest.si);
			

}

