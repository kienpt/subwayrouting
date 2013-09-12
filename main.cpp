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
#include <boost/lexical_cast.hpp>

#define EDGES_FILE "edges.txt"
#define NODES_FILE "nodes.txt"
#define STOP_2_TRAINS_FILE "stop2train.csv"
#define STOPS_FILE "stops.txt"

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
	return dist;
}

void loadStop(std::string f, map<std::string, Stop> &mStop)
{
	std::ifstream  in(f.c_str());
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

void loadNodes(std::string f, std::vector<string> &nodes, std::map<std::string, int> &node2int)
{
	//load all nodes from file
	std::ifstream  in(f.c_str());
	std::string line; 
	int idx = 1;
	nodes.push_back("NA");//There is no node with index 0
	while(std::getline(in,line))
	{
		nodes.push_back(line);   
		node2int[line] = idx;
		idx = idx + 1;
	}
}

void loadEdges(std::string f, vector<Edge> &edges, vector<int> &weights, std::map<std::string, int> &node2int, std::map<Edge, int> &e2w)
{
	//load all edges from file
	std::ifstream  in(f.c_str());
	string line;

	string node1;
	string node2;
	string sWeight;
	int nWeight;
	while(std::getline(in, line))
	{
		std::stringstream  lineStream(line);
		std::getline(lineStream, node1, '\t');
		std::getline(lineStream, node2, '\t');
		std::getline(lineStream, sWeight, '\t');
		Edge edge(node2int[node1], node2int[node2]);
		edges.push_back(edge);
		nWeight = atoi(sWeight.c_str());
		weights.push_back(nWeight);
		e2w[edge] = nWeight;
	}
}

void loadStop2Trains(std::string f, std::map<std::string, std::vector<std::string> > &stop2trains)
{
	std::ifstream in(f.c_str());
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

void find_near_station(float sLat, float sLng, float gLat, float gLng, map<std::string, Stop> mStop, map<std::string, int> &starts, map<std::string, int> &goals)
{
	for(map<std::string, Stop>::const_iterator it=mStop.begin(); it!=mStop.end(); it++)
	{
		float start_dist = distance(sLat, sLng, it->second._lat, it->second._lng);
		if (start_dist < 1.5) //it takes roundly 20 mins to walk 1.5 * sqrt(2) km
		{
			starts[it->first] = (int) (start_dist/5*3600);
		}
		else
		{
			float goal_dist = distance(gLat, gLng, it->second._lat, it->second._lng);
			if (goal_dist < 1.5)
				goals[it->first] = (int) (goal_dist/5*3600);
		}
	}
}

void dijkstra(graph_t g, int nStart, int nGoal, vector<string> nodes, std::map<std::string, int> node2int, std::map<Edge, int> e2w)
{
	vertex_descriptor start= vertex(nStart, g);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	dijkstra_shortest_paths(g, start, predecessor_map(&p[0]).distance_map(&d[0]));
	//Show Path
	std::cout << "Time: "<< d[nGoal]/60 <<" minutes"<<endl;
	std::string path = nodes[nGoal];
	int preNode = nGoal;
	int curNode = nGoal;
	/*
	for(std::map<Edge, int>::iterator it = e2w.begin(); it != e2w.end(); it++)
	{
		std::cout<<it->first.first<<", "<<it->first.second<<", "<<it->second<<endl;
	}*/
	while(curNode != nStart)
	{
		curNode = p[curNode];
		Edge e(curNode, preNode);
		std::string edge = nodes[curNode] + "--" + nodes[preNode];
		path = nodes[curNode] + ": " + edge + ": " + boost::lexical_cast<std::string>(e2w[e]) + "\n" + path;
		preNode = curNode;
	}
	std::cout<<"Path: "<<endl<<path<<endl;
}

void addEdges(int nStart,
		int nGoal,
		map<std::string, int> starts, 
		map<std::string, int> goals, 
		vector<Edge> &_edges, 
		vector<int> &weights, 
		std::map<std::string, std::vector<std::string> > stop2trains,
		std::map<std::string, int> node2int,
		std::map<Edge, int> &e2w)
{//Add additional edges that link to start and goal nodes
	std::string tempNode;
	Edge e;

	for(map<std::string, int>::const_iterator it=starts.begin(); it!=starts.end(); it++)
	{
		vector<std::string> s_trains = stop2trains[it->first];//List of trains that stop at sStart station
		for(int i=0; i < s_trains.size(); i++)
		{
			tempNode = it->first + "N_" + s_trains[i];
			e = Edge(nStart, node2int[tempNode]);
			_edges.push_back(e);
			weights.push_back(it->second);
			e2w[e] = it->second;

			tempNode = it->first + "S_" + s_trains[i];
			e = Edge(nStart, node2int[tempNode]);
			_edges.push_back(e);
			weights.push_back(it->second);
			e2w[e] = it->second;
		}
	}

	for(map<std::string, int>::const_iterator it=goals.begin(); it!=goals.end(); it++)
	{
		vector<std::string> g_trains = stop2trains[it->first];//List of trains that stop at goals[x] station
		for(int i=0; i < g_trains.size(); i++)
		{
			tempNode = it->first + "N_" + g_trains[i];
			e = Edge(node2int[tempNode], nGoal);
			_edges.push_back(e);
			weights.push_back(it->second);
			e2w[e] = it->second;

			tempNode = it->first + "S_" + g_trains[i];
			e = Edge(node2int[tempNode], nGoal);
			_edges.push_back(e);
			weights.push_back(it->second);
			e2w[e] = it->second;
		}
	}
}

int main(int argc, char **argv)
{
	vector<string> nodes;
	vector<Edge> _edges;//Boost has an object name edges so I have to use _edges :(
	vector<int> weights;
	std::map<std::string, int> node2int;
	std::map<std::string, std::vector<std::string> > s2t; //Mapping from stop to list of trains
	std::map<std::string, Stop> mStop;//Mapping stop id to Stop structure
	std::map<Edge, int> e2w; //Edge to weight
	
	//Loading data from files including nodes, edges, mapping from stop to trains, location of stops
	loadNodes(NODES_FILE, nodes, node2int);
	loadEdges(EDGES_FILE, _edges, weights, node2int, e2w);
	loadStop2Trains(STOP_2_TRAINS_FILE, s2t);
	loadStop(STOPS_FILE, mStop);
	cout<<"Done loading data."<<endl;

	//Initialize graphs's properties
	//Consider start and goal are two nodes. ID of starting node is "start", ID of goal node is "goal"
	nodes.push_back("start");
	const int nStart = nodes.size()-1;
	node2int["start"] = nStart;

	nodes.push_back("goal");
	const int nGoal = nodes.size()-1;
	node2int["goal"] = nGoal;

	const int num_nodes = nodes.size();
	
	map<std::string, int> starts;//map the nearby station to distance (in time)
	map<std::string, int> goals;
	float sLat = atof(argv[1]);//Latitude of starting point
	float sLng = atof(argv[2]);//Longitude of starting point
	float gLat = atof(argv[3]);//Latitude of goal point
	float gLng = atof(argv[4]);//Longitude of goal point
	find_near_station(sLat, sLng, gLat, gLng, mStop, starts, goals);//Find near-by station

	addEdges(nStart, nGoal, starts, goals, _edges, weights, s2t, node2int, e2w);
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

	dijkstra(g, nStart, nGoal, nodes, node2int, e2w);
	return EXIT_SUCCESS;
}
