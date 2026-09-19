/*
 * graph.h
 *
 *  Created on: Jul 24, 2018
 *      Author: shurbevski
 changes by Azam
 updated on 12-1-2018 changes: function: normalize
 updated on 12-19-2018 changes: dfs_biconn
 updated on 12-21-2018 changes: functions: find_components and test_biconn

 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <vector>
#include <stack>
#include <deque>
#include <algorithm>
//#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <string>
#include <sstream>
#include <istream>
#include <ostream>
#include <exception>

#include "debug.h"

//#include <ilcplex/ilocplex.h>

//ILOSTLBEGIN


#define ALL(x) x.begin(), x.end()

typedef size_t vertex;
typedef unordered_map< vertex, vertex > vertexMap;
typedef unordered_map< vertex, vertex >::value_type vMapValT;
//typedef pair < vertex, vertex > edge;
//typedef pair <edge, IloNumVar> edgeVarPair;

//template <typename iterable>
//std::ostream & operator<<(std::ostream &os, iterable C){
//	for (auto &c: C){
//		os << c << ", ";
//	}
//	os << endl;
//	return os;
//}

template <typename vv>
class tEdge: public std::pair<vv, vv>{
public:
	//vv u, v;

	bool operator <( const tEdge<vv> &other){
		return (this->first < other.first ||
				(this->first == other.first && this->second < other.second));
	}

	bool operator ==( const tEdge<vv> &other){
		return (this->first == other.first && this->second == other.second);
	}

	tEdge (const vv& f = 0, const vv& s = 0): std::pair<vv, vv>(f, s){
		//this->u = f;
		//this->v = s;
	}

	tEdge undirect() const{ //!
		vertex v = this->first;
		vertex u = this->second;

		vertex uu (min(u, v));
		vertex uv (max(u, v));
		return tEdge(uu, uv);
	}

	vv u(){ return this->first;}
	vv v(){ return this->second;}


	template <typename V>
	friend std::ostream& operator<<(std::ostream &, const tEdge<V> &);
};

template<typename vv>
std::ostream & operator <<(std::ostream &os, const tEdge<vv> &ee){
	os << "(" << ee.first << "," << ee.second <<")";
	return os;
}

namespace std{
template <typename vv>
class hash< tEdge<vv> >{
public:
	size_t operator()(const tEdge<vv> &ee) const{
		size_t res = std::hash<vertex>{} (ee.first);
		res ^= std::hash<vertex>{} (ee.second) + 0x9e3779b9 + (res<<6) + (res>>2);
		return res;
	}
};
}

typedef tEdge<vertex> edge;
//typedef unordered_map<edge, IloNumVar> edgeVarMap;





template<typename Container, typename Element>
bool isIn(const Element &item, const Container &vv){
	return std::find(vv.begin(), vv.end(), item) != vv.end();
}

//bool operator == (IloNumVar x, IloNumVar y){
//	return x.getId() == y.getId();
//	// x.getName() == y.getName()  ??
//}


template < template<class...> class container >
// #define unordered_set container;
class Graph{

public:


	typedef container<vertex> vertexSet;
	typedef container<edge> edgeSet;
	typedef std::unordered_map <vertex, vertexSet > adjList;
	typedef std::unordered_map<edge, double> weightMap;
	typedef std::unordered_map<vertex, size_t> vrtxColorMap;
	typedef std::vector< container<vertex> > vrtxColorPartition;

	size_t n;
	vertexSet vertices;
	edgeSet edges;
	adjList outAdjList; // a list of edges with a fixed tail
	adjList inAdjList;	// list of edge with a fixed head
	weightMap weight;
	string graphID;

	vrtxColorMap colorMap;
	vrtxColorPartition colorPartition;


	// template < template<class...> class vContainer >
 	void initAdj( /* const vContainer<vertex> &X */ ){
		// size_t sz = X.size();
		// this->n = sz;
		// this->vertices = vertexSet(ALL(X));
		for (const vertex& v: this->vertices){
			vertexSet vs;
			try{
				this->inAdjList[v].clear();
				this->outAdjList[v].clear();
			} catch (const std::exception &ex){
				cerr << "Creating new adjacency lists for " << v << endl;
				this->inAdjList.insert(typename adjList::value_type(v, vs));
				this->outAdjList.insert(typename adjList::value_type(v, vs));
			}
		}
		edges.clear();
		weight.clear();
	}

	inline size_t getN() const{
		return this->vertices.size();
	}

	Graph (size_t sz = 0){
		// this->n = sz;
		this->vertices.clear();
		// vertexSet vs;
		for (size_t i = 0; i < sz; i++)
			this->vertices.insert(i);
		// this->vertices = vertexSet(ALL(vs));
		this->initAdj();
	}

	//Graph (const IloNumArray2 &dist, const double inf = 32768): Graph(dist.getSize()){
	//	this->edges.clear();
	//	size_t sz = dist.getSize();
	//	this->vertices.clear();
	//	for (size_t i = 0; i < sz; i++){
	//		this->vertices.insert(i);
	//	}
	//	this->initAdj();
	//	for (size_t i = 0; i < sz; i++){
	//		for (size_t j = 0; j < sz; j++){
	//			if (dist[i][j] != inf){
	//				edge e(i, j);
	//				this->edges.insert(e);
	//				this->outAdjList[i].insert(j); // edge ij has tail at i
	//				this->inAdjList[j].insert(i); // and head at j
	//				weightMap::value_type ew(e, dist[i][j]);
	//				this->weight.insert(ew);
	//			}
	//		}
	//	}
	//}

	Graph (const Graph<container> &gg){
		// copy constructor
		//this->n = gg.n;
		this->vertices = vertexSet(ALL(gg.vertices));
		this->edges = edgeSet(ALL(gg.edges));
		this->weight = unordered_map<edge, double>(ALL(gg.weight));
		this->inAdjList = adjList(ALL(gg.inAdjList));
		this->outAdjList = adjList(ALL(gg.outAdjList));
		this->colorMap = vrtxColorMap(ALL(gg.colorMap));
		this->colorPartition = vrtxColorPartition(ALL(gg.colorPartition));
	}

	/*
	 * Initialize a graph with all singletons given
	 * a set of vertices vSet
	 * in and out adjacency lists will be initialized as well
	 */
	Graph (const vertexSet &vSet){
		this->vertices.clear();
		this->vertices.insert(ALL(vSet));
		this->n = vSet.size();
		this->initAdj();
	}

	/*
	 * construct a graph given a set of edges eSet
	 */
	Graph (const edgeSet &eSet){
		this->vertices.clear();
		this->edges = edgeSet(ALL(eSet));
		for (auto &ee: eSet){
			vertex u, v;
			u = ee.first;
			if (! isIn(u, this->vertices)) this->vertices.insert(u);
			v = ee.second;
			if (! isIn(v, this->vertices)) this->vertices.insert(v);
			// checking if isIn(u, vertices) is superflous
			// if container is set or unordered set, but not so much if vector!
		}
		this->n = this->vertices.size();
		this->initAdj();
		for (auto &ee: eSet){
			vertex u, v;
			u = ee.first;
			v = ee.second;
			this->outAdjList[u].insert(v);
			this->inAdjList[v].insert(u);
		}
	}

	/*Graph(const edgeSet &eSet, const IloNumArray2 &dist): Graph(eSet){
		for (auto &e: eSet){
			vertex u, v;
			u = e.first;
			v = e.second;
			weightMap::value_type ew (e, dist[u][v]);
			this->weight.insert(ew);
		}
	}

	Graph (const edgeSet &eSet, const weightMap &wMap): Graph(eSet){
		this->weight.clear();
		for (const auto &e: eSet){
			weightMap::value_type ew(e, wMap[e]);
			this->weight.insert(ew);
		}
	}
*/
	/*
	 * Given a container X (vector, set...) of vertices,
	 * return the graph induced by X.
	 * The vertices of the resulting graph will be
	 * renumbered 0 to |X|-1,
	 */
	template <template<class...> class containerX>
	Graph<container> induce(const containerX<vertex> &X) const {
		//size_t sz = X.size();
		Graph<container> res(X);
		//vector<vertex> verts(ALL(X));

		//res.outAdjList.clear();
		//res.inAdjList.clear();
		for (auto &u : res.vertices) {
			//vector<vertex> tt;
			//res.outAdjList.push_back(tt);
			//res.inAdjList.push_back(tt);
			//for (auto &v: res.vertices){
			//if (isIn(v, this->inNeighbors(u)))
			//res.inAdjList[u].insert(v);
			//if (isIn(v, this->outNeighbors(u)))
			//res.outAdjList[u].insert(v);
			//			}
			for (auto &v : res.vertices) {
				if (isIn(v, this->inNeighbors(u)))
					res.add_edge(v, u);
				if (isIn(v, this->outNeighbors(u)))
					res.add_edge(u, v);
			}
		}
		return res;
	}

	/* Return a graph G with edge e contracted
	 * this graph should not be changed
	 */
	void contract_edge(const edge &e, // e = uv, to be contracted to u
						Graph<container> &G // store the resulting graph
						) const{
		vertex u = e.first;
		vertex v = e.second;

		// uOut = adjList(ALL(this->outAdjList[u]));
		// vIn = adjList(ALL(this->inAdjList[v]));
		// weightMap.clear();

		G = Graph<container>(*this);

		G.weight.erase(e);
		G.edges.erase(e); //first erase e from the resulting graph

		edge ie(v, u);
		G.weight.erase(ie);
		G.edges.erase(ie);

		// erase all uw edges from the graph
		for (const vertex &w: G.outAdjList[u]){
			edge te(u,w);
			G.edges.erase(te);
			G.weight.erase(te);
		}
		// redirect the tails of all vw vertices to u
		G.outAdjList[v].erase(u);
		G.outAdjList[u].clear();
		G.outAdjList[u].insert(ALL(G.outAdjList[v]));
		for (const vertex &w : G.outAdjList[v]){
			edge outU(u, w);
			edge outV(v, w);
			G.weight[outU] = G.weight[outV];
			G.weight.erase(outV);
			G.edges.erase(outV);
			G.edges.insert(outU);
		}
		G.outAdjList.erase(v);
		// erase all edges with head at v
		for (const vertex &w: G.inAdjList[v]){
			edge inV(w, v);
			G.edges.erase(inV);
			G.weight.erase(inV);
		}
		G.inAdjList.erase(v);
		G.vertices.erase(v);
		G.outAdjList[u].erase(u); // remove u in case edge vu exists in this
		G.inAdjList[u].erase(v);

		// Next we need to deal with the
		// adjacency lists of all other vertices
		for (const vertex &w: G.vertices){
			G.outAdjList[w].erase(v); // wv edges are just erased
			G.inAdjList[w].erase(u); // uw edges are erased
			if (G.inAdjList[w].erase(v) != 0) G.inAdjList[w].insert(u);
			//erase return the number of erase elements
		}
		// That's all folks!
	}

	/* Given a graph T and  a maping sigma: V(this) -> V(T)
	 * add to T all (sigma(u), sigma(v)), uv \in E(this)
	 */
	void unionMapped (const Graph <container> &T,
					const unordered_map<vertex, vertex> &sigma,
					Graph &H // to store the result
					) const {
		H = Graph<container>(T);
		for (const auto &ee : this->edges){
			vertex u, v, su, sv;
			u = ee.first;
			v = ee.second;
			su = sigma.at(u);
			sv = sigma.at(v);
			edge se(su, sv);
			H.add_edge(se);
		}
	}

	size_t getOutEdges(const vertexSet vSet, edgeSet &outEdges) const{
		outEdges.clear();
		for (const auto &v: vSet){
			for (const auto &u: this->outAdjList[v]){
				if (! isIn(u, vSet)) outEdges.insert(edge(v, u));
			}
		}
		return outEdges.size();
	}

	size_t getInEdges(const vertexSet vSet, edgeSet &inEdges) const{
		inEdges.clear();
		for (const auto &v: vSet){
			for (const auto &u: this->inAdjList[v]){
				if (! isIn(u, vSet)) inEdges.insert(edge(u, v));
			}
		}
		return inEdges.size();
	}

	/* find the number of connected components in this graph
	 * store the vertices of the connected components
	 * in a container<vertices> comps
	 * return the number of connected components
	 */
	size_t find_components(	std::vector < vertexSet > &comps) const{
		comps.clear();
		std::unordered_map<vertex, bool> reached;
		typedef std::unordered_map<vertex, bool>::value_type vbp;
		for (auto &v: this->vertices){
			reached.insert(vbp(v, false));
		}

		for (auto &v: this->vertices){
			if (reached[v]) continue;
			std::deque<vertex> qq;
			qq.push_back(v);
			container<vertex> tSet;
			while (! qq.empty()){
				vertex s = qq.back();
				qq.pop_back();
				if (reached[s]) continue;
				reached[s] = true;
				tSet.insert(s);
				// check adjacent vertices ignoring edge direction
				for (const auto &u: this->inAdjList.at(s)){
					if (! reached[u]) qq.push_back(u);
				}
				for (const auto &u: this->outAdjList.at(s)){
					if (! reached[u]) qq.push_back(u);
				}
			}
			comps.push_back(tSet);
		}
		return comps.size();
	}

	//To know if the given graph is biconnected or not//
	// true if non-biconnected assuming that the given graph is a connected graph//
	bool test_biconn(const vertex &u,
							std::vector<container <vertex> > &blocks,
							std::deque<vertex> &vrtxStack,
							std::unordered_map<vertex, bool> &reached,
							std::unordered_map<vertex, vertex> &parent,
							std::unordered_map<vertex, size_t> &depth,
							std::unordered_map<vertex, size_t> &lowpt) const {
				if (reached[u]){ // this is not supposed to happen
					std::cerr << "Error in recursion" << std::endl;
					throw(-1);
				}
				// else:
				//DBG1(u);
				bool decision_var = false;// if true then non_biconnected and return true
				reached[u] = true;
				vrtxStack.push_back(u);
				size_t childCount = 0;
				bool isCut = false;
				for (const auto &v : this->outAdjList.at(u)) {
					//DBG1(v);
					if (reached[v]) {
						if (v != parent[u])
							lowpt[u] = min(lowpt[u], depth[v]);
					}
					else {
						++childCount;
						parent[v] = u;
						depth[v] = depth[u] + 1;
						lowpt[v] = depth[v];
						decision_var |= test_biconn(v, blocks, vrtxStack, reached, parent, depth, lowpt);
						lowpt[u] = min(lowpt[u], lowpt[v]);
						if (parent[u] != u && lowpt[v] >= depth[u])
						{
							isCut = true;
							//MSG("HERE");
							decision_var = true; // if we found a cut vertex then G is non-biconnected
							/*MSG("cut_vertex found lowpt[v] >= depth[u]");
							DBG1(u); DBG1(v);
							DBG1(lowpt[v]); DBG1(depth[u]); DBG1(depth[v]);
							DBG1(parent[u]); DBG1(parent[v]);*/
							//break;
						}
					}
				}  // for each neighbor v of u
					
				if (parent[u] == u && childCount >= 2) {
					isCut = true;
					decision_var = true; // if we found a cut vertex then G is non-biconnected
					//MSG("cut_vertex found parent[u] == u && childCount >= 2");
					//DBG1(u);
					//break;
				}						
				//if (isCut == true){
				//		
				//	container<vertex> newBlock;
				//	MSG("Discovered a cut vertex");
				//	DBG1(u);
				//	DBG1D(vrtxStack);
				//	//while (childCount > 0){
				//	//	vertex x = vrtxStack.back();
				//	//	newBlock.insert(x);
				//	//	while (x != u){
				//	//		vrtxStack.pop_back();
				//	//		x = vrtxStack.back();
				//	//		newBlock.insert(x);
				//	//		// if (uv.first == u || edgeStack.empty()) break;
				//	//	}
				//	//	DBG1D(newBlock);
				//	//	DBG1D(vrtxStack);
				//	//	blocks.push_back(newBlock);
				//	//	--childCount;
				//	//}
				//}

			if (decision_var == true) {
				//MSG("non-biconnected");
				//DBG1(decision_var);
				return true;
			}
			else
			{
				//DBG1(decision_var);
				//MSG("biconnected");
				return false;
			}
				/*MSG("Any thing");
				DBG1(blocks.size());
		return blocks.size();*/
	} // finished dfs
		//


