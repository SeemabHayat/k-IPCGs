/*
 * canon_graph.h
 *
 *  Created on: Aug 24, 2018
 *      Author: shurbevski
 */

#ifndef CANON_GRAPH_H_
#define CANON_GRAPH_H_

#include <string>
#include "graph3.h"
#include "debug.h"
#define _SYS_UCONTEXT_H 1
#include <signal.h>




template <template <class...> class container>
class CanonGraph: public Graph<container>{
public:
	string signature;
	std::unordered_map <vertex, vertex> labels;


	CanonGraph (size_t sz = 0) : Graph<container>(sz){
		// pass
	}

	CanonGraph (const Graph<container> Gp): Graph<container>(Gp), signature(){
		// pass
	}

	bool operator == (const CanonGraph<container> &rhs) const{
		string cmp("Comparing ");
		cmp.append(this->signature);
		cmp.append(string(" and "));
		cmp.append(rhs.signature);
		//MSG(cmp);
		if (this->signature != rhs.signature){
			//if (DEBUG) cerr << "Eqaulity fails due to signature" << endl;
		 	return false;
		}
		if (this->getN() != rhs.getN()){
			//if (DEBUG) cerr << "Equality fails due to #vertices" << endl;
			return false;
		} 
		for (const auto &v : this-> vertices){
			if (!isIn(v, rhs.vertices)){
				//if (DEBUG) cerr << "Equality fails due to vertex " << v
						//<< " not in both vertex sets" << endl;
				return false;
			} 
			bool sameColor;
			try{
				auto colorVthis = this->colorMap.at(v);
				auto colorVrhs = rhs.colorMap.at(v);
				sameColor = ( colorVthis == colorVrhs );
				if (!sameColor){ 
					//if (DEBUG) cerr << "Equality fail due to colorMap" << endl;
					//DBG1(colorVthis);
					//DBG1(colorVrhs);
					return false;
				}
			} catch (const exception &ex) {
				//if (DEBUG) cerr << "Exception here " << "canon_graph, checking coloring" << endl;
				//cout << ex.what() << endl;
				throw (ex);
			}
			// if (this->colorMap.at(v) != rhs.colorMap.at(v)) return false;
			if (this->outDeg(v) != rhs.outDeg(v)){
				//if (DEBUG) cerr << "Equality fails due to different outDegree @" << v << endl;
				return false;
			}
			if (this->inDeg(v) != rhs.inDeg(v)){
				//if (DEBUG) cerr << "Equality fails due to different inDegree @" << v << endl;
				return false;
			}
			for (const auto &u : this->outNeighbors(v))
				if (!isIn(u, rhs.outNeighbors(v))){
					//cout << "Equality fails due to " << u 
						//	<< " not an outNeighbor of " << v << endl;
				 	return false;
				}
			for (const auto &u : this->inNeighbors(v))
				if (!isIn(u, rhs.inNeighbors(v))) {
					//cout << "Equality fails due to " << u 
						//	<< " not an inNeighbor of " << v << endl;
					return false;
				}
		}
		//MSG("They're equal");
		return true;
	}

