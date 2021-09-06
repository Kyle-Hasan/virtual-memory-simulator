#include "simulator.h"

simulator::simulator(std::vector<int> &requests, int &numFrames){
    this->requests = requests;
    this->numFrames = numFrames;

}


simulator::results simulator::FIFO(){
    results returnValue;
    //stores pointers to pageTableEntries in order to edit them when they are removed
    std::deque<pageTableEntry*> Frames;
    //maps virtual page number to a pageTable entry, which is really just a bool
    //telling you whether or not the page is in memory
    std::unordered_map<int,pageTableEntry> pageTable;
    //values are iterators to deque in order to get index of entries for physical page translations.
    //the keys pageTableEntry* in order to erase entries when an entry is removed from frames
    std::unordered_map<pageTableEntry*,std::deque<pageTableEntry*>::iterator> pageFramesIterators;

    for(const auto &request : requests){
        //uses bit masks to store specified bits
        int virtualPageNumber = ((request) & 0XE0) >> 5;
        int pageOffset = ((request))& 0x1F;

        auto pageEntryAddress = &pageTable[virtualPageNumber];

        int physicalPageNumber;

        //validTranslation false means the page is not in memory and needs to be loaded in
        if(!pageTable[virtualPageNumber].validTranslation){
            //sets valid translation to true to represent bringing page into memory
            pageTable[virtualPageNumber].validTranslation = true;
            //records page fault
            returnValue.misses++;

        //if the queue holds the max number of page frames in memory, a page has to be evicted
        if((int)Frames.size() == numFrames){
            //removes the pageEntry at the front from physical memory and makes sure it is no longer a valid translation
            pageTableEntry *evicted = Frames.front();
            Frames.pop_front();
            evicted->validTranslation = false;
            //removes iterator to queue in order to save space
            pageFramesIterators.erase(evicted);

            }
            //adds new page to back of queue, represented by adding it's address
            Frames.push_back(pageEntryAddress);
            //sets the iterator to the pageEntry to point to the last element of the queue.
            pageFramesIterators[pageEntryAddress] = std::prev(Frames.end());

            //sets the physical page number as the distance from the iterator at the beginning of the queue 
            //to the iterator where the page entry actually is.
            physicalPageNumber = std::distance(Frames.begin(),pageFramesIterators[pageEntryAddress]);
            
        }
        else{
            //if there is a hit, find the iterator to the pageEntry and calculate physical page number.
            physicalPageNumber = std::distance(Frames.begin(), pageFramesIterators[pageEntryAddress]);
            returnValue.hits++;
        }
        //calculates address as physicalPageNumber*32+ pageOffset because the page sizes are 32.
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);

        }
    return returnValue;

}

simulator::results simulator::LRU(){
    results returnValue;
    //stores page frames as a doubly linked list
    //least recently page is at the back ,most recently used page is at the front
    std::list<pageTableEntry*> pageFrames;
    //maps virtual page number to a page table entry
    std::unordered_map<int,pageTableEntry> pageTable;
    //maps entries from page frames to their respective iterator in the page frames list
    std::unordered_map<pageTableEntry*,std::list<pageTableEntry*>::iterator> pageFramesIterators;
    
    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &pageTable[virtualPageNumber];
        int physicalPageNumber;
       //if the validTranslation is false, then page is not in memory and needs to be loaded in 
        if(!pageTable[virtualPageNumber].validTranslation){
            //sets valid translation to true in order to represent page is going to be loaded into memory
            pageTable[virtualPageNumber].validTranslation = true;
            
            returnValue.misses++;
            //if the list has the maximum number of page frames in memory, then the 
            //least recently used page needs to be evicted
            if((int)pageFrames.size() == numFrames){
                //the least recently used page is always at the back of the list, so it's evicted
                pageTableEntry *evicted = pageFrames.back();
                //makes sure the evicted page has a false translation and removes it from list
                evicted->validTranslation = false;
                pageFrames.pop_back();
                //erases iterator to evicted entry
                pageFramesIterators.erase(evicted);

            }
            //places new page at front of list because new page is the most recently used page
            pageFrames.push_front(pageEntryAddress);
            //sets iterator of new page entry to intially start at beginning of the list
            //(this gets updated when new pages are added)
            pageFramesIterators[pageEntryAddress] = pageFrames.begin();
            //start of the list is always page 0 in physical memory
            physicalPageNumber = 0;
        }
        //this else runs if there is a hit
        else{
            //sets the physical page number as the distance from the iterator at the beginning of the list
            //to the iterator where the page entry actually is.
           physicalPageNumber = std::distance(pageFrames.begin(), pageFramesIterators[pageEntryAddress]);
            //moves the page from the back of the list to the front of the list to represent it's the most recently used page
            pageFrames.erase(pageFramesIterators[pageEntryAddress]);
            pageFrames.push_front(pageEntryAddress);
            pageFramesIterators[pageEntryAddress] = pageFrames.begin();

            returnValue.hits++;
            }
        //calculates address as physicalPageNumber*32+ pageOffset because the page sizes are 32.
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);
        }
        
        return returnValue;
    }


