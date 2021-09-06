#include <unordered_map>
#include <vector>
#include <deque>
#include <list>
#include <utility>
#include <iostream>
#include <iterator>   
class simulator {
    

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
    std::vector<int> requests;
    int numFrames;
    pageTableEntry* getFarthest(int index, 
    std::unordered_map<int,pageTableEntry> &pageTable, std::unordered_map<pageTableEntry*,int> &pageFrames);

    
    
};


