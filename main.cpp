#include <iostream>
#include <vector>
#include <string>
#include "simulator.h"
#include <fstream>
//receives args via command line
int main(int argc, char *argv[]) {
  std::ifstream file;
  int pageFrames = 0;
  std::string algo;
  //first arg given is page frame number, second arg is file path, 3rd arg is algo type
  //if invalid number of args is given, program terminates
  if(argc == 4){
  file.open(argv[2]);
  pageFrames = atoi(argv[1]);
  algo = argv[3];
  }
  else{
    std::cout << "Usage is ./(executable name) (page frames number) (path to file) (algorithm type)" << std::endl;
    //terminates program after invalid number of args
    std::exit(0);
  }
  
  //Addresses are 8 bits. First 3 bits are the page number, last 5 bits are the page offset.
  //requests stores the address requests made by the process
  std::vector<int> requests;
  //reads from file only if the file is open and pageFrames is greater than 0.
  if(file.is_open() && pageFrames > 0){
   for (std::string line; std::getline(file, line);) {
       int request = stoi(line);
       //8 bit addresses leads to 255 being the largest address that can be requested
       if(request > 255){
         std::cout << " Requested address too large( > 255) " << std::endl;
         break;
       }
       requests.push_back(request);
        
    }
    file.close();
    simulator foo(requests,pageFrames);
    simulator::results simResults;
    bool valid = true;
    if(algo == "LRU"){
     simResults  = foo.LRU();
    }
    else if(algo == "Optimal"){
      simResults = foo.Optimal();
    }
    else if(algo == "Clock"){
      simResults = foo.Clock();
    }
    else if(algo == "FIFO"){
      simResults = foo.FIFO();
    }
    else{
      valid = false;
      std::cout << " This is not a valid algorithm, please choose LRU,Optimal,Clock or FIFO" << std::endl;
    }
    //displays results on console
  if(valid){
   std::cout << "number of page faults running " << algo << " " << simResults.misses << std::endl;
   std::cout << "number of hits running " << algo << " " << simResults.hits << std::endl;
   
    
    for(auto address : simResults.addresses){
      std::cout << " this was the address " << address << std::endl;
    }
  }
  }
  else{

    std::cout << " invalid file or page frames number" << std::endl;
    std::cout << "Usage is ./(executable name) (page frames number) (path to file) (algorithm type)" << std::endl;
  }
 
 
 
 

}