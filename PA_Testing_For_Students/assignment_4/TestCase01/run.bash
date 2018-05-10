#!/bin/bash
rm server_out.txt -f
rm client_out.txt -f 
rm messages.txt -f 

server_facing_port=$1
client_facing_port=$((server_facing_port+1))

# THIS TOOK 4 HOURS TO FIGURE OUT.
script --return --quiet -c  "./server ./voting_regions.dag $server_facing_port" server_out.txt > /dev/null &
serverpid=$!

sleep .5
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port &> messages.txt &

sleep .5
./client ./input.req 127.0.0.1 $client_facing_port &> client_out.txt


kill $serverpid