	/* istream isCanon contains the canonically labelled graph by nauty
	 * istream isColored contains the information for
	 * the colored graph as formatted to input to dreadnaut;
	 * If they both happen to be the same stream,
	 * then the info on the canonical graph will be read first.
	 */
	bool read_canon_graph(istream *isCanon) {
		string CanonLine;
		CanonLine = "\n";
		//DBG1(CanonLine);
		while (CanonLine == "\n") {
			getline(*isCanon, CanonLine);
			//DBG1(CanonLine);
			if (isCanon->eof()) return false;
		} // skip empty lines, perhaps not necessary
		this->signature = CanonLine;
		//DBG1(this->signature);
		// the first non-empty line is the signature
		// the second non-empty line is the labeling
		unordered_map <vertex, vertex> labelMap;
		// labelMap[original_vretex] = canonical_label
		vertex v = 0;
		vertex vp = 0;
		CanonLine = "";
		getline(*isCanon, CanonLine);
		while (CanonLine.find(":") == string::npos) {
			//DBG1(CanonLine);
			stringstream tstream(CanonLine); // we got the labelmap
			//DBG1(tstream);
			DBG1(v);
			while (tstream >> v) {
				//D//BG1D(tstream);
				DBG1(v);
				MSG("while 2");
				labelMap[v] = vp;
				++vp;
				// the label of v is vp, compatible with nauty
				this->vertices.insert(v);
			}
			getline(*isCanon, CanonLine);
		}
		DBG1D(labelMap);
		size_t n = labelMap.size();
		//DBG1(n);
		//DBG1(CanonLine);
		this->initAdj();
		// DBG1(*this);
		for (size_t i = 0; i < n; ++i) {
			//MSG("for 1");
			//string CanonLine1;
			//	#ifdef DEBUG
			//	cerr << CanonLine1 << endl;
			//	#endif
			char c = 0;
			vertex v, u;
			stringstream tstream(CanonLine);
			tstream >> v;
			// DBG1(v);
			while (c != ':') tstream >> c;
			while (tstream >> u) {
				// DBG1(u);
				edge uv(v, u);
				// DBG1(uv);
				this->add_edge(v, u);
				this->add_edge(u, v);
			}


			getline(*isCanon, CanonLine);

		}


		// time to read the colors
		// reminder, colors are like in the input line,
		// we have to use the labelMap to ge the vertex colors of the
		// canonical graph

		string ColoredLine(CanonLine);
		// there is only one line per graph for colored graphs
		//getline(*isCanon, ColoredLine);
		//DBG1(CanonLine);
		if (isCanon->eof()){ // this is not supposed to happen
			MSG ("File ended prematurely!");
			return false;
			throw(-1);
		}
		//DBG1(ColoredLine);
		//	size_t f = ColoredLine.find("f=");
		//	DBG1(f);
		//	if (f == string::npos){
		//		cerr << "Line does not match description!" << endl;
		//		cerr << ColoredLine << endl;
		//	}
		//getline(*isCanon, ColoredLine);
		/*MSG("start color");
		DBG1(CanonLine);
		string colorline;
		colorline = ColoredLine;
		DBG1(colorline);*/
		//CanonLine = "";
		/*getline(*isCanon, colorline);
		DBG1(colorline);*/
		//MSG("Before color while");
		//DBG1(ColoredLine);
		string TotalColoredLine = " ";
		//DBG1(TotalColoredLine);
		TotalColoredLine = ColoredLine;
		while (ColoredLine.find("]") == string::npos) {
			getline(*isCanon, ColoredLine);
			TotalColoredLine = TotalColoredLine + ColoredLine;
			
			//DBG1(ColoredLine);
		}
		//DBG1(TotalColoredLine);
		string substr = TotalColoredLine.substr(2); // +2 offsets "f=["
        //DBG1(substr);
		
		stringstream streamCol(substr);
		char c;
		size_t color = 0;
		do {
			//MSG("do 1");
			c = 0;
			vertex v;
			vertex v_start;
			while (streamCol >> v) {
				vertex vp = labelMap[v];
				this->colorMap[vp] = color;
				//DBG1d(this->colorMap, vp);
				v_start = v;
			}
			streamCol.clear();
			// this doesn't erase the stream, only resets the fail state
			do {
				//MSG("do 2");
				streamCol >> c;
			//	 DBG1(c);
			} while (c == ' ');
			if (c == ']') break;
			//else
			if (c == '|') {
				++color;
				continue; // continues the do-while loop
			}
			if (c == ':') {
				vertex v_end;
				streamCol >> v_end;
				// DBG1(v_start);
				// DBG1(v_end);
				for (vertex vv = v_start + 1; vv <= v_end; ++vv) {
					vertex vp = labelMap[vv];
					//DBG1d(labelMap, vv);
					this->colorMap[vp] = color;
					//DBG1d(this->colorMap, vp);
				}
			}
			else {
				// this is not supposed to happen
				MSG("Unexpected character in input");
				throw (-1);// justing testing
				return false;
			}
		} while (c != ']');
		//DBG1D(this->colorMap);
		return true;
	}

