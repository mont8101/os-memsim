#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool print);
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
                check = Char;        // Handle command
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
            allocateVariable(std::stoi(strings.at(1)), strings.at(2), check, std::stoi(strings.at(4)), mmu, page_table, true);
        }
        else if(strings.at(0) == "set"){
            std::vector<std::string> list;
            for(int i = 4; i < strings.size(); i++){
                list.push_back(strings.at(i));
            }
            int listSize = list.size();
            setVariable(std::stoi(strings.at(1)), strings.at(2), std::stoi(strings.at(3)), &list, mmu, page_table, memory);
        }
        else if(strings.at(0) == "free"){
            freeVariable(std::stoi(strings.at(1)), strings.at(2), mmu, page_table);
        }
        else if(strings.at(0) == "terminate"){
            terminateProcess(std::stoi(strings.at(1)), mmu, page_table);
        }
        else if(strings.at(0) == "print"){
            if(strings.at(1) == "page"){
                page_table->print();
            }
            else if(strings.at(1) == "mmu"){
                mmu->print();
            }
            else if(strings.at(1) == "processes"){
                for(int i = 0; i < mmu->getProcess().size(); i++){
                     printf("%d\n", mmu->getProcess().at(i)->pid);   
                }
            }
        }
        else{
            printf("error: command not recognized\n");
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
    allocateVariable(currentPid, "<TEXT>", Char, text_size, mmu, page_table, false);//call allocatevar
    allocateVariable(currentPid, "<GLOBALS>", Char, data_size, mmu, page_table, false);//call allocatevar
    allocateVariable(currentPid, "<STACK>", Char, 65536, mmu, page_table, false);//call allocatevar
    printf("%i\n", currentPid);

    
    //figure out way to make allocate not print at same time
    //   - print pid
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool print)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    //int lastPage = page_table->getTable()[page_table->getTable().size() -1];
    bool fail = true;
    int byteSize = getDataTypeBytes(type);
    int totalSize = getDataTypeBytes(type) * num_elements;
    int pSize = page_table->getPageSize();
    Process *currentProcess; 
    Variable *freeSpace;
    Variable *creating = new Variable();
    creating->name = var_name;
    creating->type = type;
    creating->size = totalSize;
    bool pidFound = false;
    
    for(int i = 0; i < mmu->getProcess().size(); i++){
        if(mmu->getPID(i) == pid){
            currentProcess = mmu->getProcess().at(i);
            pidFound = true;
            break;
        }
    }
    if(!pidFound){
        printf("%s\n", "error: process not found");
        return;
    }
    for(int i = 0; i < currentProcess->variables.size(); i++){
        if(var_name == currentProcess->variables.at(i)->name){
            printf("%s\n", "error: variable already exists");
            return;
        }
    }
    std::vector<Variable*>::iterator it;
    int j = 0;
    for(it = currentProcess->variables.begin(); it != currentProcess->variables.end(); it++){
        if(currentProcess->variables.at(j)->name == "<FREE_SPACE>" && currentProcess->variables.at(j)->size >= totalSize){
            freeSpace = currentProcess->variables.at(j);
            if(freeSpace->virtual_address + totalSize <=  67108864){//check if allocation excedes memory
                int remainingSpace = (pSize - (freeSpace->virtual_address%pSize))%byteSize; 
                int firstPageTest = (int) freeSpace->virtual_address/pSize;
                int lastPageTest = (int)(freeSpace->virtual_address + creating->size - 1)/pSize;
                if(remainingSpace != 0 && firstPageTest != lastPageTest){
                    int freeSpaceSize = freeSpace->size - remainingSpace - totalSize; //new small free space size
                    Variable *addFreeSpace = new Variable(); //new var for new free space after added var
                    addFreeSpace->name = "<FREE_SPACE>";
                    addFreeSpace->size = freeSpaceSize;
                    freeSpace->size = remainingSpace; //new size for small free space
                    creating->virtual_address = freeSpace->virtual_address + remainingSpace; //add new var in correct spot
                    addFreeSpace->virtual_address = creating->virtual_address + creating->size; //add free space address after var
                    currentProcess->variables.push_back(addFreeSpace); //add new free space to vector
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
                int firstPage = (int) creating->virtual_address/pSize;
                int lastPage = (int)(creating->virtual_address + creating->size - 1)/pSize;
                std::string key = std::to_string(pid) + "|" + std::to_string(firstPage);
                
                for(int i = firstPage; i <= lastPage; i++){
                    std::string key = std::to_string(pid) + "|" + std::to_string(i);
                    if(page_table->getTable().count(key) != 1){
                        page_table->addEntry(pid, i);
                    }
                }
                fail = false;
                //printf("name: %s*\n", creating->name.c_str());
                //printf("size: %d\n", creating->size);
                //printf("virtual address: %d\n", creating->virtual_address);
                //std::cout << it << std::endl;
                currentProcess->variables.insert(it, creating);
                break;
            }
            
        }
        j++;
    }
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address 
    if(print && fail == false){
        printf("%i\n", creating->virtual_address);
    } 
    else if(print && fail == true){
        printf("%s\n", "error: allocation will exceed system memory");
    }

}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    Process *currentProcess; 
    uint32_t virtualAddress;
    bool varFound = false;
    bool pidFound = false;
    int varByteSize = 0;
    char * test = (char *)value; //have absolutly no idea if this works correctly
    std::string * memtest = (std::string *)memory;
    for(int i = 0; i < mmu->getProcess().size(); i++){
        if(mmu->getPID(i) == pid){
            pidFound = true;
            currentProcess = mmu->getProcess().at(i);
            break;
        }
    }

    for(int j = 0; j < currentProcess->variables.size(); j++){
        if(currentProcess->variables.at(j)->name == var_name){
            varFound = true;
            virtualAddress = currentProcess->variables.at(j)->virtual_address;
            varByteSize = getDataTypeBytes(currentProcess->variables.at(j)->type);
        }
    }
    if(pidFound){
        if(varFound){//   - look up physical address for variable based on its virtual address / offset
            int physAddress = page_table->getPhysicalAddress(pid, virtualAddress) + offset;
            
            //   - insert `value` into `memory` at physical address
            //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
            //           multiple elements of an array) 
            //for(int i = 0; i < size; i++){
                //memory[physAddress] = value;//have absolutly no idea if this works correctly
                //physAddress += varByteSize;//have absolutly no idea if this works correctly
                //test++;//have absolutly no idea if this works correctly
           // }
        }
        else{
            printf("%s\n", "error: variable not found");
        }
    }
    else{
        printf("%s\n", "error: process not found");
    }
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    bool pidFound = false;
    bool varFound = false;
    Process *currentProcess; 
    std::vector<Variable*>::iterator it;
    for(int i = 0; i < mmu->getProcess().size(); i++){
        if(mmu->getPID(i) == pid){
            pidFound = true;
            currentProcess = mmu->getProcess().at(i);
            break;
        }
    }
    if(!pidFound){
        printf("error: process not found\n");
        return;
    }
    int i = 0;
    for(it = currentProcess->variables.begin(); it != currentProcess->variables.end(); it++){
        if(currentProcess->variables.at(i)->name == var_name){
            varFound = true;
            if(i != 0 && currentProcess->variables.at(i - 1)->name == "<FREE_SPACE>" && currentProcess->variables.at(i + 1)->name == "<FREE_SPACE>"){ //if current var is between 2 free spaces
                    currentProcess->variables.at(i - 1)->size = currentProcess->variables.at(i - 1)->size + currentProcess->variables.at(i)->size + currentProcess->variables.at(i + 1)->size; //add all 3 together
                    currentProcess->variables.erase(it);
                    currentProcess->variables.erase(it);
                    //printf("varSize: %li\n", currentProcess->variables.size());
                    break;
                    //currentProcess->variables.erase(it); //since first one was erased dont have to do it + 1
            }
            else if(i != 0 && currentProcess->variables.at(i - 1)->name == "<FREE_SPACE>"){//if free space before
                    currentProcess->variables.at(i - 1)->size = currentProcess->variables.at(i - 1)->size + currentProcess->variables.at(i)->size;
                    currentProcess->variables.erase(it);
                    break;
            }
            else if(currentProcess->variables.at(i + 1)->name == "<FREE_SPACE>"){ //if free space after
                    currentProcess->variables.at(i)->size = currentProcess->variables.at(i + 1)->size + currentProcess->variables.at(i)->size;
                    currentProcess->variables.at(i)->name = "<FREE_SPACE>"; //rename curr process to free space
                    currentProcess->variables.erase(it + 1); //erase next free space
                    break;
            }
            else{ //no free space surrounding
                    currentProcess->variables.at(i)->name = "<FREE_SPACE>"; //rename curr process to free space
                    break;
            }
        }
        i++;
    }
    if(!varFound){
        printf("error: variable not found\n");
        return;
    }
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    mmu->terminate(pid);
    
    page_table->freePIDPage(pid);

    /*bool pidFound = false;
    
    for(int i = 0; i < mmu->getProcess().size(); i ++ ){
        if(mmu->getPID(i) == pid){
            pidFound = true;
            mmu->getProcess().erase(mmu->getProcess().begin() + i);
            break;
        }
        i++;
    }
    if(!pidFound){
        printf("error: process not found\n");
        return;
    }*/

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