simulator::results simulator::Optimal(){
    results returnValue;
    //maps pageTableEntry pointers to physical page numbers. 
    //uses pointers so that we can edit the page table entries in pageTable
    std::unordered_map<pageTableEntry*,int> pageFrames;
    //maps virtual page numbers to page table entries
    std::unordered_map<int,pageTableEntry> pageTable;
  
    for(int i = 0 ; i < (int)requests.size() ; i++){
        int request = requests[i];
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &pageTable[virtualPageNumber];
        int physicalPageNumber;
       
        //if there is a hit there is nothing  that needs to be done other than update the counter
        if(pageTable[virtualPageNumber].validTranslation){
           returnValue.hits++;

            }
        //this else runs if there is a page fault
        else{
         
            returnValue.misses++;
            //sets valid translation to true in order to represent page is going to be loaded into memory
            pageTable[virtualPageNumber].validTranslation = true;
            
            //if the map contains the maximum of page frames in memory, the page used furthest in future
            //or not at all needs to be evicted.
            if(numFrames == (int)pageFrames.size()){
                //gets farthest used or used page entry in the future
                //the i represents the index of the current page request, so all indexes after are in the future
                pageTableEntry* evicted = getFarthest(i,pageTable,pageFrames);
                evicted->validTranslation = false;
                //gets physical page number of evicted page
                int pageFrameIndex = pageFrames[evicted];
                //erases evicted page from memory
                pageFrames.erase(evicted);
                //replaces page at evicted physical page number with new page
                pageFrames[pageEntryAddress] = pageFrameIndex;

                }
            //this else runs if there's a page frame open in memory
            else{
               //sets physical page number of page entry to be the first open frame.
                pageFrames[pageEntryAddress] = pageFrames.size();
                }
          
        }
        physicalPageNumber = pageFrames[pageEntryAddress];
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);

    }
    return returnValue; 
}


