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
        
        int virtualPageNumber = ((request) & 0XE0) >> 5;
        int pageOffset = ((request))& 0x1F;

        pageTableEntry* pageEntryPointer = &pageTable[virtualPageNumber];

        int physicalPageNumber;

       
        if(!pageTable[virtualPageNumber].validTranslation){
           
            pageTable[virtualPageNumber].validTranslation = true;
           
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
            
            Frames.push_back(pageEntryPointer);
           
            pageFramesIterators[pageEntryPointer] = std::prev(Frames.end());

            /*sets the physical page number as the distance from the iterator at the beginning of the queue 
            to the iterator where the page entry actually is.*/
            physicalPageNumber = std::distance(Frames.begin(),pageFramesIterators[pageEntryPointer]);
            
        }
        else{
            //physical page number of a page in the queue is difference between beginning iterator and iterator pointing to the page
            physicalPageNumber = std::distance(Frames.begin(), pageFramesIterators[pageEntryPointer]);
            returnValue.hits++;
        }
        //calculates physical address as physicalPageNumber*32+ pageOffset because the page sizes are 32.
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);

        }
    return returnValue;

}

simulator::results simulator::LRU(){
    results returnValue;
	//stores virtual page numbers in the virtual memory
    std::list<int> pageFrames;
    
	//maps virtual page numbers to iterator in linked list and pageTableEntry
    std::unordered_map<int,std::pair<std::list<int>::iterator,pageTableEntry>> pageTable;
    //std::cout << "size of a pageTablEntry" << sizeof(pageTableEntry) << std::endl;
    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
        int pageOffset = ((request))& 0x1F;
        
        int physicalPageNumber;
       // std::cout << " this is the request " << request << " this is the valid bit " << pageTable[virtualPageNumber].validTranslation << " this is the page number" << virtualPageNumber << std::endl;
        if(pageTable[virtualPageNumber].second.validTranslation == 0){
            pageTable[virtualPageNumber].second.validTranslation = 1;
            
            returnValue.misses++;
            if((int)pageFrames.size() == numFrames){
               // std::cout << " evicting a page " << std::endl;
                int evicted = pageFrames.back();
                pageTable[evicted].second.validTranslation = 0;
                pageFrames.pop_back();
                pageTable.erase(evicted);

            }
            //::cout << " placing new page into memory " << std::endl;
            pageFrames.push_front(virtualPageNumber);
            pageTable[virtualPageNumber].first = pageFrames.begin();
            physicalPageNumber = 0;



        }
        else{
           // std::cout << "updating existing entry " << std::endl;
           physicalPageNumber = std::distance(pageFrames.begin(), pageTable[virtualPageNumber].first);
            pageFrames.erase(pageTable[virtualPageNumber].first);
            pageFrames.push_front(virtualPageNumber);
            pageTable[virtualPageNumber].first = pageFrames.begin();

            returnValue.hits++;



        }
        
        //std::cout << "physical page number is " << physicalPageNumber << std::endl;
        
        returnValue.addresses.push_back(physicalPageNumber*32+pageOffset);
        }
        
        return returnValue;

}


