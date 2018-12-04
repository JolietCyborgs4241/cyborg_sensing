#!/bin/bash

if [ "$#" -lt 2 ]
then
    echo "`basename $0`: usage: $0 host port"
    exit 1
fi

HOST=$1
PORT=$2
INTERVAL=0
DELAY=0

SENSOR_DATA=sensor_records.out

OLD_IFS=${IFS}
IFS=$'\n'

MSGS=(
`cat ${SENSOR_DATA}`
)

IFS=${OLD_IFS}

MSG_COUNT=${#MSGS[@]}

echo `basename $0`: $MSG_COUNT messages to be sent to ${HOST}:${PORT} via UDP
echo

for (( i=0; i <${MSG_COUNT}; i++ ));
do
    echo -n "Sending MSGS[${i}]: \"${MSGS[$i]}\"..."
    echo -n "${MSGS[$i]}" > /dev/udp/$HOST/$PORT
    echo "  sent."

    # do a random delay from 0 to .099 seconds
    DELAY=`printf %03d $((RANDOM % 100))`
    echo "Delaying for 0.${DELAY}s ..."
    sleep "0.${DELAY}"
done
