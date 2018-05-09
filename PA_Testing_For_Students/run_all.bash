#!/bin/bash
port=$1

echo "Started on $port"

./server ./nope.txt 4061 &> nonexistant_serv.out
./client ./nope.txt 127.0.0.1 4061 &> nonexistant_client.out

echo "Running AV Test Case... on $port"
cd AV_Test
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running CP Test Case... on $port"
cd CP_Test
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running CV Test Case... on $port"
cd CV_Test
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running OP Test Case... on $port"
cd OP_Test
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running RV Test Case... on $port"
cd RV_Test
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running RW Test Case... on $port"
cd RW_Test
timeout 10 ./run.bash $port
cd ..

port=$((port+4))
echo "Running TestCase01... on $port"
cd TestCase01
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running TestCase02... on $port"
cd TestCase02
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running TestCase03... on $port"
cd TestCase03
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running TestCase04...on $port"
cd TestCase04
timeout 10 ./run.bash $port
cd ..

port=$((port+4))
echo "Running TestCase05... on $port"
cd TestCase05
timeout 10 ./run.bash $port
cd ..

port=$((port+5))
echo "Running TestCase06... on $port"
cd TestCase06
timeout 10 ./run.bash $port
cd ..

port=$((port+7))
echo "Running TestCase07... on $port"
cd TestCase07
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running TestCaseE1 (Extra Credit)... on $port"
cd TestCaseE1
timeout 10 ./run.bash $port
cd ..

port=$((port+2))
echo "Running TestCaseE2 (Extra Credit)... on $port"
cd TestCaseE2
timeout 10 ./run.bash $port
cd ..

echo "Ended on $port"
