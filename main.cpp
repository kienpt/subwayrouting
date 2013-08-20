#define _USE_MATH_DEFINES
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;
using namespace std;

typedef std::pair<int, int> Edge;
typedef adjacency_list < listS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
struct Stop
{
	float _lat;
	float _lng;
	std::string _address;
};

float distance(float lat1, float lng1, float lat2, float lng2)
{//Compute distance between two points in GPS coordinate system
	float dlng = (lng2 - lng1)*M_PI/180;
	float dlat  = (lat2 - lat1)*M_PI/180;
	lat1 = lat1*M_PI/180;
	lat2 = lat2*M_PI/180;
	// Haversine formula:
	float R = 6371;
	float a = sin(dlat/2)*sin(dlat/2) + cos(lat1)*cos(lat2)*sin(dlng/2)*sin(dlng/2);
	float c = 2 * atan2( sqrt(a), sqrt(1-a) );
	float dist = R * c;
	std::cout<<dist<<endl;
	return dist;
}

int walkTime(float lat1, float lng1, float lat2, float lng2)
{
	float dist = distance(lat1, lng1, lat2, lng2);
	return (int)dist/5*(3600);//prefered walking speed is 5km/h
}

void loadStop(char* f, map<std::string, Stop> &mStop)
{
	std::ifstream  in(f);
        std::string line;
	std::string address;
	std::string stopID;
	std::string temp;
	std::string slat, slng;
	int count = 0;
	std::getline(in, line);//header
	while(std::getline(in,line))
	{
		if (count%3 == 0)
		{
			std::stringstream  lineStream(line);
			std::getline(lineStream, stopID, ',');
			std::getline(lineStream, temp, ',');
			std::getline(lineStream, address, ',');
			std::getline(lineStream, temp, ',');
			std::getline(lineStream, slat, ',');
			std::getline(lineStream, slng, ',');
			
			Stop aStop;
			aStop._address = address;
			aStop._lat = atof(slat.c_str());
			aStop._lng = atof(slng.c_str());
			mStop[stopID] = aStop;
		}
		count++;
	}
}

void loadNodes(char* f, std::vector<string> &nodes, std::map<std::string, int> &node2int)
{
	//load all nodes from file
	std::ifstream  in(f);
	std::string line; 
	int idx = 0;
	while(std::getline(in,line))
	{
		nodes.push_back(line);   
		node2int[line] = idx;
		idx = idx + 1;
	}
}

void loadEdges(char*f, vector<Edge> &edges, vector<int> &weights, std::map<std::string, int> &node2int)
{
	//load all edges from file
	std::ifstream  in(f);
	string line;

	string node1;
	string node2;
	string weight;
	while(std::getline(in, line))
	{
		std::stringstream  lineStream(line);
		std::getline(lineStream, node1, '\t');
		std::getline(lineStream, node2, '\t');
		std::getline(lineStream, weight, '\t');
		Edge edge(node2int[node1], node2int[node2]);
		edges.push_back(edge);
		weights.push_back(atoi(weight.c_str()));
	}
}

void loadStop2Trains(char *f, std::map<std::string, std::vector<std::string> > &stop2trains)
{
	std::ifstream in(f);
	string line;
	string stop;
	string train;
	while(std::getline(in, line))
	{
		std::stringstream lineStream(line);
		std::getline(lineStream, stop, '\t');
		std::vector<string> trains;
		while(std::getline(lineStream, train, '\t'))
			trains.push_back(train);
		stop2trains[stop] = trains;
	}
}

void dijkstra(graph_t g, std::string sStart, std::string sGoal, vector<string> nodes, std::map<std::string, int> node2int, std::map<std::string, std::vector<std::string> > stop2trains)
{
	vector<std::string> s_trains = stop2trains[sStart];//List of trains that stop at sStart station
	vector<std::string> g_trains = stop2trains[sGoal];//List of trains that stop at sGoal station
	vector<std::string> start_list;
	vector<std::string> goal_list;

	for(int i=0; i < s_trains.size(); i++)  
	{
		start_list.push_back(sStart + "N_" + s_trains[i]);
		start_list.push_back(sStart + "S_" + s_trains[i]);
	}

	for(int i=0; i < g_trains.size(); i++)  
	{
		goal_list.push_back(sGoal + "N_" + g_trains[i]);
		goal_list.push_back(sGoal + "S_" + g_trains[i]);
	}

	int minDistance = 999999;
	std::vector<vertex_descriptor> minp;
	//std::vector<int> mind;
	int minStart;//Station index
	int minGoal;//Station index
	for(int i=0; i<start_list.size(); i++)
		for(int j=0; j<goal_list.size(); j++)
		{
			int nStart = node2int[start_list[i]];
			int nGoal = node2int[goal_list[j]];
			vertex_descriptor start= vertex(nStart, g);
			std::vector<vertex_descriptor> p(num_vertices(g));
			std::vector<int> d(num_vertices(g));

			dijkstra_shortest_paths(g, start, predecessor_map(&p[0]).distance_map(&d[0]));

			//Show Path
			graph_traits < graph_t >::vertex_iterator vi, vend;
			for (tie(vi, vend) = vertices(g); vi != vend; ++vi) 
			{
				if (*vi == nGoal)
				{
					if (d[*vi] < minDistance)
					{
						minDistance = d[*vi];
						minp = p;
						//mind = d;	
						minStart = nStart;
						minGoal = nGoal;
					}		
				}
			}	
		}
	if (minDistance < 999999)
	{
		cout<<"Start: "<<nodes[minStart];
		cout<<", Destination: "<<nodes[minGoal];
		//std::cout << ", Distance: "<< mind[minGoal] <<endl;
		std::cout << ", Time: "<< minDistance <<" seconds"<<endl;
		std::string path;
		std::cout << "Path: \n"<< std::endl;
		while(minGoal != minStart)
		{
			path = nodes[minGoal] + "\n" + path;
			minGoal = minp[minGoal];
		}
		path = nodes[minGoal] + "\n" + path;
		std::cout<<path<<endl;
	}
}

int main(int argc, char **argv)
{
	//Test
	distance(40.714111, -74.008585, 40.680438, -73.950426);
	//Test
	vector<string> nodes;
	vector<Edge> _edges;
	vector<int> weights;
	std::map<std::string, int> node2int;
	std::map<std::string, std::vector<std::string> > s2t; //Mapping from stop to list of trains
	std::map<std::string, Stop> mStop;//Mapping stop id to it's properties

	loadNodes(argv[1], nodes, node2int);
	loadEdges(argv[2], _edges, weights, node2int);
	loadStop2Trains(argv[3], s2t);
	loadStop(argv[4], mStop);

	//Test
	cout<< mStop.size()<<endl;
	
	//Test

	//Initialize graphs's properties
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
	dijkstra(g, std::string(argv[5]), std::string(argv[6]), nodes, node2int, s2t);
	return EXIT_SUCCESS;
}
