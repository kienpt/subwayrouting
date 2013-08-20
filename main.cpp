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
	int idx = 0;
	while(std::getline(in,line))
	{
		nodes.push_back(line);   
		node2int[line] = idx;
		idx = idx + 1;
	}
}

void loadEdges(std::string f, vector<Edge> &edges, vector<int> &weights, std::map<std::string, int> &node2int)
{
	//load all edges from file
	std::ifstream  in(f.c_str());
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

void mk_starts_goals(float sLat, float sLng, float gLat, float gLng, map<std::string, Stop> mStop, map<std::string, int> &starts, map<std::string, int> &goals)
{
	cout<<sLat<<endl;
	cout<<sLng<<endl;
	cout<<gLat<<endl;
	cout<<gLng<<endl;
	for(map<std::string, Stop>::const_iterator it=mStop.begin(); it!=mStop.end(); it++)
	{
		float start_dist = distance(sLat, sLng, it->second._lat, it->second._lng);
		if (start_dist < 1.5) //it takes roundly 20 mins to walk 1.5 * sqrt(2) km
			starts[it->first] = (int) (start_dist/5*3600);
		else
		{
			float goal_dist = distance(gLat, gLng, it->second._lat, it->second._lng);
			if (goal_dist < 1.5){
				goals[it->first] = (int) (goal_dist/5*3600);
		}
	}
}

void dijkstra(graph_t g, map<std::string, int> starts, map<std::string, int> goals, vector<string> nodes, std::map<std::string, int> node2int, std::map<std::string, std::vector<std::string> > stop2trains)
{
	map<std::string, int> start_list;
	map<std::string, int> goal_list;
	for(map<std::string, int>::const_iterator it=starts.begin(); it!=starts.end(); it++)
	{
		vector<std::string> s_trains = stop2trains[it->first];//List of trains that stop at sStart station
		for(int i=0; i < s_trains.size(); i++)  
		{
			start_list[it->first + "N_" + s_trains[i]] = it->second;
			start_list[it->first + "S_" + s_trains[i]] = it->second;
		}
	}

	for(map<std::string, int>::const_iterator it=goals.begin(); it!=goals.end(); it++)
	{
		vector<std::string> g_trains = stop2trains[it->first];//List of trains that stop at goals[x] station
		for(int i=0; i < g_trains.size(); i++)  
		{
			goal_list[it->first + "N_" + g_trains[i]] = it->second;;
			goal_list[it->first + "S_" + g_trains[i]] = it->second;
		}
	}

	int minDistance = 999999;
	std::vector<vertex_descriptor> minp;
	int minStart;//Station index
	int minGoal;//Station index
	
	for(map<std::string, int>::const_iterator s_it=start_list.begin(); s_it!=start_list.end(); s_it++)
		for(map<std::string, int>::const_iterator g_it=goal_list.begin(); g_it!=goal_list.end(); g_it++)
		{
			if(g_it->second < 0)
				cout<<"Goal:  "<<g_it->second<<endl;
			if(s_it->second < 0)
				cout<<"Start:  "<<s_it->second<<endl;
			int nStart = node2int[s_it->first];
			int nGoal = node2int[g_it->first];
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
					int distance = d[*vi];
					if (d[*vi] < minDistance)
					{
						int distance = d[*vi] + s_it->second + g_it->second;
						if (distance < minDistance)
						{
							minDistance = distance;
							minp = p;
							minStart = nStart;
							minGoal = nGoal;
						}
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
	vector<string> nodes;
	vector<Edge> _edges;
	vector<int> weights;
	std::map<std::string, int> node2int;
	std::map<std::string, std::vector<std::string> > s2t; //Mapping from stop to list of trains
	std::map<std::string, Stop> mStop;//Mapping stop id to it's properties

	loadNodes(NODES_FILE, nodes, node2int);
	loadEdges(EDGES_FILE, _edges, weights, node2int);
	loadStop2Trains(STOP_2_TRAINS_FILE, s2t);
	loadStop(STOPS_FILE, mStop);
	cout<<"Done loading data..."<<endl;
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
	map<std::string, int> starts;//map the station with walk time to starting location
	map<std::string, int> goals;
	float sLat = atof(argv[1]);//Latitude of starting point
	float sLng = atof(argv[2]);//Longitude of starting point
	float gLat = atof(argv[3]);//Latitude of goal point
	float gLng = atof(argv[4]);//Longitude of goal point
	mk_starts_goals(sLat, sLng, gLat, gLng, mStop, starts, goals);
	dijkstra(g, starts, goals, nodes, node2int, s2t);
	return EXIT_SUCCESS;
}
