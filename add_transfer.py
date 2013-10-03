#!/usr/bin/python
import sys
from sets import Set

transfer_file = sys.argv[1] #input
nodes_file = sys.argv[2] #input
edges_file = sys.argv[3] #output
#Load mapping stop to trains
stop2trains = {}
with open(nodes_file) as lines:
	for line in lines:
		a = line.strip("\n").split("_")
		stop = a[0][:-1]
		train = a[1]
		if stop2trains.has_key(stop):
			stop2trains[stop].add(train)
		else:
			trains = Set([train])
			stop2trains[stop] = trains

#add transfer
edges_file = open(edges_file, "a")
count = 0;
with open(transfer_file) as lines:
	for line in lines:
		count += 1
		if count == 1:
			continue
		a = line.split(",")
		stop1 = a[0]
		stop2 = a[1]
		if stop1 != stop2:
			trains1 = stop2trains[stop1]
			trains2 = stop2trains[stop2]
			for train1 in trains1:
				for train2 in trains2:
					node11 = stop1 + "N_" + train1
                                        node12 = stop1 + "S_" + train1
                                        node21 = stop2 + "N_" + train2
                                        node22 = stop2 + "S_" + train2
                                        edge1 = node11 + "\t" + node21
                                        edge2 = node11 + "\t" + node22
                                        edge3 = node12 + "\t" + node21
                                        edge4 = node12 + "\t" + node22
					edges_file.write(edge1 + "\t609\n")
					edges_file.write(edge2 + "\t609\n")
					edges_file.write(edge3 + "\t609\n")
					edges_file.write(edge4 + "\t609\n")
edges_file.close()
