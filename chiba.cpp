#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>

int n;                              // Number of vertices
std::vector<std::vector<bool>> adj; // Adjacency matrix
std::vector<int> degree;            // Degree of each vertex
std::vector<int> sortedToOriginal;  // Mapping from sorted to original vertices

std::vector<int> T;
std::vector<int> S;
long long TcliqueCount;
long long cliqueCount[100];

// Function to read text file and map nodes to 1-based indexing
std::unordered_map<int, int> readFileIntoHashMap(const std::string &filename)
{
    std::unordered_map<int, int> nodeMap;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Failed to open the file." << std::endl;
        return nodeMap;
    }

    // Skip the first two numbers (n and m)
    int n_dummy, m_dummy;
    file >> n_dummy >> m_dummy;

    int nodeIndex = 1;
    int u, v;
    while (file >> u >> v)
    {
        // Check if u is not already in the map before inserting
        if (nodeMap.find(u) == nodeMap.end())
        {
            nodeMap[u] = nodeIndex++;
        }
        // Check if v is not already in the map before inserting
        if (nodeMap.find(v) == nodeMap.end())
        {
            nodeMap[v] = nodeIndex++;
        }
    }
    file.close();
    return nodeMap;
}

void UPDATE(int i, std::set<int> &C, int k)
{
    if (i > n)
    {
        if (C.size() == k)
        {
            cliqueCount[C.size()]++;
            TcliqueCount++;
        }
        return;
    }

    // Step 1: Check if C is subset of N(i)
    bool is_subset = true;
    for (int v : C)
    {
        if (!adj[i][v])
        {
            is_subset = false;
            break;
        }
    }
    if (is_subset)
    {
        std::set<int> newC = C;
        newC.insert(i);
        UPDATE(i + 1, newC, k);
        return;
    }

    std::set<int> CNi;
    for (int v : C)
    {
        if (adj[i][v])
            CNi.insert(v);
    }

    // Step 2: Compute T[y]
    std::fill(T.begin(), T.end(), 0);
    for (int x : CNi)
    {
        for (int y = 1; y <= n; y++)
        {
            if (y != i && C.find(y) == C.end() && adj[x][y])
                T[y]++;
        }
    }

    // Step 3: Compute S[y]
    std::fill(S.begin(), S.end(), 0);
    std::vector<int> C_minus_Ni;
    for (int v : C)
    {
        if (!adj[i][v])
        {
            C_minus_Ni.push_back(v);
            for (int y = 1; y <= n; y++)
            {
                if (C.find(y) == C.end() && adj[v][y])
                    S[y]++;
            }
        }
    }

    // Step 4: Maximality test
    bool FLAG = true;
    for (int y = 1; y < i; y++)
    {
        if (C.find(y) == C.end() && adj[i][y] && T[y] == CNi.size())
        {
            FLAG = false;
            break;
        }
    }

    // Step 5: Sort C-N(i)
    std::sort(C_minus_Ni.begin(), C_minus_Ni.end());
    const int p = C_minus_Ni.size();

    // Step 6: Lex test for S(y) > 0
    if (FLAG)
    {
        for (size_t k = 0; k < C_minus_Ni.size(); k++)
        {
            int jk = C_minus_Ni[k];
            for (int y = 1; y < i; y++)
            {
                if (adj[jk][y] && C.find(y) == C.end() && T[y] == CNi.size())
                {
                    if (y > jk)
                    {
                        S[y]--;
                    }
                    else
                    {
                        size_t j_idx = std::lower_bound(C_minus_Ni.begin(), C_minus_Ni.end(), y) - C_minus_Ni.begin();
                        if (j_idx < C_minus_Ni.size())
                        {
                            const int j_prev = j_idx > 0 ? C_minus_Ni[j_idx - 1] : 0;
                            if ((S[y] + k) < p && (j_prev == 0 || y > j_prev))
                            {
                                FLAG = false;
                                break;
                            }
                        }
                    }
                }
            }
            if (!FLAG)
                break;
        }
    }

    // Step 7: Lex test for S(y) = 0
    if (FLAG && !CNi.empty())
    {
        const int jp = C_minus_Ni.empty() ? 0 : C_minus_Ni.back();
        for (int y = 1; y < i; y++)
        {
            if (C.find(y) == C.end() && T[y] == CNi.size() && S[y] == 0)
            {
                if (jp < y || jp < i - 1)
                {
                    FLAG = false;
                    break;
                }
            }
        }
    }

    // Recursive exploration
    if (FLAG)
    {
        std::set<int> newC = CNi;
        newC.insert(i);
        UPDATE(i + 1, newC, k);
    }

    // Explore without including i
    UPDATE(i + 1, C, k);
}

void sortVertices()
{
    std::vector<std::pair<int, int>> vertices;
    for (int i = 1; i <= n; i++)
    {
        vertices.emplace_back(i, degree[i]);
    }

    std::sort(vertices.begin(), vertices.end(),
              [](const auto &a, const auto &b)
              { return a.second < b.second; });

    for (int i = 1; i <= n; i++)
    {
        sortedToOriginal[i] = vertices[i - 1].first;
    }

    std::vector<std::vector<bool>> newAdj(n + 1, std::vector<bool>(n + 1, false));
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            newAdj[i][j] = adj[sortedToOriginal[i]][sortedToOriginal[j]];
        }
    }

    adj = std::move(newAdj);
}

void findCliques()
{
    sortVertices();
    for (int k = 3; k <= n; ++k)
    {
        std::set<int> C;
        TcliqueCount = 0;
        UPDATE(1, C, k);
        if (TcliqueCount == 0)
            break;
        std::cout << "Number of maximal " << k << "-cliques: " << TcliqueCount << std::endl;
        cliqueCount[k] = TcliqueCount;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    // Create node mapping
    std::unordered_map<int, int> nodeMap = readFileIntoHashMap(argv[1]);

    // Reopen the file to read edges
    std::ifstream file(argv[1]);
    int m;
    file >> n >> m;

    // n is now the number of unique nodes in the graph
    n = nodeMap.size();

    adj.resize(n + 1, std::vector<bool>(n + 1, false));
    degree.resize(n + 1, 0);
    sortedToOriginal.resize(n + 1);
    T.resize(n + 1);
    S.resize(n + 1);

    int u, v;
    for (int i = 0; i < m; i++)
    {
        file >> u >> v;
        adj[nodeMap[u]][nodeMap[v]] = true;
        adj[nodeMap[v]][nodeMap[u]] = true;
        degree[nodeMap[u]]++;
        degree[nodeMap[v]]++;
    }

    findCliques();

    return 0;
}