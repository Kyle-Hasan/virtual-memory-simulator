#include <unordered_map>
#include <vector>
#include <deque>
#include <list>
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


    private:
    std::vector<int> requests;
    int numFrames;
    std::list<pageTableEntry*>::iterator getFarthest(int index, std::list<pageTableEntry*> &pageFrames, 
    std::unordered_map<int,pageTableEntry> &pageTable,std::unordered_map<pageTableEntry*,std::list<pageTableEntry*>::iterator> &pageFramesIterators);
    
    
};


