Names: Carlos Alvarenga, Jeremy Herzog



### Purpose:
Simulate a vote counting system of an election with a client-server program that uses thread programming and synchronization. Requests will be sent to the server
and the server will send back responses to determine the winner of the election.



### How to compile & shell commands:
Included makefile compiles the main driver programs client.c & server.c with the "make" command on the terminal.
The server program can be run with the following terminal command: "./server <DAG FILE> <Server Port>" where
- <DAG FILE> is the file containing the specification of the DAG, under any filename
- <Server Port> is any unsigned integer to be used as a port number.
The client program can be run with the following terminal command: "./client <REQ FILE> <Server IP> <Server Port>" where
- <REQ FILE> is the name of a file containing the requests to be sent to the server
- <Server IP> is the IP of the server to connect to
- <Server Port> is the port of the server to connect to.



### Program functionality:
The main driver programs are client.c and server.c and the supplementary files are utils.h, map.h and list.h
client.c works in the following manner:
- The REQ file is first checked to exist
- Then, a loop is entered to read every line of the REQ file (if it exists)
- At each iteration, the line is parsed and createRequest() is called to create the request string from the REQ file line
- After creating the request, sendRequest() is called to send the request string to the server and then the response is read back from the server
- Morover, at each iteration the client prints out the following useful information:
     - “Initiated connection with server at <SERVER IP>:<SERVER PORT>”
     ● “Sending request to server: <REQUEST_CODE> ​ <REQUEST_DATA>​ ”
     ● “Received response from server: <RESPONSE_CODE> <RESPONSE_DATA>”
     ● “Closed connection with server at <SERVER IP>:<SERVER PORT>”
server.c works in the following manner:
- The DAG file is first checked to exist
- if it does exist, the file is read and an appropriate tree structure is constructed where each region is treated as a poll with candidates
- Then, the server accepts a connection from the client and reads in the requests from the client
- At each iteration, a thread is creating to handle the client request by calling handleRequest()
- In handleRequest(), the client request string is parsed and the appropriate function is called based on the 2-character request code to handle the given request
- At each function call the response string is created and built upon and once the called function exists, the response is sent back to the client
- This process continues on as long there's a connection between the client and server and the client has requests to be sent to the server
- Moreover, at each iteration the server prints out the following useful information:
     ● “Server listening on port <PORT_NUMBER>”
     ● “Connection initiated from client at <CLIENT IP>:<CLIENT PORT>”
     ● “Request received from client at <CLIENT IP>:<CLIENT PORT>, <REQUEST_CODE>, ​ <REQUEST_DATA>​ ”
     ● “Sending response to client at <CLIENT IP>:<CLIENT PORT>, <RESPONSE_CODE> ​ <RESPONSE_DATA>​ ”
     ● “Closed connection with client at <CLIENT IP>:<CLIENT PORT>”

- Makefile completed by both
- client.c, utils.h, README completed by Carlos
- server.c, map.h, list.h completed by Jeremy
