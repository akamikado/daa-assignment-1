#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

class SparseGraph {
private:
  std::unordered_map<int, std::vector<int>> adjacency;
  std::vector<int> vertices;

public:
  SparseGraph() {}

  void add_edge(int from, int to) {
    if (!has_vertex(from)) {
      vertices.push_back(from);
    }
    if (!has_vertex(to)) {
      vertices.push_back(to);
    }

    add_to_sorted_vector(adjacency[from], to);
    add_to_sorted_vector(adjacency[to], from);
  }

  bool has_vertex(int node) const {
    return std::binary_search(vertices.begin(), vertices.end(), node);
  }

  void finalize() {
    std::sort(vertices.begin(), vertices.end());
    vertices.erase(std::unique(vertices.begin(), vertices.end()),
                   vertices.end());

    for (auto &[node, neighbors] : adjacency) {
      std::sort(neighbors.begin(), neighbors.end());
      neighbors.erase(std::unique(neighbors.begin(), neighbors.end()),
                      neighbors.end());
    }
  }

  const std::vector<int> &get_vertices() const { return vertices; }

  const std::vector<int> &get_neighbors(int node) const {
    static const std::vector<int> empty_vector;
    auto it = adjacency.find(node);
    if (it != adjacency.end()) {
      return it->second;
    }
    return empty_vector;
  }

  size_t count_common_elements(int node, const std::vector<int> &other) const {
    const auto &neighbors = get_neighbors(node);
    std::vector<int> intersection;
    intersection.reserve(std::min(neighbors.size(), other.size()));

    std::set_intersection(neighbors.begin(), neighbors.end(), other.begin(),
                          other.end(), std::back_inserter(intersection));

    return intersection.size();
  }

  void difference_with_neighbors(const std::vector<int> &set, int node,
                                 std::vector<int> &result) const {
    result.clear();
    result.reserve(set.size());
    const auto &neighbors = get_neighbors(node);

    std::set_difference(set.begin(), set.end(), neighbors.begin(),
                        neighbors.end(), std::back_inserter(result));
  }

  int get_max_node_id() const {
    if (vertices.empty())
      return 0;
    return *std::max_element(vertices.begin(), vertices.end());
  }

private:
  void add_to_sorted_vector(std::vector<int> &vec, int value) {
    auto it = std::lower_bound(vec.begin(), vec.end(), value);
    if (it == vec.end() || *it != value) {
      vec.insert(it, value);
    }
  }
};

size_t max_clique_size = 0;
size_t maximal_clique_count = 0;
std::unordered_map<size_t, size_t> clique_size_distribution;

void expand(const SparseGraph &graph, const std::vector<int> &subg,
            std::vector<int> &cand, std::vector<int> &current_path,
            bool first_call) {
  if (subg.empty()) {
    maximal_clique_count++;
    size_t size = current_path.size();
    clique_size_distribution[size]++;
    max_clique_size = std::max(max_clique_size, size);
    return;
  }

  std::vector<int> subg_q, cand_q;

  int u = -1;
  size_t max_count = 0;

  if (first_call) {
    for (int node : cand) {
      size_t count = graph.get_neighbors(node).size();
      if (count > max_count) {
        max_count = count;
        u = node;
      }
    }
  } else {
    for (int node : subg) {
      size_t count = graph.count_common_elements(node, cand);
      if (count > max_count) {
        max_count = count;
        u = node;
      }
    }
  }

  if (u == -1) {
    return;
  }

  std::vector<int> diff;
  graph.difference_with_neighbors(cand, u, diff);

  while (!diff.empty()) {
    int q = diff.back();
    diff.pop_back();

    current_path.push_back(q);

    const auto &q_neighbors = graph.get_neighbors(q);

    subg_q.clear();
    cand_q.clear();

    std::set_intersection(subg.begin(), subg.end(), q_neighbors.begin(),
                          q_neighbors.end(), std::back_inserter(subg_q));

    std::set_intersection(cand.begin(), cand.end(), q_neighbors.begin(),
                          q_neighbors.end(), std::back_inserter(cand_q));

    expand(graph, subg_q, cand_q, current_path, false);

    current_path.pop_back();

    auto it = std::lower_bound(cand.begin(), cand.end(), q);
    if (it != cand.end() && *it == q) {
      cand.erase(it);
    }
  }
}

void read_dataset(const std::string &dataset, SparseGraph &graph) {
  std::ifstream file(dataset);
  if (!file) {
    std::cerr << "File could not be opened" << std::endl;
    exit(1);
  }

  std::string line;
  while (getline(file, line)) {
    if (line.find("Nodes") != std::string::npos) {
      break;
    }
  }

  while (getline(file, line)) {
    if (line.find("#") == std::string::npos) {
      break;
    }
  }

  int from_node, to_node;
  while (file >> from_node >> to_node) {
    graph.add_edge(from_node, to_node);
  }

  file.close();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <dataset_file>" << std::endl;
    return 1;
  }

  std::string dataset = argv[1];

  SparseGraph graph;
  read_dataset(dataset, graph);
  graph.finalize();

  const std::vector<int> subg = graph.get_vertices();
  std::vector<int> cand = graph.get_vertices();
  std::vector<int> current_path;

  auto start_time = std::chrono::high_resolution_clock::now();
  expand(graph, subg, cand, current_path, true);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - start_time);

  std::vector<std::pair<size_t, size_t>> sorted_distribution;
  for (const auto &pair : clique_size_distribution) {
    sorted_distribution.push_back(pair);
  }

  std::sort(sorted_distribution.begin(), sorted_distribution.end());

  std::string csv_filename = "cli_" + dataset + "_clique_distribution.csv";
  std::ofstream csv_file(csv_filename);

  if (!csv_file) {
    std::cerr << "Could not create CSV file: " << csv_filename << std::endl;
    return 1;
  }

  csv_file << "Clique Size,Count" << std::endl;

  for (const auto &pair : sorted_distribution) {
    csv_file << pair.first << "," << pair.second << std::endl;
  }

  csv_file.close();
  std::cout << "Clique size distribution written to: " << csv_filename
            << std::endl;

  std::string summary_filename = "cli_" + dataset + "_summary.csv";
  std::ofstream summary_file(summary_filename);

  if (!summary_file) {
    std::cerr << "Could not create summary file: " << summary_filename
              << std::endl;
    return 1;
  }

  summary_file << "Statistic,Value" << std::endl;
  summary_file << "Max Clique Size," << max_clique_size << std::endl;
  summary_file << "Total Maximal Cliques," << maximal_clique_count << std::endl;
  summary_file << "Execution Time (s)," << (double)duration.count() / 1000
               << std::endl;

  summary_file.close();
  std::cout << "Summary statistics written to: " << summary_filename
            << std::endl;

  return 0;
}
