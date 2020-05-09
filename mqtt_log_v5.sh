#!/bin/sh
# Log MQTT to file, new file per day
# Author: Rui Pires
# proudly sourced from: http://www.cyberciti.biz/tips/shell-scripting-creating-reportlog-file-names-with-date-in-filename.html
# ---------------------------------------------------------------------------------------------------------
 
## date format ##
YESTERDAY=$(date -d "yesterday"  +"%Y-%m-%d")
NOW=$(date +"%F")
NOWT=$(date +"%T")
 
## Backup path ##
BAK="/home/rpires/git/arduino_nose/log"

## Filename
FILE="$BAK/anose-$NOW.csv"


## unlogging procedure
#mosquitto_sub -t home/nose -U home/nose &
mosquitto_sub -v -h 192.168.1.201 -t 'home/nose' -U home/nose &

## logging procedure
#mosquitto_sub -t home/nose > $FILE &
#date > $FILE &
mosquitto_sub -v -h 192.168.1.201 -t 'home/nose'  > $FILE &


#Example
#crontab 
#0 0 * * * * mqtt_log.sh
#pi@raspberrypi:~ $ ls
#acetone.csv  alcohol.csv  bin  butane.csv  clean_air.csv  e_nose_log-2019-11-17.csv  network-manager.sh
#pi@raspberrypi:~ $ pwd
#/home/pi
