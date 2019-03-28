Names: Carlos Alvarenga, Jeremy Herzog



### Purpose
Simulate a vote counting system of an election with a client-server program that uses thread programming and synchronization. Requests will be sent to the serverand the server will send back responses to determine the winner of the election.

[Assignment guidelines and specifications](https://github.com/carlosandfound/Client-Server-Voting-System/blob/master/project-requirements.pdf)



### How to compile & shell commands
Included makefile compiles the main driver programs client.c & server.c with the "make" command on the terminal.
The server program can be run with the following terminal command: "./server &lt;DAG FILE&gt; &lt;Server Port&gt;" where
- &lt;DAG FILE&gt; is the file containing the specification of the DAG, under any filename
- &lt;Server Port&gt; is any unsigned integer to be used as a port number.
The client program can be run with the following terminal command: "./client &lt;REQ FILE&gt; &lt;Server IP&gt; &lt;Server Port&gt;" where
     - &lt;REQ FILE&gt; is the name of a file containing the requests to be sent to the server
     - &lt;Server IP&gt; is the IP of the server to connect to
     - &lt;Server Port&gt; is the port of the server to connect to.



### Program functionality
The main driver programs are client.c and server.c and the supplementary files are utils.h, map.h and list.h
client.c works in the following manner:
- The REQ file is first checked to exist
- Then, a loop is entered to read every line of the REQ file (if it exists)
- At each iteration, the line is parsed and createRequest() is called to create the request string from the REQ file line
- After creating the request, sendRequest() is called to send the request string to the server and then the response is read back from the server

Morover, at each iteration the client prints out the following useful information:
- “Initiated connection with server at &lt;SERVER IP&gt;:&lt;SERVER PORT&gt;”
- “Sending request to server: &lt;REQUEST_CODE&gt; ​ &lt;REQUEST_DATA&gt;​ ”
- “Received response from server: &lt;RESPONSE_CODE&gt; &lt;RESPONSE_DATA&gt;”
- “Closed connection with server at &lt;SERVER IP&gt;:&lt;SERVER PORT&gt;”

server.c works in the following manner:
- The DAG file is first checked to exist
- If it does exist, the file is read and an appropriate tree structure is constructed where each region is treated as a poll with candidates
- Then, the server accepts a connection from the client and reads in the requests from the client
- At each iteration, a thread is creating to handle the client request by calling handleRequest()
 - In handleRequest(), the client request string is parsed and the appropriate function is called based on the 2-character request code to handle the given request
 - At each function call the response string is created and built upon and once the called function exists, the response is sent back to the client
 - This process continues on as long there's a connection between the client and server and the client has requests to be sent to the server
- Moreover, at each iteration the server prints out the following useful information:
     - “Server listening on port &lt;PORT_NUMBER&gt;”
     - “Connection initiated from client at &lt;CLIENT IP&gt;:&lt;CLIENT PORT&gt;”
     - “Request received from client at &lt;CLIENT IP&gt;:&lt;CLIENT PORT&gt;, &lt;REQUEST_CODE&gt;, ​ &lt;REQUEST_DATA&gt;​ ”
     - “Sending response to client at &lt;CLIENT IP&gt;:&lt;CLIENT PORT&gt;, &lt;RESPONSE_CODE&gt; ​ &lt;RESPONSE_DATA&gt;​ ”
     - “Closed connection with client at &lt;CLIENT IP&gt;:&lt;CLIENT PORT&gt;”


Makefile completed by both

client.c, utils.h, README completed by Carlos

server.c, map.h, list.h completed by Jeremy
