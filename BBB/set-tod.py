import json
import argparse
from datetime import datetime
import warnings


import shutil
import os
import sys
import re

from dateutil.parser import parse as dateutil_parse
from dateutil import tz

parser = argparse.ArgumentParser(description='Event JSON Updater')

parser.add_argument('-e', help='Event name.') #event name
parser.add_argument('-t', required=False) # time
parser.add_argument('-d', required=False) # day
parser.add_argument('-c', required=False) # claimer
parser.add_argument('-n', required=False) # target
parser.add_argument('-x', required=False) # extra note
parser.add_argument('--start', required=False, type=int) # minium wait in seconds (21 hours = 75600 seconds) 
parser.add_argument('--interval', required=False, type=int) # interval between windows in seconds (1800 for 30 mins) 
parser.add_argument('--alerts', required=False) # alert intervals list of integers
parser.add_argument('--windows', required=False, type=int) # number of windows

args = parser.parse_args()


json_file = '/path/to/events.json'
backup_file = '/path/to/events-{}.json'.format(
    datetime.now().strftime('%m-%d-%Y--%H-%M-%S')
)

#make a backup...
shutil.copyfile(json_file, backup_file)

with open(json_file) as f:
    json_data = json.load(f)

# placeholder fields...
new_data = {
    "tod": 0,
    "windows": 1,
    "day" : 1,
    "start": 0,
    "alerts": [0],
    "custom": 1,
    "interval": 0,
}

# start with the existing entry if we already have stuff there, then update it
if args.e in json_data.keys():
    new_data = json_data[args.e]


if args.t != None:
    # we have a time stamp sent, so let's try parsing it. 
    
    target_timestamp = 0
    if 'tod' in new_data.keys():
        target_timestamp = new_data['tod']
    
    try:
        int_ver = int(args.t)
    except ValueError:
        int_ver = 0

    # this is the 01/01/2023 at midnight...
    # basically we're heuristically detecting if we were passed a unix timestamp
    # if we get one, use that, otherwise, parse it
    
    if args.t == "same":
        target_timestamp = new_data['tod']
    elif int_ver >= 1672549200:
        target_timestamp = int_ver
    else:
        
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            parsed_date = dateutil_parse(args.t)


        target_tz = 'America/New_York'
        if 'ET' in args.t or 'EST' in args.t or 'EDT' in args.t:
            target_tz = 'America/New_York'
        elif 'CT' in args.t or 'CST' in args.t or 'CDT' in args.t:
            target_tz = 'America/Chicago'
        elif 'PT' in args.t or 'PST' in args.t or 'PDT' in args.t:
            target_tz = 'America/Los_Angeles'
        elif 'GMT' in args.t or 'UTC' in args.t or 'CDT' in args.t:
            target_tz = 'Europe/London'

        parsed_date = int( parsed_date.replace(tzinfo=tz.gettz(target_tz)).timestamp() )

        if parsed_date < 1672549200:
            # something went wrong if it parses to a date before 1/1/23...
            print('Date failed to parse.')
        else:
            target_timestamp = parsed_date

    new_data['tod'] = target_timestamp

if args.d != None:
    if args.d == '+1':
        new_data['day'] = int(new_data['day']) + 1
    else:
        day = int(args.d)
        if day < 30 and day >=0:
            new_data['day'] = day

if args.c != None:
    claimer = args.c
    if len(claimer) <= 16 and re.match('^[A-Z][a-z]+$', claimer):
        new_data['claimer'] = claimer

 

if args.n != None:
    target_name = args.n
    if len(target_name) <= 50 and re.match('^[A-Z][A-Za-z0-9 _-]$', target_name):
        new_data['target_name'] = target_name

if args.x != None:
    notes = args.x
    if len(notes) <= 50:
        new_data['notes'] = notes


if args.start != None:
    print("Min time={start}".format(start=args.start))

    if args.start >= 0: 
        new_data['start'] = args.start

if args.interval != None:
    print("Inteveral={interval}".format(interval=args.interval))

    if args.interval > 0:
        new_data['interval'] = args.interval

if args.alerts != None:
    print("Alerts={alert}".format(alert=args.alerts))

    alert_list = []
    if ',' in args.alerts:
        alert_times = args.alerts.split(',')
        for t in alert_times:
            try:
                alert_list.append(int(t))
            except ValueError:
                print('Bad alert list.')
                sys.exit()
    else:
        try:
            alert_list.append(int(args.alerts))
        except ValueError:
            print('Bad alert singular.')
            sys.exit()
    
    if len(alert_list) > 0:
        new_data['alerts'] = alert_list

if args.windows != None:
    print("Windows={windows}".format(windows=args.windows))

    if args.windows >= 0:
        new_data['windows'] = args.windows

if not args.e in json_data.keys():
    print('Adding {} to events...'.format(args.e))


json_data[args.e] = new_data

with open(json_file, 'w') as f:
    json.dump(json_data, f)


print('OK: {event} @ {td} ({tod}) W:{windows} D:{day} S:{start} I:{interval}.'.format(
    event=args.e, 
    tod=json_data[args.e]['tod'],
    td = datetime.fromtimestamp(
        json_data[args.e]['tod']).replace(
            tzinfo=tz.gettz('America/New_York')
        ).strftime('%m-%d-%Y %H:%M:%S ET'),
    windows = new_data['windows'],
    day = new_data['day'],
    start = new_data['start'],
    interval = new_data['interval']
    ))




