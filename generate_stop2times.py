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

infile = "fullschedule.csv"
stop2times_f = open("stop2times.txt", "w")
stop2times = {}
count = 0
with open(infile) as lines:
	for line in lines:
		count += 1
		if count == 1:
			continue
		ar = line.split("\t")	
		time = ar[1]
		stop = ar[2]
		train = ar[3]
		st = stop + "_" + train
		if stop2times.has_key(st):
			stop2times[st].add(time2int(time))
		else:
			stop2times[st] = Set([time2int(time)])

for key in stop2times.keys():
	s = sorted(stop2times[key])
	times = ""
	for time in s:
#		times += int2time(time) + "\t"
		times += str(time) + "\t"
	stop2times_f.write(key + "\t" + times.strip("\t") + "\n")

stop2times_f.close()
