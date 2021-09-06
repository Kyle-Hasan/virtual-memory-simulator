# virtual-memory-simulator
Simulates virtual memory in an operating system using LRU,Optimal,Clock and FIFO page replace algorithms\
<br/><br/>
Receives input in the from command line arguments containing number of page frames, path to text file containing
virtual address translation requests and type of algorithm to run. To see example files, please look in tests folder.
Outputs the number of hits, page faults and physical address translations on the console.
<br/><br/>
In order to run, first compile using g++ main.cpp simulator -o (executable name) where executable name can be whatever you want. \
Then in order to run executable, go <br> ./(executable name) (number of page frames) (relative path to text file) (type of algorithm(LRU,Optimal,Clock,FIFO)) \
For example in order to run the FIFO algorithm on tests/test1.txt with 3 page frames with an executable name of main, go
<br>./main 3 tests/test1.txt FIFO