simulator::results simulator::Clock(){
    results returnValue;
    /* represents circular linked list as vector, this stores page frames in memory
    //the pair represents the pageTable entry as a pointer(so that we can edit the entries of evicted pages
    in the pageTable map) 
    the bool represents the "second chance" bit*/
    std::vector<std::pair<pageTableEntry*,bool >> pageFrames;
    //maps physical page numbers to page table entries
    std::unordered_map<int, pageTableEntry> pageTable;
    /*maps page table entries to physical page numbers(pointer are used so that entries evicted from pageFrames)
    can be erased from this map by value.
    */
    std::unordered_map<pageTableEntry*,int> pageFrameIndexes;
    int clockHand = 0;
    bool secondChance;

    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
        auto pageEntryAddress = &(pageTable[virtualPageNumber]);
        int physicalPageNumber;
        //std::cout << " request is " << request << std::endl;
        /*if there a is valid translation for the virtual page number, then there is a hit
        */
        if(pageTable[virtualPageNumber].validTranslation){
            returnValue.hits++;
            
            //get index of the pageEntry corresponding to the virtual page number in the vector(aka memory)
            int index = pageFrameIndexes[pageEntryAddress] ;
            //sets the page entry's second chance bit to true since it there was a hit
            pageFrames[index].second = true;
            
            }
        //this else runs if there was a page fault
        else{
          
            returnValue.misses++;
            //sets the valid translation to true to represent that we are loading the page into memory
            pageTable[virtualPageNumber].validTranslation = true;
            
            //if the max number of page frames are in memory , then a page needs to be evicted
            if(numFrames == (int)pageFrames.size()){
                
                //moves the clock hand along the vector until there clock hand points to an entry with a false second chance bit
                while(true){
                    //touches the entry in the vector with the clock hand and checks it's second chance bit
                    secondChance = pageFrames[clockHand].second;
                    if(!secondChance){
                        break;
                    }
                    //sets second chance bit of entry touched by clock hand to false
                    pageFrames[clockHand].second = false;
                    /*moves up clock hand( the expression is used to loop the vector around once clock hand hits
                    the max number of frames) */
                    clockHand = (clockHand+1)%numFrames;
                }
                

                
                //gets evicted page as the last entry in pageFrames vector touched by the clock hand
                std::pair<pageTableEntry*,bool > evicted = pageFrames[clockHand];
                /*sets the translation of evicted value to false and erases it from indexes map as it
                is no longer in memory
                */
                pageFrames[clockHand].first->validTranslation = false;
                pageFrameIndexes.erase(evicted.first);
                //loads new page in memory with second chance bit set to true
                pageFrames[clockHand] = std::make_pair(pageEntryAddress,true);
                //the index of the new page in the vector is equal to wherever the clock hand ended
                pageFrameIndexes[pageEntryAddress] = clockHand;
               



            }
            //this runs if there's still space in the memory for another page to be added
            else{
                //loads new page into memory with a second chance bit equal to true
                pageFrames.push_back(std::make_pair(pageEntryAddress,true));
                
                pageFrameIndexes[pageEntryAddress] = pageFrames.size()-1;

            }
           


        }
        /*
         for(auto &h : pageTable){
                if(h.second.validTranslation){
                std::cout << " address of " << h.first*32 << " is at " << &(h.second) << std::endl;
                }

            }
             for(int i = 0; i < (int)pageFrames.size(); i++){
                    std::cout << " page frames contains " << pageFrames[i].first << " at " << i << "valid bit " << pageFrames[i].second << std::endl;
                }
                std::cout << "hand is at " << clockHand << std::endl;
            std::cout << std::endl;
            */
        //the physical page number is whatever index the page is in the pageFrames vector
        physicalPageNumber = pageFrameIndexes[pageEntryAddress];
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);
        }

    return returnValue;
}


//used to return the furthest pageTableEntry or the one not used at all in the future
//page frames represents physical memory, index represent the current position
simulator::pageTableEntry* simulator::getFarthest(int index, 
    std::unordered_map<int,pageTableEntry> &pageTable, std::unordered_map<pageTableEntry*,int> &pageFrames){
    bool usedInfuture;
      pageTableEntry* furthestUsed;
      //moves the index up so that it starts at the first future index.
       index++;
       //stores index to furthest entry
       int furthest = -1;
       //goes through every frame in memory
      for(auto &frame : pageFrames){
          usedInfuture = false;
        
          pageTableEntry* currentFrame = frame.first;
        //starts at the first future index and goes until the of the  requests vector
          for(int starting = index; starting < (int)requests.size(); starting++){
            int virtualPageNumber = ((requests[starting]) & 0XE0) >> 5;
            /*sees if the page requested in the future is currently stored in the page table,
            if not this request can be skipped since it doesn't correspond to any frame in memory
            */
            auto pageEntry= pageTable.find(virtualPageNumber);
            if(pageEntry == pageTable.end()){
                continue;

            }
            //if the page currently used in memory has an invalid translation, then it's also not in memory
            //and can be skipped
            else if(!pageEntry->second.validTranslation){
                continue;
            }
            //gets address of page table entry from the page table
            pageTableEntry* pageEntryAddress = &(pageEntry->second);
            
            /*if the page entry address is the same as the current frame,
            this means that page is used in the future
            */
            if(pageEntryAddress == currentFrame ){
                /*stores the furthest used page as the one with the largest index
                with regards with when it's used in the future*/
                if(starting > furthest){
                furthest = starting;
                furthestUsed = currentFrame;
                }
               //breaks the loop since we already found the next time the page is used in the future
                usedInfuture = true;
                break;
                
            }
        }
          //if the page is never used in the future, it can be returned instantly to be evicted
          if(!usedInfuture){
            
            return currentFrame;

          }
         
        }
    
    return furthestUsed;
   }
