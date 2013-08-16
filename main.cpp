#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;
typedef std::pair<int, int> Edge;

void loadNodes(char* f, char** nodes)
{
//load all nodes from file
  std::ifstream  in(f);
  char* line; 
  int i = 0;
  while(std::getline(in,line))
  {
    //std::cout<<line<<std::endl;
    //nodes[i] = const_cast<char*>(line.c_str());
    
    i++;
  }
  std::cout<<"Done loading nodes"<<std::endl;
  std::cout<<nodes[1]<<std::endl;
}

void loadEdges()
{
//load all edges from file
}

bool construct_graph(char* node_file, int &num_nodes, char** names, Edge* edge_array, int* weights)
{
  std::cout<<"Test";
  /*char* _names[] = {"A", "B", "C"};
  for(int i=0; i<=3; i++)
  {
    names[i] = _names[i];
  }*/
  
  loadNodes(node_file, names);
  std::cout<<names[1]<<std::endl;
  num_nodes = 3;
  
  Edge _edge_array[] = { Edge(1, 2), Edge(2, 3), Edge(3, 1) };
  for(int i=0; i<=3; i++)  
    edge_array[i] = _edge_array[i];
  
  int _weights[] = {1, 2, 3};
  for(int i=0; i<=3; i++) 
    weights[i] = _weights[i];
   
}

int main(int argc, char **argv)
{
  typedef adjacency_list < listS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
  typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
  typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;


/*
  const int num_nodes = 5;
  enum nodes { A, B, C, D, E };
  char name[] = "ABCDE";
  Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
    Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B) };
  int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
*/


  int num_nodes;
  char** names = new char *[3];
  Edge* edge_array = new Edge[3];
  int* weights = new int[3];
  construct_graph(argv[1], num_nodes, names, edge_array, weights);
  std::cout<<names[0]<<std::endl;
  int num_arcs = sizeof(edge_array) / sizeof(Edge);

  graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
  property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
  std::vector<vertex_descriptor> p(num_vertices(g));
  std::vector<int> d(num_vertices(g));
  vertex_descriptor s = vertex(1, g);


  dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

  std::cout << "distances and parents:" << std::endl;
  graph_traits < graph_t >::vertex_iterator vi, vend;
  for (tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    std::cout << "distance(" << names[*vi] << ") = " << d[*vi] << ", ";
    std::cout << "parent(" << names[*vi] << ") = " << names[p[*vi]] << std::
      endl;
  }
  std::cout << std::endl;

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
    dot_file << names[u] << " -> " << names[v]
      << "[label=\"" << get(weightmap, e) << "\"";
    if (p[v] == u)
      dot_file << ", color=\"black\"";
    else
      dot_file << ", color=\"grey\"";
    dot_file << "]";
  }
  dot_file << "}";
  return EXIT_SUCCESS;
}
