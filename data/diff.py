import sys
from sets import Set

f1 = sys.argv[1]
f2 = sys.argv[2]

node1 = Set([])
node2 = Set([])
with open(f1) as lines:
	for line in lines:
		node2.add(line.strip("\n"))

with open(f2) as lines:
	for line in lines:		
		if line.strip("\n") not in node2:
			print line.strip("\n")
		