	/*
	 * return the configuration (G, T, sigma, pi)
	 * given by this 5-colored graph
	 */
	void toConf(//size_t n, // the size of |V(G)| and |L(T)|
				Graph<container> &G,
				Graph<container> &T,
				unordered_map<vertex, vertex> &sigma,
				unordered_map<edge, bool> &pi) const {
		// H has 2n-2 + 3 verts, for |V(G)| = |L(T)| = n
		size_t k = this->getN();
		size_t n = (sqrt(8*k + 1) - 3)/2;
		G = Graph< container >(n);
		T = Graph< container >(2*n - 2);
		unordered_map<vertex, vertex> idMapG, idMapT; // mapping vertis from this to G and T
		unordered_map<vertex, vertex> idMapGrev, idMapTrev; // inverse of above
		vertex vID = 0;
		for (const auto &v : this->vertices){
			if (this->outDeg(v) == 2){
				// we have discovered a dummy colored vertex
				vector<vertex> pp(ALL(this->outNeighbors(v)));
				// for (const auto &vp : this->outNeighbors(v)) pp.push_back(vp);
				for (const auto &vp : pp){
					if (idMapG.find(vp) == idMapG.end()){ // cannot use isIn in maps!
						idMapG[vp] = vID;
						idMapT[vp] = vID;
						idMapGrev[vID] = vp;
						idMapTrev[vID] = vp;
						sigma[vID] = vID;
						vID++;
					}
				}
				// DBG1D(idMapG);
				edge uv;
				edge tEdge(idMapG[pp[0]], idMapG[pp[1]]);
				uv = tEdge.undirect();
				DBG1(uv);
				if (this->getColor(v) == 2){ // this means an edge of G
					// DBG1(uv);
					G.add_edge(uv);
					G.add_edge(uv.second, uv.first);
				}
				else{ // this means a non-edge
					pi[uv] = (bool)(this->getColor(v)); // 0-false, 1-true
				}
			}
		}
		for (const auto &v : this->vertices){
			if (this->outDeg(v) == 3){ // we have discovered an inner vertex of T
				MSG("Deg 3");
				DBG1(v);

				if (idMapT.find(v) == idMapT.end()){
					//MSG("New"); DBG1(v);
					idMapT[v] = vID++;
				}
				for (const auto &u : this->outNeighbors(v)){
					if (idMapT.find(u) == idMapT.end()){
						idMapT[u] = vID++;
					}
					edge vu (idMapT[v], idMapT[u]);
					T.add_edge(vu);
				}
				for (const auto &u : this->inNeighbors(v)){
					if (idMapT.find(u) ==idMapT.end())
						idMapT[u] = vID++;
					edge uv (idMapT[u], idMapT[v]);
					T.add_edge(uv);
				}
			}
		}
	}

	/*
	 * relabel the vertices of this graph according to labelMap
	 * CG is the resulting graph
	 */
	void relabel(const unordered_map<vertex, vertex> &labelMap, CanonGraph<container > &CG) const {
		CG = CanonGraph<container>(this->getN());
		for (const auto &eg : this->edges){
			vertex u = eg.first;
			vertex v = eg.second;
			vertex up = labelMap.at(u);
			vertex vp = labelMap.at(v);
			CG.add_edge(up, vp);
		}
		for (const auto &v : this->vertices){
			vertex vp = labelMap[v];
			CG.colorMap[vp] = this->colorMap[v];
		}
	}

