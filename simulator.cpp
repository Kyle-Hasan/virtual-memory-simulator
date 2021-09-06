#include "simulator.h"

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
    std::unordered_map<pageTableEntry*,int> pageFrames;
    std::unordered_map<int,pageTableEntry> pageTable;
   // std::cout << " num frames is " << numFrames << std::endl;
    for(int i = 0 ; i < (int)requests.size() ; i++){
        int request = requests[i];
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &pageTable[virtualPageNumber];
        int physicalPageNumber;
       
       
        if(pageTable[virtualPageNumber].validTranslation){
           returnValue.hits++;

            }
        else{
         // std::cout << "vpn missed " << virtualPageNumber << "request " << request << std::endl;
            returnValue.misses++;
            pageTable[virtualPageNumber].validTranslation = true;
            
            if(numFrames == (int)pageFrames.size()){
                pageTableEntry* evicted = getFarthest(i,pageTable,pageFrames);
                
                

                evicted->validTranslation = false;
               
                int pageFrameIndex = pageFrames[evicted];
                pageFrames.erase(evicted);
                pageFrames[pageEntryAddress] = pageFrameIndex;
                
                
                

            }
            else{
               
                pageFrames[pageEntryAddress] = pageFrames.size();
                
               // std::cout << " this is the page frames end prev " << &*(std::prev(pageFrames.end())) << std::endl;
            }
          
        }
    
         
        // std::cout << " address of begin iterator " << &*(pageFrames.begin()) << " address of end iterator " << &*(pageFrames.end()) <<std::endl;
        
       //  std::cout << "\n"; 
        physicalPageNumber = pageFrames[pageEntryAddress];
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);

    }

    return returnValue; 


}

simulator::results simulator::Clock(){
    results returnValue;
    std::vector<std::pair<pageTableEntry*,bool >> pageFrames;
    std::unordered_map<int, pageTableEntry> pageTable;
    std::unordered_map<pageTableEntry*,int> pageFrameIndexes;
    int clockHand = 0;
    bool secondChance;

    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &(pageTable[virtualPageNumber]);
        int physicalPageNumber;
        //std::cout << " request is " << request << std::endl;
        if(pageTable[virtualPageNumber].validTranslation){
            returnValue.hits++;
            std::cout << " request hit " << request << std::endl;
            
           std::cout << " page entry address " << pageEntryAddress << std::endl;
            int index = pageFrameIndexes[pageEntryAddress] ;
            pageFrames[index].second = true;
            std::cout << " set " << index << " to true" << std::endl;
            }

        else{
            std::cout << " request missed " << request/32 << std::endl;
            returnValue.misses++;
            pageTable[virtualPageNumber].validTranslation = true;

            if(numFrames == (int)pageFrames.size()){

                while(true){
                   // std::cout << "clock hand is at " << clockHand << std::endl;
                    secondChance = pageFrames[clockHand].second;
                    if(!secondChance){
                        break;
                    }
                   // std::cout << "setting the value at " << clockHand << " to false" << std::endl;
                    pageFrames[clockHand].second = false;
                    clockHand = (clockHand+1)%numFrames;
                }
                std::cout << std::endl;

                
                //std::cout << "clock hand ended at " << clockHand << std::endl;
                std::pair<pageTableEntry*,bool > evicted = pageFrames[clockHand];
                
                pageFrames[clockHand].first->validTranslation = false;
                pageFrameIndexes.erase(evicted.first);
                
                pageFrames[clockHand] = std::make_pair(pageEntryAddress,true);
                pageFrameIndexes[pageEntryAddress] = clockHand;
               



            }
            else{
                pageFrames.push_back(std::make_pair(pageEntryAddress,true));
                pageFrameIndexes[pageEntryAddress] = pageFrames.size()-1;

            }
           


        }
         for(auto &h : pageTable){
                if(h.second.validTranslation){
                std::cout << " address of " << h.first*32 << " is at " << &(h.second) << std::endl;
                }

            }
             for(int i = 0; i < pageFrames.size(); i++){
                    std::cout << " page frames contains " << pageFrames[i].first << " at " << i << "valid bit " << pageFrames[i].second << std::endl;
                }
                std::cout << "hand is at " << clockHand << std::endl;
            std::cout << std::endl;
        physicalPageNumber = pageFrameIndexes[pageEntryAddress];
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);
        

    }


    return returnValue;
}

simulator::pageTableEntry* simulator::getFarthest(int index, 
    std::unordered_map<int,pageTableEntry> &pageTable, std::unordered_map<pageTableEntry*,int> &pageFrames){
    bool usedInfuture;
      pageTableEntry* furthestUsed;
       index++;
       int furthest = -1;
      for(auto &frame : pageFrames){
          usedInfuture = false;
          pageTableEntry* currentFrame = frame.first;

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
            
            
            if(pageEntryAddress == currentFrame ){
                if(starting > furthest){
                furthest = starting;
                furthestUsed = currentFrame;
                }
               
                usedInfuture = true;
                break;
                
            }



          }
          
          if(!usedInfuture){
            
            return currentFrame;

          }
         

      }
    
     return furthestUsed;
   

    }
