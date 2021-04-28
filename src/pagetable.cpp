#include "pagetable.h"
#include "math.h"
#include "algorithm"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

int PageTable::getPageSize(){
    return _page_size;
}

std::map<std::string, int> PageTable::getTable(){
    return _table;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    std::map<std::string, int>::iterator it = _table.begin();

    int frame = 0; 
    // Find free frame
    while(it != _table.end()){
        int value = it->second;
        if(value == frame){
            frame++;
            it = _table.begin();
        }
        else{
            it++;
        }
    }
    
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;

    int numBits = log2(_page_size);
    for(int i = 0; i <= numBits; i++){
        page_offset += 1*(2^i);
    }
    page_number = virtual_address >> numBits; 

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    std::map<std::string, int>::iterator it;
    if (_table.count(entry) > 0)
    {
        it = PageTable::_table.find(entry);
        if(it != _table.end()){
            address = (_page_size*PageTable::_table.at(entry)) + page_offset;
        }
        else{
            printf("entry not found in table");
        }
    }
    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        // TODO: print all pages
    }
}