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
#define STOP_2_TIMES_FILE "stop2times.txt"
#define STOPS_FILE "stops.txt"

using namespace boost;
using namespace std;

typedef std::pair<int, int> Edge;
typedef adjacency_list < listS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
struct Address
{
	float _lat;
	float _lng;
	std::string _address;
};

struct Stop
{
	std::string _stopID;
	int _time;

	Stop(std::string stopID):_stopID(stopID){};
};
typedef std::vector<Stop> Route;

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

int searchTime(std::vector<int> times, int key)
{//In times, find the closest and greater number to key
//Because size of times is small, then using brute force is good enough
//It is noted that numbers in times are sorted
	for(int i = 0; i < times.size(); i++)
	{
		if (times[i] > key)
			return times[i];
	}
	return times[0];
}

void loadStop(std::string f, map<std::string, Address> &stop2addr)
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

			Address addr;
			addr._address = address;
			addr._lat = atof(slat.c_str());
			addr._lng = atof(slng.c_str());
			stop2addr[stopID] = addr;
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

void loadEdges(std::string f, vector<Edge> &edges, vector<int> &weights, std::map<std::string, int> &node2int, std::map<Edge, int> &edge2weight)
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
		edge2weight[edge] = nWeight;
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

void loadStop2Times(std::string f, std::map<std::string, std::vector<int> > &stop2times)
{
	std::ifstream in(f.c_str());
        string line;
        string stop;
        string time;
        while(std::getline(in, line))
        {
                std::stringstream lineStream(line);
                std::getline(lineStream, stop, '\t');
                std::vector<int> times;
                while(std::getline(lineStream, time, '\t'))
                        times.push_back(atoi(time.c_str()));
                stop2times[stop] = times;
        }

}

void find_near_station(float sLat, float sLng, float gLat, float gLng, map<std::string, Address> stop2addr, map<std::string, int> &starts, map<std::string, int> &goals)
{
	for(map<std::string, Address>::const_iterator it=stop2addr.begin(); it!=stop2addr.end(); it++)
	{
		float start_dist = distance(sLat, sLng, it->second._lat, it->second._lng);
		if (start_dist<1.5) //it takes roundly 20 mins to walk 1.5 * sqrt(2) km
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

void dijkstra(const graph_t &g, 
		int nStart, int nGoal, 
		const vector<string> &nodes, 
		const std::map<std::string, int> &node2int, 
		const std::map<Edge, int> &edge2weight,
		const std::map<std::string, std::vector<int> > &stop2time,
		int startTime = 32400)//32400 == 9:00:00
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
	Route route;
	std::vector<int> weights;
	route.push_back(Stop(nodes[nGoal]));
	weights.push_back(0);
	while(curNode != nStart)
        {
                curNode = p[curNode];
		Stop stop(nodes[curNode]); 
		route.push_back(stop);
                Edge e(curNode, preNode);
		weights.push_back(edge2weight.at(e));
                std::string edge = nodes[curNode] + "--" + nodes[preNode];
                path = nodes[curNode] + ": " + edge + ": " + boost::lexical_cast<std::string>(edge2weight.at(e)) + "\n" + path;
                preNode = curNode;
        }
	route.back()._time = searchTime(stop2time[], startTime + weights.back());
	std::cout<<route.back()._stopID<<"--"<<route.back()._time<<endl;
	for(int i=route.size()-2; i>=0; i--)
	{
		route[i]._time = route[i+1]._time + weights[i+1];
		std::cout<<route[i]._stopID<<"--"<<route[i]._time<<endl;
	}
	
	std::cout<<"Path: "<<endl<<path<<endl;
}

void addEdges(int nStart,
		int nGoal,
		const map<std::string, int> &starts, 
		const map<std::string, int> &goals, 
		vector<Edge> &_edges, 
		vector<int> &weights, 
		std::map<std::string, std::vector<std::string> > stop2train,
		std::map<std::string, int> node2int,
		std::map<Edge, int> &edge2weight)
{//Add additional edges that link to start and goal nodes
	std::string tempNode;
	Edge e;

	for(map<std::string, int>::const_iterator it=starts.begin(); it!=starts.end(); it++)
	{
		vector<std::string> s_trains = stop2train[it->first];//List of trains that stop at sStart station
		for(int i=0; i < s_trains.size(); i++)
		{
			tempNode = it->first + "N_" + s_trains[i];
			e = Edge(nStart, node2int[tempNode]);
			_edges.push_back(e);
			weights.push_back(it->second);
			edge2weight[e] = it->second;

			tempNode = it->first + "S_" + s_trains[i];
			e = Edge(nStart, node2int[tempNode]);
			_edges.push_back(e);
			weights.push_back(it->second);
			edge2weight[e] = it->second;
		}
	}

	for(map<std::string, int>::const_iterator it=goals.begin(); it!=goals.end(); it++)
	{
		vector<std::string> g_trains = stop2train[it->first];//List of trains that stop at goals[x] station
		for(int i=0; i < g_trains.size(); i++)
		{
			tempNode = it->first + "N_" + g_trains[i];
			e = Edge(node2int[tempNode], nGoal);
			_edges.push_back(e);
			weights.push_back(it->second);
			edge2weight[e] = it->second;

			tempNode = it->first + "S_" + g_trains[i];
			e = Edge(node2int[tempNode], nGoal);
			_edges.push_back(e);
			weights.push_back(it->second);
			edge2weight[e] = it->second;
		}
	}
}

int main(int argc, char **argv)
{
	vector<string> nodes;
	vector<Edge> _edges;//Boost has an object name edges so I have to use _edges :(
	vector<int> weights;
	std::map<std::string, int> node2int;
	std::map<std::string, std::vector<std::string> > stop2train; //Mapping from stop to list of trains
	std::map<std::string, std::vector<int> > stop2time; //Mapping stop_train to time when the train stops
	std::map<std::string, Address> stop2addr;//Mapping stop id to Address structure
	std::map<Edge, int> edge2weight; //Edge to weight
	
	//Loading data from files including nodes, edges, mapping from stop to trains, location of stops
	loadNodes(NODES_FILE, nodes, node2int);
	loadEdges(EDGES_FILE, _edges, weights, node2int, edge2weight);
	loadStop2Trains(STOP_2_TRAINS_FILE, stop2train);
	loadStop2Times(STOP_2_TIMES_FILE, stop2time);
	loadStop(STOPS_FILE, stop2addr);
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
	find_near_station(sLat, sLng, gLat, gLng, stop2addr, starts, goals);//Find near-by station

	addEdges(nStart, nGoal, starts, goals, _edges, weights, stop2train, node2int, edge2weight);
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

	dijkstra(g, nStart, nGoal, nodes, node2int, edge2weight, stop2time);
	return EXIT_SUCCESS;
}
