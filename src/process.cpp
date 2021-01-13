#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const{ 
    return pid_;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
    return cpu_utilization_;
}

// Return the command that generated this process

string Process::Command() const { 
    return command_; 
}

// Return this process's memory utilization
string Process::Ram() const{ 
    return ram_; 
}

// Return the user (name) that generated this process
string Process::User() const{ 
    return user_; 
}

// Return the age of this process (in seconds)
long int Process::UpTime() const{ 
    return uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    //return  (cpu_utilization_ < a.cpu_utilization_);
    return  (a.cpu_utilization_ < cpu_utilization_);
}

// Overload the "greater than" comparison operator for Process objects
bool Process::operator>(Process const& a) const { 
    return  (cpu_utilization_ > a.cpu_utilization_);
}

bool Process::updateProcess(){
  return updateProcess(pid_);
}

// calculate cpu utilization using following formula: 
// (new active jiffies[pid]- prev active jiffies[pid])/ (new total jiffies - prev total jiffies)
bool Process::calcUtil() {
  bool success = false;
  float ret;
  //preventing divide by zero situations
  if ((new_pv_aj_ - prev_pv_aj_) != 0) {
    ret = float(new_pv_ajp_ - prev_pv_ajp_) /float(new_pv_aj_ - prev_pv_aj_);
    //make sure calculation is not out of range
    if ((ret >= 0.0) && (ret <= 1.0)) {
      cpu_utilization_ = ret;
      success = true;
    }
  }
  return success; 
}

//Update process, exclude non existing processes
bool Process::updateProcess(int pid) {
  bool pid_exist = false;
  //make sure the process is still valid
  if (LinuxParser::isExist(pid)) {
    prev_pv_ajp_ = new_pv_ajp_;
    prev_pv_aj_ = new_pv_aj_;
    new_pv_ajp_ = LinuxParser::ActiveJiffies(pid);
    new_pv_aj_ = LinuxParser::Jiffies();
    pid_ = pid;
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid);
    user_ = LinuxParser::User(pid);
    calcUtil(); // update cpu utilization
    uptime_ = LinuxParser::UpTime(pid);
    exist_ = true;
    //return success
    pid_exist = true;
  } else {
    exist_ = false;
  }
  return pid_exist;
}
