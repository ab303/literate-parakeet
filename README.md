# unordered multicasting
This project deals with the implementation of unordered multicasting in C using UDP protocols.
In this implentation each node multicasts messages on a thread and listens on a different thread.

To compile the programs:

type:

gcc unordered.c -lpthread -o unordered



To run the program:

 - ./unordered <total-no-of-nodes>



Note : 
 to run unordered multicast for 3 nodes follow the sample command and open 3 separate terminals
* Sample Command : 
new terminal
./unordered 3
new terminal
./unordered 3
new terminal
./unordered 3
 
