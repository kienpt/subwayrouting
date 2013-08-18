#check N~0 & S~1
with open("fullschedule.csv") as lines:
	for line in lines:
		a = line.strip("\n").split("\t")
		stop = a[2]
		direct = a[4]
		x = stop[len(stop)-1]
		if stop == "R36N":
			print a[3]	
