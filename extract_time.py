#!/usr/bin/python
import sys

infile = sys.argv[1]
times = open("time_extracted.csv", "w")

with open(infile) as lines:
	for line in lines:
		a = line.split(",")
		time = "%s\t%s\t%s\n" %(a[0], a[1], a[3])
		times.write(time)
times.close()
