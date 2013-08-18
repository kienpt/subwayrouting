import datetime
import time
import sys

cur_t = time.strptime(sys.argv[1], '%H:%M:%S')
cur_t_sec = datetime.timedelta(hours=cur_t.tm_hour,minutes=cur_t.tm_min,seconds=cur_t.tm_sec).total_seconds()
print cur_t_sec
