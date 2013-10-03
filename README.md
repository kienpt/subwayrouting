subwayrouting
=============
* How to construct graph:
 * Download schedule from MTA: http://www.mta.info/developers/data/nyct/subway/google_transit.zip; extract the zip file and put the folder google_transit to project directory
 * Run scripts:
  * python extract_trips.py (input: trips.txt, output: trips_extracted.csv and direction_extracted.csv)
  * python extract_time.py stop_times.txt (output: time_extracted.csv)
  * python extract_stop.py stops.txt (output: stop_extracted.csv)
  * python aggregate.py (input: time_extracted.csv and trip_extracted.csv, output: fullschedule.csv)
  * python construct_graph.py (input: fullschedule.csv ouput: nodes.txt and edges.txt)
  * python add_transfer.py transfers.txt nodes.txt (output: edges.txt - add more edges)

* Note that stop_extracted.csv and direction_extracted.csv are not used yet

* Run a test: 
 * ./a.out 40.574416 -73.978575 40.690645 -73.981934
 * ./a.out 40.627186, -74.0224549 40.6722028, -74.0111417
