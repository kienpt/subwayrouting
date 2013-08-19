#!/usr/bin/python
#Choose the longest trip for each train (+ direction)
import sys
import copy
from sets import Set
import datetime
import time

infile = "fullschedule.csv"
nodes = Set([])
edges = {}
stop2train = {}
pre_node = ""
pre_t_sec = ""
pre_train = ""
pre_trip = ""
count = 0
with open(infile) as lines:
	for line in lines:
		count += 1
		if count == 1:
			continue
		a = line.strip("\n").split("\t")#1: trip_id; 2:arrival_time; 3:stop_id; 4:route_id; 5:direction_id
		cur_train = a[3]
		cur_trip = a[0]
		#convert time:
		#note that the converting function does not accept this format: 24:MM:SS so there is a trick here 
		hour = int(a[1].split(":")[0]) #MTA data has something wrong: hour could get values: 24, 25, 26, 27
		if hour > 23:
			s_hour = "0" + str(hour - 24)
			t = s_hour + ":" + a[1].split(":", 1)[1]
			cur_t = time.strptime(t, '%H:%M:%S')
	                cur_t_sec = datetime.timedelta(hours=cur_t.tm_hour,minutes=cur_t.tm_min,seconds=cur_t.tm_sec).total_seconds()
			cur_t_sec += 86400
		else:
			cur_t = time.strptime(a[1], '%H:%M:%S')
        	        cur_t_sec = datetime.timedelta(hours=cur_t.tm_hour,minutes=cur_t.tm_min,seconds=cur_t.tm_sec).total_seconds()

		cur_node = a[2] + "_" + cur_train
		nodes.add(cur_node)
		if cur_trip == pre_trip:
			cur_node = a[2] + "_" + cur_train
			edge = pre_node + "\t" + cur_node
			if not edges.has_key(edge):
				edges[edge] = cur_t_sec - pre_t_sec
				if (cur_t_sec - pre_t_sec) < 0:
					print line
		pre_node = cur_node
		pre_t_sec = cur_t_sec
		pre_train = cur_train
		pre_trip = cur_trip
		#Gather trip_train_direction set that cross the same stop
		_stop = a[2][:-1] #parent stop, without direction N, S
		if stop2train.has_key(_stop):
			stop2train[_stop].add(cur_train)
		else:
			trains = Set([cur_train])
			stop2train[_stop] = trains

for stop in stop2train.keys():
        if len(stop2train[stop]) > 1:
                for train1 in stop2train[stop]:
                        for train2 in stop2train[stop]:
                                if train1 != train2:
                                        node11 = stop + "N_" + train1
					node12 = stop + "S_" + train1
                                        node21 = stop + "N_" + train2
					node22 = stop + "S_" + train2
                                        edge1 = node11 + "\t" + node21
					edge2 = node11 + "\t" + node22
					edge3 = node12 + "\t" + node21 
					edge4 = node12 + "\t" + node22
                                        edges[edge1] = "900"
					edges[edge2] = "900"
					edges[edge3] = "900"
					edges[edge4] = "900"
                                        #It is directed graph then we keep edges in both direction

#Write nodes
nodes_f = open("nodes.txt", "w")
for node in nodes:
	nodes_f.write(node + "\n");

#Write edges:
edges_f = open("edges.txt", "w")
for key in edges.keys():
	edges_f.write(key + "\t" + str(int(edges[key])) +  "\n")	

nodes_f.close()
edges_f.close()
