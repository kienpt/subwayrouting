#!/usr/bin/python
import sys

infile = sys.argv[1]
times_f = open("time_extracted.csv", "w")

with open(infile) as lines:
	for line in lines:
		a = line.split(",")
		trip_id = a[0]
		time = a[1]
		stop = a[3]
		times_f.write("%s\t%s\t%s\n" %(trip_id, time, stop))
		
times.close()