	/* prepare a string (without q)
	 * that describes this graph to dreadnaut
	 */
	string for_dreadnaut( bool quit = true, const string &outfile = "\0") const{
		stringstream ssa;
		ssa << "c n = " << this->getN() << " g ";
		for (vertex v = 0; v < this->getN(); v ++){
			for (const auto &u : this->outNeighbors(v)){
				ssa << " " << u;
			}
			if ((v+1) < this->getN()) ssa << " ;";
			else ssa << ". ";
		}
		ssa << "f = [";
		size_t maxCol = 0;
		for (const auto &cvp : this->colorMap){
			size_t curColor = cvp.second;
			maxCol = max(maxCol, curColor);
		}

		for (size_t curCol = 0; curCol <= maxCol; ++curCol){
			for (const auto &v : this->vertices){
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
		if (quit == true){
			ssa << "q";
		}
		ssa << endl;

		string ans = ssa.str();
		return ans;
	}

	/* call dreadnaut to get a canonical version of this graph,
	 * then store the canonical version in CG
	 */
	void get_canonical(CanonGraph<container> &CG, string dPath = "."){
		stringstream drn = this->call_dreadnaut(dPath);
		istream *is(&drn);
		CG.read_canon_graph(is);
	}

	stringstream call_dreadnaut(string dPath = "."){
		string dreadnaut_command = this->for_dreadnaut(true);
		DBG1(dreadnaut_command);
	    int fd_p2c[2], fd_c2p[2], bytes_read;
	    pid_t childpid;
	    char readbuffer[2046];
	    // change the value 2046 if the message from dreadnaut gets too long
	    string program_name = dPath + "/dreadnaut";
	    string receive_output = "";

	    if (pipe(fd_p2c) != 0 || pipe(fd_c2p) != 0)
	    {
	        cerr << "Failed to pipe\n";
	        exit(1);
	    }
	    childpid = fork();
		cout << " childpid= " << childpid << endl;

	    if (childpid < 0)
	    {
			//printf("child process, pid = %u\n", getpid());

			// the argv list first argument should point to   
			// filename associated with file being executed 
			// the array pointer must be terminated by NULL  
			// pointer 
			//char * argv_list[] = { "ls","-lart","/home",NULL };

			// the execv() only return if error occured. 
			// The return value is -1 
			/*execv("ls", argv_list);
			exit(0);*/
	        cout << "Fork failed" << endl;
	        exit(-1);
	    }
	    else if (childpid == 0)
	    {
	        if (dup2(fd_p2c[0], 0) != 0 ||
	            close(fd_p2c[0]) != 0 ||
	            close(fd_p2c[1]) != 0)
	        {
	            cerr << "Child: failed to set up standard input\n";
	            exit(1);
	        }
	        if (dup2(fd_c2p[1], 1) != 1 ||
	            close(fd_c2p[1]) != 0 ||
	            close(fd_c2p[0]) != 0)
	        {
	            cerr << "Child: failed to set up standard output\n";
	            exit(1);
	        }
			int rr;
	        rr = execl(program_name.c_str(), program_name.c_str(), (char *) 0);
			// rr = execv(program_name.c_str(), program_name.c_str());
			MSG("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			DBG1(rr);
			MSG("Are we ever here?");
	        cerr << "Failed to execute " << program_name << endl;
	        exit(1);
	    }
	    else
	    {
	        close(fd_p2c[0]);  // not needed
	        close(fd_c2p[1]);  // not needed
	        MSG("We got communication");

			dreadnaut_command.append("\n");
	        int nbytes = dreadnaut_command.length();
	        if (write(fd_p2c[1], dreadnaut_command.c_str(), nbytes) != nbytes)
	        {
	            cerr << "Parent: short write to child\n";
	            exit(1);
	        }

			bytes_read = read(fd_c2p[0], readbuffer, sizeof(readbuffer)-1);
			DBG1(bytes_read);
			readbuffer[bytes_read] = '\0';
			DBG1(readbuffer);

			string terminate = "\x1a\n";
			nbytes = terminate.length();
			// write(fd_p2c[1], terminate.c_str(), nbytes);

	        close(fd_c2p[0]); // needed to pipe child -> parent
	        close(fd_p2c[1]); // need to pipe parent -> child
			//kill(childpid, SIGTERM);

	        stringstream ss(readbuffer);
	        string line;
			do{
				getline(ss, line);
				//DBG1(line);
				// ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}while (line.find("seconds") == string::npos);

			stringstream drnClean;
			while (getline(ss, line))drnClean << line << std::endl;
	        return drnClean;
	    }
	}

	template <template<class...> class C>
	friend std::ostream& operator <<(std::ostream &, CanonGraph<C> &);
};

template < template < class... > class container >
std::ostream& operator <<(std::ostream &os, CanonGraph<container> &G){
	for (auto &v: G.vertices){
		os << v << ": ";
		for (const auto &u: G.outAdjList[v]){
			edge e(v, u);
			os << u << " (" << G.weight[e] << "), ";
		}
		os << std::endl;
	}
	return os;
}


namespace std{
template <template <class...> class container >
class hash< CanonGraph<container> >{
public:
	size_t operator()(const CanonGraph<container> &g) const{
		std::hash<string> hasher;
		string str(g.signature);
		size_t result = hasher(str);
		return result;
	}
};
}


#endif /* CANON_GRAPH_H_ */
