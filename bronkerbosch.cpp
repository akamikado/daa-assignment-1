#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <chrono> 
#include <iomanip>

using namespace std;
class Graph {
private:
    vector<unordered_set<int> > adj;
    int n;

public:
    Graph(int vertices) : n(vertices) {
        adj.resize(n);
    }

    void addEdge(int u, int v) {
        adj[u].insert(v);
        adj[v].insert(u); 
    }

    const unordered_set<int>& getNeighbors(int v) const {
        return adj[v];
    }

    int getVertexCount() const {
        return n;
    }

    vector<int> degeneracyOrdering() const {
        vector<int> order;
        order.reserve(n);
        
        vector<unordered_set<int> > bins(n + 1);  
        vector<int> degree(n);
        int maxDegree = 0;
        
        for (int v = 0; v < n; ++v) {
            degree[v] = adj[v].size();
            bins[degree[v]].insert(v);
            maxDegree = max(maxDegree, degree[v]);
        }
        
        for (int i = 0; i < n; ++i) {
            int d = 0;
            while (d <= maxDegree && bins[d].empty()) {
                d++;
            }
            
            int v = *bins[d].begin();
            bins[d].erase(v);
            
            order.push_back(v);
            
            for (int u : adj[v]) {
                if (degree[u] > degree[v]) {  
                    bins[degree[u]].erase(u);
                    --degree[u];
                    bins[degree[u]].insert(u);
                }
            }
        }
        

        reverse(order.begin(), order.end());
        return order;
    }
};

class BronKerbosch {
private:
    const Graph& G;
    vector<vector<int> > maximalCliques;
    map<int, int> cliqueSizeCount;

    void BronKerboschPivot(vector<int>& R, vector<int>& P, vector<int>& X) {
        if (P.empty() && X.empty()) {
            maximalCliques.push_back(R);
            cliqueSizeCount[R.size()]++;
            return;
        }


        int pivot = -1;
        size_t maxNeighbors = 0;


        for (int u : P) {
            size_t neighborCount = 0;
            const auto& neighbors = G.getNeighbors(u);
            
            for (int v : P) {
                if (neighbors.find(v) != neighbors.end()) {
                    neighborCount++;
                }
            }
            
            if (neighborCount > maxNeighbors) {
                maxNeighbors = neighborCount;
                pivot = u;
            }
        }

        for (int u : X) {
            size_t neighborCount = 0;
            const auto& neighbors = G.getNeighbors(u);
            
            for (int v : P) {
                if (neighbors.find(v) != neighbors.end()) {
                    neighborCount++;
                }
            }
            
            if (neighborCount > maxNeighbors) {
                maxNeighbors = neighborCount;
                pivot = u;
            }
        }


        vector<int> P_minus_neighbors_of_pivot;
        
        for (int v : P) {
            if (pivot == -1 || G.getNeighbors(pivot).find(v) == G.getNeighbors(pivot).end()) {
                P_minus_neighbors_of_pivot.push_back(v);
            }
        }

        for (int v : P_minus_neighbors_of_pivot) {
            const auto& v_neighbors = G.getNeighbors(v);
            

            R.push_back(v);


            vector<int> P_new, X_new;
            
            for (int u : P) {
                if (v_neighbors.find(u) != v_neighbors.end()) {
                    P_new.push_back(u);
                }
            }
            
            for (int u : X) {
                if (v_neighbors.find(u) != v_neighbors.end()) {
                    X_new.push_back(u);
                }
            }

            BronKerboschPivot(R, P_new, X_new);


            R.pop_back();
            

            P.erase(find(P.begin(), P.end(), v));
            X.push_back(v);
        }
    }

public:
    BronKerbosch(const Graph& graph) : G(graph) {}

    static bool compareBySize(const vector<int>& a, const vector<int>& b) {
    return a.size() < b.size();
    }

    void findMaximalCliques() {
        vector<int> degeneracyOrder = G.degeneracyOrdering();
        

        vector<int> vertexPosition(G.getVertexCount());
        for (int i = 0; i < G.getVertexCount(); ++i) {
            vertexPosition[degeneracyOrder[i]] = i;
        }
        
        for (int i = 0; i < G.getVertexCount(); ++i) {
            int v = degeneracyOrder[i];
            

            vector<int> R;
            R.push_back(v);
            vector<int> P, X;
            

            for (int u : G.getNeighbors(v)) {
                if (vertexPosition[u] > i) {

                    P.push_back(u);
                } else if (vertexPosition[u] < i) {

                    X.push_back(u);
                }
            }
            
            BronKerboschPivot(R, P, X);
        }
    }

    void printResults() const {
        cout << "Total maximal cliques: " << maximalCliques.size() << endl;
        
        int largestCliqueSize = 0;
        if(!maximalCliques.empty()) {
    largestCliqueSize = max_element(
        maximalCliques.begin(), maximalCliques.end(),
        compareBySize
    )->size();
}
        
        cout << "Size of the largest maximal clique: " << largestCliqueSize << endl;
        
        cout << "Clique size distribution:" << endl;
        for (const auto& entry : cliqueSizeCount) {
            cout << "Size " << entry.first << ": " << entry.second << endl;
        }
    }


    void saveResultsToCSV(const string& filename) const {
        ofstream outFile(filename);
        outFile << "Size,Count\n";
        for (const auto& entry : cliqueSizeCount) {
            outFile << entry.first << "," << entry.second << "\n";
        }
        outFile.close();
        cout << "Results saved to " << filename << endl;
    }
};


Graph readGraphFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }
    
    string line;
    unordered_map<int, int> vertexMap;
    vector<pair<int, int> > edges;
    int vertexCount = 0;

    while (getline(inFile, line)) {
        if (line[0] == '#') continue; 
        
        istringstream iss(line);
        
        int u, v;
        if (!(iss >> u >> v)) continue;


        if (vertexMap.find(u) == vertexMap.end()) {
            vertexMap[u] = vertexCount++;
        }
        if (vertexMap.find(v) == vertexMap.end()) {
            vertexMap[v] = vertexCount++;
        }

        edges.push_back(make_pair(vertexMap[u], vertexMap[v]));
    }

    Graph G(vertexCount);
    for (const auto& edge : edges) {
        G.addEdge(edge.first, edge.second);
    }

    return G;
}

int main() {
    string filename = "wiki-Vote.txt";
    
    cout << "Reading graph from " << filename << "..." << endl;
    Graph G = readGraphFromFile(filename);
    
    cout << "Graph loaded with " << G.getVertexCount() << " vertices." << endl;
    
    BronKerbosch bk(G);
    
    cout << "Finding maximal cliques..." << endl;
    auto start = chrono::high_resolution_clock::now();
    bk.findMaximalCliques();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    bk.printResults();
    cout << "\nExecution time: " << fixed << setprecision(3) << elapsed.count() << " seconds" << endl;
    
    bk.saveResultsToCSV("histogram_wiki-vote.csv");
    
    return 0;
}
