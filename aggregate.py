import sys

trips = "trips_extracted.csv"
times = "time_extracted.csv"
out = open("fullschedule.csv", "w")
tripMap = {}

with open(trips) as lines:
	for line in lines:
		a = line.strip("\n").split("\t")
		tripMap[a[1]] = "%s\t%s" %(a[0], a[2])

with open(times) as lines:
	for line in lines:
		line = line.strip("\n") 
		a = line.split("\t")
		tripID = a[0]
		trip = tripMap[tripID]
		fulltrip = line + "\t" + trip + "\n"
		out.write(fulltrip)
out.close()
