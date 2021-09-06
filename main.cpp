#include <iostream>
#include <vector>
#include <string>
#include "simulator.h"
int main() {
 
int pageFrames  = 3;
  //Addresses are 8 bits. First 3 bits are the page number, last 5 bits are the page offset.
  std::vector<int> requests;

   for (std::string line; std::getline(std::cin, line);) {
       int request = stoi(line);
       if(request > 255){
         std::cout << " Requested address too large( > 255) " << std::endl;
         break;
       }
       requests.push_back(request);
        
    }
    simulator foo(requests,pageFrames);
    
    
    auto simResults  = foo.Clock();
   std::cout << "number of page faults running clock  " << simResults.misses << std::endl;
    std::cout << "number of hits running clock " << simResults.hits << std::endl;
    
    for(auto h : simResults.addresses){
     // std::cout << " this was the address " << h << std::endl;
    }
   
 
 
 
 

}