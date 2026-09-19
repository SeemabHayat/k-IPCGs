#ifndef _DEBUG_H
#define _DEBUG_H

#ifndef DEBUG
#define DEBUG true
#endif

#define DBG1(x) if(DEBUG) std::cerr << #x << " = " << x << std::endl
#define DBG1d(x, y) if(DEBUG) std::cerr << #x << "[" << y << "] = " << x[y] << std::endl
#define DBG2d(x, y, z) if(DEBUG) std::cerr << #x << "[" << y << "]" << "[" << z << "] = " << x[y][z] << std::endl
#define DBG1D(x)  if (DEBUG){ cerr << #x << endl; for (const auto it: x) cerr << it <<", ";} cerr << std::endl
#define DBG2D(x) cerr << #x << endl; if (DEBUG) for (const auto it: x){ for (const auto  itt: it) cerr << itt << ", "; cerr << std::endl;} cerr << std::endl
#define MSG(msg) if(DEBUG) std::cerr << msg << std::endl

#define ECHO(x) std::cout << #x << " = " << x << std::endl
#define ECHO1d(x, y) std::cout << #x << "[" << y << "] = " << x[y] << std::endl
#define ECHO2d(x, y, z) if(DEBUG) std::cout << #x << "[" << y << "]" << "[" << z << "] = " << x[y][z] << std::endl
#define ECHO1D(x) cout << #x << endl; for (const auto it: x) cout << it <<", "; cout << std::endl

#define DBGCon(x) if (DEBUG){ std::cerr<< #x << std::endl; for (auto &y:x) cerr << y << std::endl; }

namespace std{
	template <class T1, class T2>
	ostream& operator << (ostream& os, const pair<T1, T2> &p){
		os << "(" << p.first << "," << p.second << ")";
		return os;
	}
}
#endif
