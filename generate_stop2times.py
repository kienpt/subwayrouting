import sys
from sets import Set

def time2int(time):
	hms = time.split(":")
	s = 3600*int(hms[0]) + 60*int(hms[1]) + int(hms[2])
	return s

def int2time(s):
	h = s/3600
	ms = s%3600
	m = ms/60
	s = ms%60
	hms = str(h) + ":" + str(m) + ":" + str(s)
	return hms

schedule_file = sys.argv[1] #input
stop2times_file = sys.argv[2] #ouput
stop2times_f = open(stop2times_file, "w") #ouput
stop2times = {}
count = 0
with open(schedule_file) as lines:
	for line in lines:
		count += 1
		if count == 1:
			continue
		ar = line.split("\t")	
		time = ar[1]
		stop = ar[2]
		train = ar[3]
		tripID = ar[0]
		time_tripID = (time2int(time), tripID)
		st = stop + "_" + train
		if stop2times.has_key(st):
			stop2times[st].add(time_tripID)
		else:
			stop2times[st] = Set([time_tripID])

for key in stop2times.keys():
	s = sorted(stop2times[key], key=lambda trip: trip[0])
	time_tripIDs = ""
	for time_tripID in s:
#		times += int2time(time) + "\t"
		time_tripIDs += str(time_tripID[0]) + "\t" + time_tripID[1] + "\t"
	stop2times_f.write(key + "\t" + time_tripIDs.strip("\t") + "\n")

stop2times_f.close()
