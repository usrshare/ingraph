#!/bin/bash
while true; do

cat /proc/loadavg | cut -d ' ' -f 1-3
sleep 5
done
