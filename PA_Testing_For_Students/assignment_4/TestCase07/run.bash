#!/bin/bash
rm server_out.txt -f
rm client_out1.txt -f 
rm messages1.txt -f 
rm client_out2.txt -f 
rm messages2.txt -f 
rm client_out3.txt -f 
rm messages3.txt -f 
rm client_out4.txt -f 
rm messages4.txt -f 
rm client_out5.txt -f 
rm messages5.txt -f 
rm client_out6.txt -f 
rm messages6.txt -f 
rm client_out7.txt -f 
rm messages7.txt -f 

server_facing_port=$1
client_facing_port=$((server_facing_port+1))

# THIS TOOK 4 HOURS TO FIGURE OUT.
script --return --quiet -c  "./server ./voting_regions.dag $server_facing_port" server_out.txt > /dev/null &
serverpid=$!

# Runnning opener client.
sleep .5
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port &> messages1.txt &

sleep .5
./client ./input_open.req 127.0.0.1 $client_facing_port &> client_out1.txt

client_facing_port1=$((client_facing_port+1))
client_facing_port2=$((client_facing_port1+1))
client_facing_port3=$((client_facing_port2+1))
client_facing_port4=$((client_facing_port3+1))
client_facing_port5=$((client_facing_port4+1))

# Running concurrent clients.
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port1 &> messages2.txt &
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port2 &> messages3.txt &
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port3 &> messages4.txt &
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port4 &> messages5.txt &
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port5 &> messages6.txt &
sleep .5

./client ./input_1.req 127.0.0.1 $client_facing_port1 &> client_out2.txt &
./client ./input_2.req 127.0.0.1 $client_facing_port2 &> client_out3.txt &
./client ./input_3.req 127.0.0.1 $client_facing_port3 &> client_out4.txt &
./client ./input_4.req 127.0.0.1 $client_facing_port4 &> client_out5.txt &
./client ./input_5.req 127.0.0.1 $client_facing_port5 &> client_out6.txt 

client_facing_port6=$((client_facing_port5+1))

# Closing polls client.
sleep .5
python middleman.py 127.0.0.1 $server_facing_port $client_facing_port6 &> messages7.txt &

sleep .5
./client ./input_close.req 127.0.0.1 $client_facing_port6 &> client_out7.txt

kill $serverpid
