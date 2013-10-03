python extract_trips.py google_transit/trips.txt
python extract_time.py google_transit/stop_times.txt 
python extract_stop.py google_transit/stops.txt 
python aggregate.py 
./construct_graph.sh
python add_transfer.py google_transit/transfers.txt data/nodes-wkd.txt data/edges-wkd.txt
python add_transfer.py google_transit/transfers.txt data/nodes-sat.txt data/edge-sat.txt
python add_transfer.py google_transit/transfers.txt data/nodes-sun.txt data/edge-sun.txt
./generate_stop2times.sh
