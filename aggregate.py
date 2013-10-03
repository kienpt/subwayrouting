import sys
#Join two tables based on tripID
#Divide joined table wrt Weekday (WKD), Saturday (SAT) and Sunday (SUN)

trips = "trips_extracted.csv"
times = "time_extracted.csv"
out = {}
out["WKD"] = open("fullschedule-wkd.csv", "w")
out["SAT"] =  open("fullschedule-sat.csv", "w")
out["SUN"] = open("fullschedule-sun.csv", "w")

tripMap = {}

with open(trips) as lines:
	for line in lines:
		a = line.strip("\n").split("\t")
		tripMap[a[1]] = "%s\t%s" %(a[0], a[2])

count = 0
with open(times) as lines:
	for line in lines:
		count += 1
		if count == 1:
			out["WKD"].write("trip_id\tarrival_time\tstop_id\troute_id\tdirection_id\n")
			out["SAT"].write("trip_id\tarrival_time\tstop_id\troute_id\tdirection_id\n")
			out["SUN"].write("trip_id\tarrival_time\tstop_id\troute_id\tdirection_id\n")
			continue
		line = line.strip("\n") 
		a = line.split("\t")
		tripID = a[0]
		day_type = tripID.split("_")[0][-3:]
		trip = tripMap[tripID]
		fulltrip = line + "\t" + trip + "\n"
		out[day_type].write(fulltrip)
out["WKD"].close()
out["SAT"].close()
out["SUN"].close()
