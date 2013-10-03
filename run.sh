python extract_trips.py google_transit/trips.txt
python extract_time.py google_transit/stop_times.txt 
python extract_stop.py google_transit/stops.txt 
python aggregate.py 
./construct_graph.sh
python add_transfer.py google_transit/transfers.txt nodes.txt 
./generate_stop2times.sh