simulator::results simulator::Optimal(){
    results returnValue;
    //maps pageTableEntry pointers to physical page numbers. 
    //uses pointers so that we can edit the page table entries in pageTable map when pageEntries are evicted
    std::unordered_map<pageTableEntry*,int> pageFrames;
    //maps virtual page numbers to page table entries
    std::unordered_map<int,pageTableEntry> pageTable;
  
    for(int i = 0 ; i < (int)requests.size() ; i++){
        int request = requests[i];
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
        pageTableEntry* pageEntryPointer = &pageTable[virtualPageNumber];
        int physicalPageNumber;
       
       
        if(pageTable[virtualPageNumber].validTranslation){
           returnValue.hits++;

            }
       
        else{
         
            returnValue.misses++;
            
            pageTable[virtualPageNumber].validTranslation = true;
            
            //if the map contains the maximum of page frames in memory, the page used furthest in future
            //or not at all needs to be evicted.
            if(numFrames == (int)pageFrames.size()){
                
                //the i represents the index of the current page request, so all indexes after are in the future
                pageTableEntry* evicted = getFarthest(i,pageTable,pageFrames);
                evicted->validTranslation = false;
                //gets physical page number of evicted page
                int pageFrameIndex = pageFrames[evicted];
                
                pageFrames.erase(evicted);
                //replaces page at evicted physical page number with new page
                pageFrames[pageEntryPointer] = pageFrameIndex;

                }
            //this else runs if there's a page frame open in memory
            else{
               //sets physical page number of page entry to be the first open frame.
                pageFrames[pageEntryPointer] = pageFrames.size();
                }
          
        }
        physicalPageNumber = pageFrames[pageEntryPointer];
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
    //maps virtual page numbers to page table entries
    std::unordered_map<int, pageTableEntry> pageTable;
    /*maps page table entries to physical page numbers(pointer are used so that entries evicted from pageFrames
    can be erased from this map by value.)
    */
    std::unordered_map<pageTableEntry*,int> pageFrameIndexes;
	//clock hand points to the last page added to memory
    int clockHand = 0;
   

    for(const auto &request : requests){
        int virtualPageNumber = ((request) & 0XE0) >> 5;
       
        int pageOffset = ((request))& 0x1F;
		//gets the address to the pageTableEntry for this virtual number so that we can create/access to pointer to it for pageFrames
        pageTableEntry* pageEntryPointer = &(pageTable[virtualPageNumber]);
        int physicalPageNumber;
        
        if(pageTable[virtualPageNumber].validTranslation){
            returnValue.hits++;
            
            
            int index = pageFrameIndexes[pageEntryPointer] ;
            //sets the page entry's second chance bit to true since it there was a hit
            pageFrames[index].second = true;
            
            }
        
        else{
          
            returnValue.misses++;
            
            pageTable[virtualPageNumber].validTranslation = true;
            
            //if the max number of page frames are in memory , then a page needs to be evicted
            if(numFrames == (int)pageFrames.size()){
                
                //moves the clock hand along the vector until the clock hand points to an page frame in memory with a false second chance bit
                while(true){
                    
                    
                    if(!pageFrames[clockHand].second){
                        break;
                    }
                
                    pageFrames[clockHand].second = false;
                    /*moves the clock hand to next frame in memory( the expression is used to loop the vector around once clock hand hits
                    the max number of frames) */
                    clockHand = (clockHand+1)%numFrames;
                }
                

                
                //evicted page is whatever page the the clock hand is pointing to 
                std::pair<pageTableEntry*,bool > evicted = pageFrames[clockHand];
               
                pageFrames[clockHand].first->validTranslation = false;
                pageFrameIndexes.erase(evicted.first);
                //replaces evicted page with new page loaded from memory
                pageFrames[clockHand] = std::make_pair(pageEntryPointer,true);
               
                pageFrameIndexes[pageEntryPointer] = clockHand;
               



            }
            //this runs if there's still space in the memory for another page to be added
            else{
                //loads new page into memory with a second chance bit equal to true
                pageFrames.push_back(std::make_pair(pageEntryPointer,true));
                
                pageFrameIndexes[pageEntryPointer] = pageFrames.size()-1;

            }
          }

        physicalPageNumber = pageFrameIndexes[pageEntryPointer];
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
      
       int furthest = -1;
     
      for(auto &frame : pageFrames){
          usedInfuture = false;
        
          pageTableEntry* currentFrame = frame.first;
        
          for(int starting = index; starting < (int)requests.size(); starting++){
            int virtualPageNumber = ((requests[starting]) & 0XE0) >> 5;
            /*sees if the page requested in the future is currently stored in the page table,
            if not this request can be skipped since it doesn't correspond to any frame in memory
            */
           /*keep in mind that pageEntry->first is a virtual page number and that pageEntry->second is the 
           page table entry that maps to that virtual page number
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
        
            pageTableEntry* pageEntryPointer = &(pageEntry->second);
            
            /*if the page entry address is the same as the current frame,
            this means that page is used in the future
            */
            if(pageEntryPointer == currentFrame ){
                
                if(starting > furthest){
                furthest = starting;
                furthestUsed = currentFrame;
                }
               //breaks the loop since we already found the next time the page is used in the future
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