//	void dfs_biconn(const vertex &u,
//					std::vector<container <vertex> > &blocks,
//					std::deque<vertex> &vrtxStack,
//					std::unordered_map<vertex, bool> &reached,
//					std::unordered_map<vertex, vertex> &parent,
//					std::unordered_map<vertex, size_t> &depth,
//					std::unordered_map<vertex, size_t> &lowpt) const {
//		if (reached[u]){ // this is not supposed to happen
//			std::cerr << "Error in recursion" << std::endl;
//			return;
//		}
//		reached[u] = true;
//		vrtxStack.push_back(u);
//		size_t childCount = 0;
//		// else:
//		for (const auto &v : this->outAdjList[u]){
//			bool isCut = false;
//			if (reached[v]){
//				if ( v != parent[u] )
//					lowpt[u] = min(lowpt[u], depth[v]);
//			} else {
//				++childCount;
//				lowpt[u] = min(lowpt[u], lowpt[v]);
//				parent[v] = u;
//				depth[v] = depth[u] + 1;
//				lowpt[v] = depth[v];
//				dfs_biconn(v, blocks, vrtxStack, reached, parent, depth, lowpt);
//				if (lowpt[v] >= depth[u])
//					isCut = true;
//			}
//			if (parent[u] == u && childCount >= 2){
//				isCut = true;
//			if (isCut == true){
//				container<vertex> newBlock;
//				MSG("Discovered a cut vertex");
//				DBG1(u);
//				DBG1D(vrtxStack);
//				//while (childCount > 0){
//					vertex x = vrtxStack.back();
//					newBlock.insert(x);
//					while (x != u){
//						vrtxStack.pop_back();
//						x = vrtxStack.back();
//						newBlock.insert(x);
//						// if (uv.first == u || edgeStack.empty()) break;
//					}
//					DBG1D(newBlock);
//					DBG1D(vrtxStack);
//					blocks.push_back(newBlock);
//					//--childCount;
//				//}
//			}
//		} // for each neighbor v of u
//		}
//	} // finished dfs
//
//	/* Run dfs on this graph to get its biconnected components.
//	 * Store the sets of edges of each component into the vector comps
//	 * return the number of biconnected components
//	 */
//	size_t find_bi_components( std::vector < container < vertex > > &blocks) const{
//		blocks.clear();
//		std::unordered_map <vertex, size_t> depth;
//		std::unordered_map <vertex, size_t> lowpt;
//		std::unordered_map <vertex, vertex> parent;
//		std::unordered_map <vertex, bool> reached;
//		std::unordered_map <vertex, bool> isArticulation;
//		std::deque <vertex> vrtxStack;
//		for (const auto &u : this->vertices){
//			reached[u] = isArticulation[u] = false;
//			depth[u] = lowpt[u] = SIZE_MAX;
//		}
//
//		vertex u = *(this->vertices.begin());
//		this->dfs_biconn(u, blocks, vrtxStack, reached, parent, depth, lowpt);
//		MSG("Finished Recursion");
//		DBG1D(vrtxStack);
//		return (blocks.size());
//	} // find bi-components


	void add_edge(const vertex& u, const vertex &v, const double w = 0){
		if (! isIn(u, this->vertices)) this->vertices.insert(u);
		if (! isIn(v, this->vertices)) this->vertices.insert(v);
		this->inAdjList[v].insert(u);
		this->outAdjList[u].insert(v);
		edge e(u, v);
		weightMap::value_type ew(e, w);
		this->edges.insert(e);
		this->weight.insert(ew);
		//this->n = this->vertices.size();
	}

	void add_edge(const edge &e, const double w = 0){
		vertex u = e.first;
		vertex v = e.second;
		this->add_edge(u, v, w);
	}

	void add_vertex(const vertex &v) {
		if (!isIn(v, this->vertices)) {
			this->vertices.insert(v);
			vertexSet vs;
			try {
				this->inAdjList[v].clear();
				this->outAdjList[v].clear();
			}
			catch (const std::exception &ex) {
				cerr << "Creating new adjacency lists for " << v << endl;
				this->inAdjList.insert(typename adjList::value_type(v, vs));
				this->outAdjList.insert(typename adjList::value_type(v, vs));
			}
		}
		//else continue;
	}
	void add_undirected_edge(const vertex &u, const vertex &v, const double w = 0){
		this->add_edge(u, v, w);
		this->add_edge(v, u, w);
	}

	void add_undirected_edge(const edge &e, const double w = 0){
		vertex u = e.first;
		vertex v = e.second;
		this->add_undirected_edge(u, v, w);
	}

	/* return the set of edges that are NOT in G */
	size_t eBar(container<edge> &eBarSet) const{ //!
		eBarSet.clear();
		size_t ret = 0;
		for (const auto &u: this->vertices){
			for (const auto &v: this->vertices){
				if (u != v){
					edge ee(u, v);
					if (! isIn(ee, this->edges)){
						eBarSet.insert(ee);
						ret++;
					}
				}
			}
		}
		return ret;
	}

	/* Get all the edges on an (arbitrary) s,t path in this,
	 * given s and t
	 */
	size_t getPathEdges(const vertex &s, const vertex &t, container <edge> &thePath) const{
		size_t length = 0;
		thePath.clear();

		if (! (isIn(s, this->vertices) && isIn(t, this->vertices))) return 0;
		//else:
		unordered_map<vertex, bool> visited;
		typedef unordered_map<vertex, bool>::value_type visType;
		unordered_map<vertex, vertex> preds; //predecessor
		typedef unordered_map<vertex, vertex>::value_type predType;
		for (const auto &u: this->vertices){
			visited.insert(visType(u, false));
			preds.insert(predType(u, u));
		}

		deque<vertex> qq;
		qq.push_back(s);
		while (! qq.empty()){
			vertex v = qq.back();
			qq.pop_back();
			if (visited[v]) continue;
			visited[v] = true;
			for (const auto &u: this->outNeighbors(v)){//!
				if (visited[u]) continue;
				//else
				preds[u] = v;
				qq.push_back(u);
			}
		} // while qq is not empty

		// backtracking
		vertex skip = t;
		while (preds[skip]!= skip){
			edge ee(preds[skip], skip);
			thePath.insert(ee);
			skip = preds[skip];
			++length;
		}
		return length;
	}

	/* read a graph from an open input stream pointer *is
	 * for every edge written in the file pointed by is,
	 * two opposite edges are added to this
	 */
	void read_undirected_noweights(std::istream* is){
		size_t n, m; //# verts, edges, respectivelly
		string estrmn = "\n";
		getline(*is, estrmn);
		// DBG1(estrmn);
		stringstream sstrmn(estrmn);
		sstrmn >> n >> m;

		//DBG1(n);
		//DBG1(m);

		this->vertices.clear();
		this->edges.clear();
		this->weight.clear();
		this->initAdj();
		container<edge> eSet;
		for (size_t i = 0; i < m; ++i){
			string estr;
			vertex u, v;
			std::getline(*is, estr);
			stringstream sstr(estr);
			sstr >> u >> v;
			edge e(u, v);
			edge ee(v, u);
			this->add_edge(e);
			this->add_edge(ee);
		}

	}

	/* read a graph from an open input stream pointer *is
	* for every edge written in the file pointed by is,
	* two opposite edges are added to this
	*/
	bool read_nauty_g(std::istream* is) {
		size_t n, m; //# verts, edges, respectively
		string estrmn = "";
		while (estrmn.find("Graph") == std::string::npos) {
			getline(*is, estrmn);
			if (is->eof()) return false;// to stop the code when we have 
										//DBG1(estrmn);
		}
		// At this moment, estrmn should look like
		// Graph ####, Order ###
		int idBegin = estrmn.find("Graph") + 6;
		int idLen = estrmn.find(",") - idBegin;
		this->graphID = estrmn.substr(idBegin, idLen);
		
		//MSG("Graph #");
		//DBG1(estrmn);
		getline(*is, estrmn);
		//DBG1(estrmn);
		stringstream sstrmn(estrmn);
		sstrmn >> n >> m;

		//DBG1(n);
		//DBG1(m);

		this->vertices.clear();
		this->edges.clear();
		this->weight.clear();
		for (size_t v = 0; v < n; ++v) {
			this->vertices.insert(v);
		}
		this->initAdj();
		container<edge> eSet;
		//string estr;
		//std::getline(*is, estr);
		size_t edge_counter = 0;
		while (estrmn != "") {//not empty
			std::getline(*is, estrmn);// take a new line
									  //DBG1D(estrmn);

			stringstream sstr(estrmn);
			vertex u, v;
			while (sstr >> u >> v)
			{
				++edge_counter;
				edge e(u, v);
				edge ee(v, u);
				this->add_edge(e);
				this->add_edge(ee);
			}
			/*for (size_t i = 0; i < m; ++i) {
			vertex u, v;
			sstr >> u >> v;
			edge e(u, v);
			edge ee(v, u);
			this->add_edge(e);
			this->add_edge(ee);
			}*/
			//std::getline(*is, estrmn);
			//DBG1D(estrmn);
		}
		if (edge_counter != m) {// edges are not read correctly
			cout << "Error: Edges are not read correctly" << endl;
			throw(-1);
		}
		estrmn = "";
		//DBG1D(estr);
		//std::getline(*is, estr);
		//return 0;
		return true;

	}

	/* Get all vertices in this with outdegree = 1
	 *
	 */
	void get_leaves(container<vertex> &leafSet) const {//!
		leafSet.clear();
		for (const auto &v : this->vertices){
			const container<vertex> *adj = &(this->outNeighbors(v)); //!
			if (1 == adj->size()) leafSet.insert(v);
		}
		//done
	}

	size_t outDeg(const vertex &v) const {
		auto ref = this->outAdjList.find(v);
		if (ref != this->outAdjList.end()){
			return ref->second.size();
		} 
		else {
			cerr << "Vertex " << v 
				<< " not registered in outAdjacency list!" << endl;
		}
		return SIZE_MAX;
	}



	size_t inDeg(const vertex &v) const {
		auto ref = this->inAdjList.find(v);
		if (ref != this->inAdjList.end()){
			return ref->second.size();
		} 
		else {
			cerr << "Vertex " << v 
				<< " not registered in inAdjacency list!" << endl;
		}
		return SIZE_MAX;
	}

	const vertexSet& outNeighbors (const vertex &v) const{
		auto ref = this->outAdjList.find(v);
		
		if (ref == this->outAdjList.end())
			cerr << "Vertex " << v 
				 << " not registered in outAdjacency list!" << endl;
		return ref->second;
		
	}

	const vertexSet& inNeighbors (const vertex &v) const{
		auto ref = this->inAdjList.find(v);
		if (ref == this->inAdjList.end())
			cerr << "Vertex " << v 
				 << " not registered in inAdjacency list!" << endl;
		return ref->second;
	}

	/* renumber the vertices to [0, n-1]
	 * return a map giving the old number of each vertex
	 */
	unordered_map<vertex, vertex> normalize(Graph<container> &G) const{
		vertex id = 0;
		unordered_map<vertex, vertex> idMap;
		G = Graph<container>(this->getN());
		for (const auto &v : this->vertices) {//on 12-1-2018
			idMap[v] = id;
			++id;
		}
		for (const auto &uv : this->edges){
			vertex u = uv.first;
			vertex v = uv.second;
			edge uvp(idMap[u], idMap[v]);
			G.add_edge(uvp);
		}
		for (const auto & vc : this->colorMap){ // copy colors of vertices
			vertex v = vc.first;
			G.colorMap[idMap[v]] = vc.second; 
		}
		unordered_map <vertex, vertex> ret;
		for (const auto &pp: idMap){
			ret[pp.second] = pp.first;
		}
		return ret;
	}

	void remove_edge(const edge &_uv){
		//	MSG("Removing edge");
		//	DBG1(_uv);
		vertex _u = _uv.first;
		vertex _v = _uv.second;
		this->edges.erase(_uv);
		this->outAdjList[_u].erase(_v);
		this->inAdjList[_v].erase(_u);
	}

	void remove_edge(const vertex &__u, const vertex &__v){
		edge __uv (__u, __v);
		this->remove_edge(__uv);
	}


	/*
	 * removes a vertex v and all its incident edges from this graph,
	 * but adds an edge pq for all (pv, vq)
	 */
	void split_vertex(const vertex &v) {
		if (!isIn(v, this->vertices)) return;
		// else:
		this->vertices.erase(v);
		container<vertex> inn(this->inNeighbors(v));
		container<vertex> outn(this->outNeighbors(v));
		for (const auto &uin : inn) {
			if (uin == v) continue;
			// else
			for (const auto &uout : outn) {
				if (uout == v) continue;
				// else
				if (uin != uout) {
					//	MSG("Adding edge");
					//	edge ade(uin, uout);
					//	DBG1(ade);
					this->add_edge(uin, uout);
					// this->outAdjList[uin].insert(uout);
					// this->inAdjList[uout].insert(uin);
				}
			}
		}

		for (const auto &u : outn)
			this->remove_edge(v, u);
		for (const auto &u : inn)
			this->remove_edge(u, v);


		this->inAdjList.erase(v);
		this->outAdjList.erase(v);
	}

	size_t getColor(const vertex &v) const {
		const auto pp = this->colorMap.find(v);
		if (pp != this->colorMap.end()) {
			return pp->second;
		}
		MSG("Key not found in colorMap:");
		DBG1(v);
		throw ("Value not found");
	}

	void pretty_print(ostream *ofs = &(std::cout), size_t count = 0) const{
		size_t nn = this->getN();
		vector<edge> ve;
		for (const auto &e : this -> edges){
			if (e.first < e.second) ve.push_back(e);
		}
		size_t mm = ve.size();
		//DBG1(mm);
		if (count > 1){
			*ofs << endl;
		}
		*ofs << "Graph " << count
					<< ", Order " << nn << "." << endl;
		*ofs << nn << " " << mm << endl;
		if (mm >=1 ){
			for (size_t i = 0; i < mm-1; ++i)
				*ofs << ve[i].first << " " << ve[i].second << "  ";
			*ofs << ve[mm-1].first << " " << ve[mm-1].second;
		}
		*ofs << endl;
//		vector<vertex> vv(ALL(this->vertices));
//		sort(ALL(vv));
//		for (const auto &v : vv){
//			vector<vertex> nn(ALL(this->outNeighbors(v)));
//			sort(ALL(nn));
//			*ofs << v << " :";
//			for (const auto &u : nn)
//				*ofs << " " << u;
//			*ofs << ";" << std::endl;
//		}
	}

	void pretty_print_1(ostream *ofs = &(std::cout)) const {
		size_t nn = this->getN();
		vector<edge> ve;
		for (const auto &e : this->edges) {
			if (e.first < e.second) ve.push_back(e);
		}
		size_t mm = ve.size();
		//DBG1(mm);
		*ofs << nn << " " << mm << endl;
		if (mm >= 1) {
			for (size_t i = 0; i < mm - 1; ++i)
				*ofs << ve[i].first << " " << ve[i].second << "  ";
			*ofs << ve[mm - 1].first << " " << ve[mm - 1].second;
		}
		*ofs << endl;
	}

	///12/10/2018//
	/* prepare a string (without q)
	* that describes this graph to dreadnaut
	*/
	string for_dreadnaut(bool quit = true, const string &outfile = "\0") const {
		stringstream ssa;
		//DBG1D(ssa.str());
		ssa << "c n = " << this->getN() << " g ";
		for (vertex v = 0; v < this->getN(); v++) {
			for (const auto &u : this->outNeighbors(v)) {
				ssa << " " << u;
			}
			if ((v + 1) < this->getN()) ssa << " ;";
			else ssa << ". ";
		}
		ssa << "f = [";
		size_t maxCol = 0;
		for (const auto &cvp : this->colorMap) {
			size_t curColor = cvp.second;
			maxCol = max(maxCol, curColor);
		}

		for (size_t curCol = 0; curCol <= maxCol; ++curCol) {
			for (const auto &v : this->vertices) {
				if (this->colorMap.at(v) == curCol)
					ssa << " " << v;
			}
			if (curCol < maxCol)
				ssa << " |";
		}
		ssa << " ]";
		ssa << " x";
		if (outfile != "\0")
			ssa << " >> " << outfile;
		ssa << " z b & ->";
		if (quit == true) {
			ssa << "q";
		}
		ssa << endl;

		string ans = ssa.str();
		return ans;
	}
	const double &get_weight(const edge &ee)const {
		return this->weight.at(ee);
	}
	template <template<class...> class C>
	friend std::ostream& operator <<(std::ostream &, const Graph<C> &);
};

template <template<class...> class container>
std::ostream& operator <<(std::ostream &os, const Graph<container> &G){
	for (auto &v: G.vertices){
		os << v << ": ";
		for (const auto &u: G.outNeighbors(v)){
			edge e(v, u);
			os << u << " (" << G.get_weight(e) << "), ";
		}
		os << std::endl;
	}
	return os;
}



#endif /* GRAPH_H_ */
