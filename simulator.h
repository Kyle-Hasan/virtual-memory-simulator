#include <unordered_map>
#include <vector>
#include <deque>
#include <list>
#include <utility>
#include <iostream>
#include <iterator>   
class simulator {
    
     //Addresses are 8 bits. First 3 bits are the page number, last 5 bits are the page offset.
    public:
    struct pageTableEntry{
        bool validTranslation = false;

        }typedef pageTableEntry;

    struct results{
        int misses = 0;
        int hits = 0;
        std::vector<int> addresses;
       

    }typedef results;
    simulator(std::vector<int>& requests, int& numFrames);
    
    results LRU();
    results FIFO();
    results Optimal();
    results Clock();

    private:
    //represents all the address requests made by a process to the OS.
    std::vector<int> requests;
    //stores max number of page frames in physical memory
    int numFrames;
    //gets page used furthest in the future or a page never used in the future
    pageTableEntry* getFarthest(int index, 
    std::unordered_map<int,pageTableEntry> &pageTable, std::unordered_map<pageTableEntry*,int> &pageFrames);

    
    
};


