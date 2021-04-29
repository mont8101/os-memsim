#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
std::vector<std::string> splitCommand(std::string command);
int getDataTypeBytes(DataType type);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    //check for power of 2 num & (num -1) == 0

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
        // TODO: implement this!
        std::vector<std::string> strings;
        strings = splitCommand(command);

        
        if(strings.at(0) == "create"){
            //void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
            createProcess(std::stoi(strings.at(1)), std::stoi(strings.at(2)), mmu, page_table); 
        }
        else if(strings.at(0) == "allocate"){
            DataType check;//enum DataType Char, Short, Int, Float, Long, Double
            if(strings.at(3) == "char"){
                check = Char;
            }
            else if(strings.at(3) == "short"){
                check = Short;
            }
            else if(strings.at(3) == "int"){
                check = Int;
            }
            else if(strings.at(3) == "float"){
                check = Float;
            }
            else if(strings.at(3) == "long"){
                check = Long;
            }
            else if(strings.at(3) == "double"){
                check = Double;
            }
            //void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
            //std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
            allocateVariable(std::stoi(strings.at(1)), strings.at(2), check, std::stoi(strings.at(4)), mmu, page_table);
        }
        else if(strings.at(0) == "set"){
            
        }
        else if(strings.at(0) == "free"){
            
        }
        else if(strings.at(0) == "terminate"){
            
        }
        else if(strings.at(0) == "print"){
            //test if break
        }
        else{
            printf("command doesn't exist");
            for(int i = 0; i < strings.size(); i++){
                std::cout << strings.at(i) << std::endl;
            }
        }
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    uint32_t currentPid = mmu->createProcess();
    
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    //void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    mmu->addVariableToProcess(currentPid, "<TEXT>", Int, text_size, 0);
    mmu->addVariableToProcess(currentPid, "<GLOBALS>", Int, data_size, text_size);
    mmu->addVariableToProcess(currentPid, "<STACK>", Int, 65536, text_size+data_size);
    printf("%i\n", currentPid);
    //   - print pid
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    //int lastPage = page_table->getTable()[page_table->getTable().size() -1];
    
    int byteSize = getDataTypeBytes(type);
    int totalSize = getDataTypeBytes(type) * num_elements;
    int pSize = page_table->getPageSize();
    Process *currentProcess; 
    Variable *freeSpace;
    Variable *creating = new Variable();
    creating->name = var_name;
    creating->type = type;
    creating->size = totalSize;
    
    for(int i = 0; i < mmu->getProcess().size(); i++){
        if(mmu->getPID(i) == pid){
            currentProcess = mmu->getProcess().at(i);
            break;
        }
    }
    
    for(int j = 0; j < currentProcess->variables.size(); j++){
        if(currentProcess->variables.at(j)->name == "<FREE_SPACE>" && currentProcess->variables.at(j)->size >= totalSize){
            freeSpace = currentProcess->variables.at(j);
            std::vector<Variable*>::iterator it;
            it = mmu->getVars(currentProcess).begin() + j;
            int remainingSpace = (freeSpace->virtual_address%pSize)%byteSize;
            if(remainingSpace != 0){
                int freeSpaceSize = freeSpace->size - remainingSpace - totalSize; //new small free space size
                Variable *addFreeSpace = new Variable(); //new var for new free space after added var
                addFreeSpace->name = "<FREE_SPACE>";
                addFreeSpace->size = freeSpaceSize;
                freeSpace->size = remainingSpace; //new size for small free space
                creating->virtual_address = freeSpace->virtual_address + remainingSpace; //add new var in correct spot
                addFreeSpace->virtual_address = creating->virtual_address + creating->size; //add free space address after var
                currentProcess->variables.emplace_back(addFreeSpace); //add new free space to vector
            }
            else{
                creating->virtual_address = freeSpace->virtual_address;
                freeSpace->virtual_address = freeSpace->virtual_address + totalSize;
                if(freeSpace->size - totalSize == 0){
                    //remove freeSpace 
                    currentProcess->variables.erase(it);
                }

                else{
                    freeSpace->size = freeSpace->size - totalSize;
                }
            }
            mmu->getVars(currentProcess).emplace(it, creating);
            break;
        }
    }
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address 
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array) 
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}

std::vector<std::string> splitCommand(std::string command){
    std::vector<std::string> strings;
    char *word = strdup((char *)command.c_str());
    char *test = strtok(word, " ");
    while(test != NULL){
        strings.push_back(test);
        test = strtok(NULL, " ");
    }
    free(word);
    return strings;
}

int getDataTypeBytes(DataType type){
    if(type == Char){
        return 1;
    }
    else if(type == Short){
        return 2;
    }
    else if(type == Int || type == Float){
        return 4;
    }
    else{
        return 8;
    }
}