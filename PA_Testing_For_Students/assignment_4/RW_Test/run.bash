#!/bin/bash
rm server_out1.txt -f
rm client_out1.txt -f 
rm messages1.txt -f 
rm server_out2.txt -f
rm client_out2.txt -f 
rm messages2.txt -f 

server_facing_port=$1
client_facing_port=$((server_facing_port+1))

# THIS TOOK 4 HOURS TO FIGURE OUT.
script --return --quiet -c  "./server ./voting_regions.dag $server_facing_port" server_out1.txt > /dev/null &
serverpid=$!

sleep .5
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port &> messages1.txt &

sleep .5
./client ./input_1.req 127.0.0.1 $client_facing_port &> client_out1.txt

kill $serverpid

sleep .5
server_facing_port=$((server_facing_port+2))
client_facing_port=$((server_facing_port+1))

# THIS TOOK 4 HOURS TO FIGURE OUT.
script --return --quiet -c  "./server ./voting_regions.dag $server_facing_port" server_out2.txt > /dev/null &
serverpid=$!

sleep .5
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port &> messages2.txt &

sleep .5
./client ./input_2.req 127.0.0.1 $client_facing_port &> client_out2.txt


kill $serverpid
