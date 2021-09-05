#include "simulator.h"
#include <iostream>
#include <iterator>
simulator::simulator(std::vector<int> &requests, int &numFrames){
    this->requests = requests;
    this->numFrames = numFrames;

}



simulator::results simulator::FIFO(){
    results returnValue;
    std::deque<pageTableEntry*> Frames;
    std::unordered_map<int,pageTableEntry> pageTable;
    std::unordered_map<pageTableEntry*,std::deque<pageTableEntry*>::iterator> pageFramesIterators;
    
    
    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
        int pageOffset = ((request))& 0x1F;

        auto pageEntryAddress = &pageTable[virtualPageNumber];

        int physicalPageNumber;

        if(pageTable[virtualPageNumber].validTranslation == 0){
            pageTable[virtualPageNumber].validTranslation = 1;
            
            returnValue.misses++;

        
        
        if((int)Frames.size() == numFrames){
            pageTableEntry *evicted = Frames.front();
            Frames.pop_front();
            evicted->validTranslation = 0;
            pageFramesIterators.erase(evicted);

            }
            Frames.push_back(pageEntryAddress);
            pageFramesIterators[pageEntryAddress] = std::prev(Frames.end());

            physicalPageNumber = std::distance(Frames.begin(),pageFramesIterators[pageEntryAddress]);
            
        }
        else{
            physicalPageNumber = std::distance(Frames.begin(), pageFramesIterators[pageEntryAddress]);
            returnValue.hits++;
        }
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);
           



    }
    return returnValue;



}

simulator::results simulator::LRU(){
    results returnValue;
    std::list<pageTableEntry*> pageFrames;
    std::unordered_map<int,pageTableEntry> pageTable;
    std::unordered_map<pageTableEntry*,std::list<pageTableEntry*>::iterator> pageFramesIterators;
    //std::cout << "size of a pageTablEntry" << sizeof(pageTableEntry) << std::endl;
    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &pageTable[virtualPageNumber];
        int physicalPageNumber;
       // std::cout << " this is the request " << request << " this is the valid bit " << pageTable[virtualPageNumber].validTranslation << " this is the page number" << virtualPageNumber << std::endl;
        if(pageTable[virtualPageNumber].validTranslation == 0){
            pageTable[virtualPageNumber].validTranslation = 1;
            
            returnValue.misses++;
            if((int)pageFrames.size() == numFrames){
               // std::cout << " evicting a page " << std::endl;
                pageTableEntry *evicted = pageFrames.back();
                evicted->validTranslation = 0;
                pageFrames.pop_back();
                pageFramesIterators.erase(evicted);

            }
            //::cout << " placing new page into memory " << std::endl;
            pageFrames.push_front(pageEntryAddress);
            pageFramesIterators[pageEntryAddress] = pageFrames.begin();
            physicalPageNumber = 0;



        }
        else{
           // std::cout << "updating existing entry " << std::endl;
           physicalPageNumber = std::distance(pageFrames.begin(), pageFramesIterators[pageEntryAddress]);
            pageFrames.erase(pageFramesIterators[pageEntryAddress]);
            pageFrames.push_front(pageEntryAddress);
            pageFramesIterators[pageEntryAddress] = pageFrames.begin();

            returnValue.hits++;



        }
        
        //std::cout << "physical page number is " << physicalPageNumber << std::endl;
        
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);
        }
        
        return returnValue;

}

simulator::results simulator::Optimal(){
    results returnValue;
    std::list<pageTableEntry*> pageFrames;
    std::unordered_map<int,pageTableEntry> pageTable;
    std::unordered_map<pageTableEntry*,std::list<pageTableEntry*>::iterator> pageFramesIterators;
    for(int i = 0 ; i < (int)requests.size() ; i++){
        int request = requests[i];
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &pageTable[virtualPageNumber];
        int physicalPageNumber;
       
       
        if(!pageTable[virtualPageNumber].validTranslation){
            returnValue.misses++;
            pageTable[virtualPageNumber].validTranslation = true;
            
            if(numFrames == (int)pageFrames.size()){
                auto evicted = getFarthest(i,pageFrames,pageTable,pageFramesIterators);

                (*evicted)->validTranslation = false;
               
              
                pageFramesIterators.erase(*(evicted));
                *(evicted) = pageEntryAddress;
                
                pageFramesIterators[pageEntryAddress] = evicted;
                

            }
            else{
               
                pageFrames.push_back(pageEntryAddress);
                pageFramesIterators[pageEntryAddress] = std::prev(pageFrames.end());
               // std::cout << " this is the page frames end prev " << &*(std::prev(pageFrames.end())) << std::endl;
            }
            

        }
        else{
          //  std::cout << " theres a hit" << std::endl;
            returnValue.hits++;
        }
    
         
        // std::cout << " address of begin iterator " << &*(pageFrames.begin()) << " address of end iterator " << &*(pageFrames.end()) <<std::endl;
        
       //  std::cout << "\n"; 
        physicalPageNumber = std::distance(pageFrames.begin(), pageFramesIterators[pageEntryAddress]);
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);

    }

    return returnValue; 


}

std::list<simulator::pageTableEntry*>::iterator simulator::getFarthest(int index, std::list<pageTableEntry*> &pageFrames, 
    std::unordered_map<int,pageTableEntry> &pageTable,std::unordered_map<pageTableEntry*,std::list<pageTableEntry*>::iterator> &pageFramesIterators){
    bool usedInfuture;
      std::list<pageTableEntry*>::iterator furthestUsed;
       index++;
      for(auto &frame : pageFrames){
          usedInfuture = false;
          for(int starting = index; starting < (int)requests.size(); starting++){
            int virtualPageNumber = ((requests[starting]) & 0XE0) >> 5;
            auto pageEntry= pageTable.find(virtualPageNumber);
            if(pageEntry == pageTable.end()){
                continue;

            }
            else if(!pageEntry->second.validTranslation){
                continue;
            }
            pageTableEntry* pageEntryAddress = &(pageEntry->second);
            
            if(pageEntryAddress == frame){
               
                usedInfuture = true;
                furthestUsed = pageFramesIterators[frame];
            }



          }
          if(!usedInfuture){
              
              
              return pageFramesIterators[frame];

          }
         

      }
 
     return furthestUsed;
   

    }

