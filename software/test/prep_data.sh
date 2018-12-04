#!/bin/bash
#
#   Takes all the .data files and creates a randomized output merging
#   all of the test data records together into a single file
#

TEMP_FILE=sensor_records.tmp
TEMP_FILE_2=sensor_records.tmp2
OUTPUT_FILE=sensor_records.out
RANDOM_RANGE=1000

if [ -s ${OUTPUT_FILE} ]
then
    rm ${OUTPUT_FILE}           # clear it out
fi

for FILE in *.data
do
    while read LINE
    do
        echo `printf %03d $((RANDOM % 1000))` $LINE >>${TEMP_FILE}
    done <${FILE}
done

sort ${TEMP_FILE} | cut "-d " -f2- >${TEMP_FILE_2}

while read LINE
do
    echo "$LINE" >>${OUTPUT_FILE}
done <${TEMP_FILE_2}

rm ${TEMP_FILE} ${TEMP_FILE_2}
