#!/usr/bin/python
import sys

infile = sys.argv[1]
trips = open("trips_extracted.csv", "w")
direction = open("direction_extracted.csv", "w")
dMap = {}
with open(infile) as lines:
	for line in lines:
		a = line.split(",")
		trip = "%s\t%s\t%s\n" %(a[0], a[2], a[4])
		trips.write(trip)
		
		dName = "%s\t%s" %(a[0], a[3])
		dID = a[4]
		if not dMap.has_key(dName):
			dMap[dName] = dID
			direction.write("%s\t%s\n" %(dName, dID))
trips.close()
direction.close()
