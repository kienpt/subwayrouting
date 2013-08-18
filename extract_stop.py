#!/usr/bin/python
import sys

infile = sys.argv[1]
stops = open("stop_extracted.csv", "w")
with open(infile) as lines:
	for line in lines:
		a = line.split(",")
		stop = "%s\t%s\t%s\t%s\n" %(a[0], a[2], a[4], a[5])
		stops.write(stop)
stops.close()
