#include <iostream>
#include <vector>
#include <string>
#include "simulator.h"
int main() {
 
int pageFrames  = 4;
  //Addresses are 8 bits. First 3 bits are the page number, last 5 bits are the page offset.
  std::vector<int> results;

   for (std::string line; std::getline(std::cin, line);) {
       results.push_back(stoi(line));
        
    }
    simulator foo(results,pageFrames);
    int test1 = 255;
    auto j  = foo.Optimal();
    std::cout << "number of missses " << j.misses << std::endl;
    std::cout << "number of hits " << j.hits << std::endl;
    
    for(auto h : j.addresses){
      //std::cout << " this was the address " << h << std::endl;
    }
   
 
 
 
 

}