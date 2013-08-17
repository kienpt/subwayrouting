#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;
using namespace std;
typedef std::pair<int, int> Edge;

void loadNodes(char* f, std::vector<string> &nodes, std::map<std::string, int> &node2int)
{
//load all nodes from file
  std::ifstream  in(f);
  string line; 
  int idx = 0;
  while(std::getline(in,line))
  {
    nodes.push_back(line);   
    node2int[line] = idx;
    idx = idx + 1;
  }
  std::cout<<"Done loading nodes"<<std::endl;
}

void loadEdges(char*f, vector<Edge> &edges, vector<int> &weights, std::map<std::string, int> &node2int)
{
//load all edges from file
  std::ifstream  in(f);
  string line;

  string node1;
  string node2;
  string weight;
  while(std::getline(in,line))
  {
    std::stringstream  lineStream(line);
    std::getline(lineStream, node1, '\t');
    std::getline(lineStream, node2, '\t');
    std::getline(lineStream, weight, '\t');
    Edge edge(node2int[node1], node2int[node2]);
    edges.push_back(edge);
    weights.push_back(atoi(weight.c_str()));
  }
  std::cout<<"Done loading edges"<<std::endl;
}

int main(int argc, char **argv)
{
  typedef adjacency_list < listS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
  typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
  typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;


/*
  const int num_nodes = 5;
  enum enum_nodes { A, B, C, D, E };
  char nodes[] = "ABCDE";
  Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
    Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B) };
  int weight_array[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
  int num_arcs = sizeof(edge_array) / sizeof(Edge);
*/


  vector<string> nodes;
  vector<Edge> _edges;
  vector<int> weights;
  std::map<std::string, int> node2int;
  loadNodes(argv[1], nodes, node2int);
  loadEdges(argv[2], _edges, weights, node2int);

  const int num_nodes = nodes.size();
  Edge edge_array [_edges.size()];
  int weight_array [_edges.size()];
  for(int i=0; i<_edges.size(); i++)
  {
      edge_array[i] = _edges[i];
      weight_array[i] = weights[i];
  } 
  int num_arcs = sizeof(edge_array) / sizeof(Edge);

  graph_t g(edge_array, edge_array + num_arcs, weight_array, num_nodes);
  property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
  std::vector<vertex_descriptor> p(num_vertices(g));
  std::vector<int> d(num_vertices(g));
  
  int src = node2int[std::string(argv[3])];
  int des = node2int[std::string(argv[4])];
  cout<<"Source: "<<nodes[src]<<endl;
  cout<<"Destination: "<<nodes[des]<<endl;
  vertex_descriptor s = vertex(src, g);

  cout<<"Number of vertices: "<<num_vertices(g)<<endl;
  
  dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

  std::cout << "distances and parents:" << std::endl;
  graph_traits < graph_t >::vertex_iterator vi, vend;
  for (tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    if (*vi == des)
    {
      std::cout << "distance(" << nodes[*vi] << ") = " << d[*vi] << ", ";
      std::cout << "parent(" << nodes[*vi] << ") = " << nodes[p[*vi]] << std::endl;
    }
  }
  std::cout << std::endl;
/*
  std::ofstream dot_file("figs/dijkstra-eg.dot");

  dot_file << "digraph D {\n"
    << "  rankdir=LR\n"
    << "  size=\"4,3\"\n"
    << "  ratio=\"fill\"\n"
    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

  graph_traits < graph_t >::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    graph_traits < graph_t >::edge_descriptor e = *ei;
    graph_traits < graph_t >::vertex_descriptor
      u = source(e, g), v = target(e, g);
    dot_file << nodes[u] << " -> " << nodes[v]
      << "[label=\"" << get(weightmap, e) << "\"";
    if (p[v] == u)
      dot_file << ", color=\"black\"";
    else
      dot_file << ", color=\"grey\"";
    dot_file << "]";
  }
  dot_file << "}";*/
  return EXIT_SUCCESS;
}
