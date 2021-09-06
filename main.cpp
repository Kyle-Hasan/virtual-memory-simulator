#include <iostream>
#include <vector>
#include <string>
#include "simulator.h"
int main() {
 
  int pageFrames = 4;
  //Addresses are 8 bits. First 3 bits are the page number, last 5 bits are the page offset.
  std::vector<int> requests;
  //reads from standard input
   for (std::string line; std::getline(std::cin, line);) {
       int request = stoi(line);
       //8 bit addresses leads to 255 being the largest address that can be requested
       if(request > 255){
         std::cout << " Requested address too large( > 255) " << std::endl;
         break;
       }
       requests.push_back(request);
        
    }
    simulator foo(requests,pageFrames);
    
    //this can be changed to represent page replacement algorithm you want
    auto simResults  = foo.LRU();
    //displays results on console
   std::cout << "number of page faults running LRU  " << simResults.misses << std::endl;
    std::cout << "number of hits running LRU " << simResults.hits << std::endl;
    
    for(auto address : simResults.addresses){
      std::cout << " this was the address " << address << std::endl;
    }
   
 
 
 
 

}