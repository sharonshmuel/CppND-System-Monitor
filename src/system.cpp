#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// Return the system's kernel identifier (string)
std::string System::Kernel() { 
  return kernel_;
}

// Return the system's memory utilization
float System::MemoryUtilization() { 
  return memory_utilization_; 
}

// Return the operating system name
std::string System::OperatingSystem() { 
  return  oprating_system_;
}

// Return the number of processes actively running on the system
int System::RunningProcesses() { 
  return running_processes_; 
}

// Return the total number of processes on the system
int System::TotalProcesses() { 
  return total_processes_; 
}

// Return the number of seconds since the system started running
long int System::UpTime() { 
  return uptime_; 
}

//update members of System class beside the processes and cpu
void System::UpdateOtherSystemMembers() {
  kernel_ = LinuxParser::Kernel();
  oprating_system_ = LinuxParser::OperatingSystem();
  uptime_ = LinuxParser::UpTime();
  memory_utilization_ = LinuxParser::MemoryUtilization();
}

//used for existing processes searching
struct PidClassComp
{
  explicit PidClassComp(int i) : pid(i) { }
  inline bool operator()(const Process & m) const { return m.Pid() == pid; }
private:
  int pid;
};

//update processes /insert/modify/delete/sort
void System::UpdateProcesses () {
  std::vector<int> pids = LinuxParser::Pids();
  //inserting new processes/updating existing processes
  for(int pid : pids) { 
    std::vector<Process>::iterator it;
    it = std::find_if(processes_.begin(), processes_.end(), PidClassComp(pid));
    if(it != processes_.end()) {
      it->updateProcess(pid);
    } else {
      //filter out pid without command name
      if (LinuxParser::Command(pid)!=""){
        Process p;
        if (p.updateProcess(pid))
          processes_.push_back(p);
      }
    }
  }
  //marking processes for delete
  for(Process p : processes_) {
    std::vector<int>::iterator iit; 
    iit = std::find(pids.begin(), pids.end(),p.Pid());
    if(iit == pids.end()) {
      p.notExist();
    }
  }
  //deleting nonexisting processes  
  for(long unsigned int i=0; i< processes_.size();i++) {
    if ((&processes_[i]!=NULL) && (!processes_[i].isExist()))
      processes_.erase(processes_.begin() + i);
  }

  std::sort (processes_.begin(), processes_.end());
  running_processes_ = LinuxParser::RunningProcesses();
  total_processes_ = LinuxParser::TotalProcesses();
}

void System::UpdateSystem() {
  this->UpdateProcesses();
  this->Cpu().updateProcessor();
  UpdateOtherSystemMembers();
}
