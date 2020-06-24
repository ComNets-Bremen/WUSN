#!/bin/bash



for (( counter=1; counter<=10; counter++ ))
do
../../AdaptiveCommWUSN -r "$counter" -u Cmdenv -c General -n ..:../../src omnetpp.ini > vwc1chan1prob1.txt
echo -n "$counter "
done
printf "\n"





for (( counter=4; counter>=0; counter-- ))
do
../../AdaptiveCommWUSN -r "$counter" -u Cmdenv -c General -n ..:../../src omnetpp.ini > vwc"$counter"chan1prob1.txt
echo -n "$counter "
done
printf "\n"
