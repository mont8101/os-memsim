#include "mmu.h"
#include <string>
#include <sstream>

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::getPID(int index){
    return _processes[index]->pid;
}

std::vector<Process*> Mmu::getProcess(){
    return _processes;
}

std::vector<Variable*> Mmu::getVars(Process *process){
    return process->variables;
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
            int pid = _processes[i]->pid;
            std::string varName = _processes[i]->variables[j]->name;
            std::string virtualAddress = intToHex(_processes[i]->variables[j]->virtual_address);
            int size = _processes[i]->variables[j]->size;

            if(varName != "<FREE_SPACE>"){
                std::cout << " " << pid << " | " << varName << getSpaces(varName.length(), 13) << " |   0x" << virtualAddress << " | " << getSpaces(virtualAddress.length(), 10) << size << " " << std::endl;
            }
        }
    }
}

std::string Mmu::getSpaces(int string, int maxSize){
    int stringSize = maxSize - string;
    std::string result; 
    for(int i = 0; i < stringSize; i++){
        result = result + " ";
    }
    return result;
}

std::string Mmu::intToHex(int num){
    std::string result;
    std::stringstream ss;
    ss << std::hex <<num;
    ss >> result;
    return result;
}