#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   
  std::vector<Process>& Processes();  
  float MemoryUtilization();          
  long UpTime();                      
  int TotalProcesses();               
  int RunningProcesses();             
  std::string Kernel();               
  std::string OperatingSystem();      

  //constructor
  System() : processes_() {}
  
  // Update all the system memebers
  void UpdateSystem();

 private:
  Processor cpu_;
  std::vector<Process> processes_;
  std::string kernel_;
  std::string oprating_system_;
  long uptime_;
  int total_processes_;
  int running_processes_;
  float memory_utilization_;
  
  //update processes /insert/modify/delete/sort
  void UpdateProcesses ();
  //update members of System class beside the processes and cpu
  void UpdateOtherSystemMembers();
};

#endif