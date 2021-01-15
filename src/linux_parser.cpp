#ifndef MODIFIED_UDACITY_CODE
#include <dirent.h>
#else
#include <experimental/filesystem>
using namespace std;
namespace fs = std::experimental::filesystem;
#endif
#include <unistd.h>
#include <string>
#include <vector>
#include <regex>
#include "linux_parser.h"
#include <cmath>


using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;
using LinuxParser::CPUStates;
using LinuxParser::ProcessStates;

// Information about the operating system exists outside of the /proc directory, in the /etc/os-release file.
// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Information about the kernel exists /proc/version file.
// DONE: An example of how to read data from the filesystem (fixed to return correct version)
string LinuxParser::Kernel() {
  string os, kernel, version_string;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    //added dummy version_string, so we can get correct kernel version.
    linestream >> os >> version_string >> kernel;
  }
  return kernel;
}


#ifndef MODIFIED_UDACITY_CODE
// DONE: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}
#else
// DONE updates using std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  const fs::path proc_path = kProcDirectory;
  if (fs::exists(proc_path) && fs::is_directory(proc_path))
	{
     for (const auto& entry : fs::directory_iterator(proc_path))
        {
            auto filename = entry.path().filename();
            if (fs::is_directory(entry.status()))
            {
              //take only folders containing only digits
              if (filename.native().find_first_not_of("0123456789") == std::string::npos) {
                int pid = stoi(filename);
                pids.push_back(pid);
              }
            }
        }  
  }
  return pids;
}

#endif

// calculate memory utilization in bytes
unsigned int claculateSize (std::smatch sm) {
  float memsize;
  if (sm[2]=="kB")
    memsize = stof(sm[1])*1024;
  else if (sm[2]=="mB")
    memsize = stof(sm[1])*pow(1024,2);
  else if (sm[2]=="gB")
    memsize = stof(sm[1])*pow(1024,3);
  else if (sm[2]=="B")
    memsize = stof(sm[1]);
  return memsize; 
}

// Information about memory utilization exists in the /proc/meminfo file.
// The formula for calculating memory usage in percentage is as follow:
// Total Physical Memory - (Memory Free + Memory Buffers + Cache Memory) = Result/Total Physical Memory 
// MemAvailable = Memory Buffers + Cache Memory
// DONE: Read and return the system memory utilization in %
float LinuxParser::MemoryUtilization() { 
  float memoryutilization;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  float memtotal, memfree, memavailable;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::smatch sm;
    std::regex str_expr (R"(MemTotal:\s+(\d+)\s+(B|kB|mB|gB))");
    std::regex_match(line,sm,str_expr);
    memtotal = claculateSize(sm);
    std::getline(stream, line);
    std::regex str_expr2 (R"(MemFree:\s+(\d+)\s+(B|kB|mB|gB))");
    std::regex_match(line,sm,str_expr2);
    memfree = claculateSize(sm); 
    std::regex str_expr3 (R"(MemAvailable:\s+(\d+)\s+(B|kB|mB|gB))");
    std::regex_match(line,sm,str_expr3);
    memavailable = claculateSize(sm);
    stream.close();
  }
  memoryutilization = (memtotal - (memfree + memavailable))/memtotal;
  //prevent out of range results to ocour
  if (memoryutilization < 0.0) 
    memoryutilization = 0.0;
  return memoryutilization;
}

// Information about system up time exists in the /proc/uptime file.
// This file contains two numbers (values in seconds): the
// uptime of the system (including time spent in suspend) and
// the amount of time spent in the idle process.
// DONE: Read and return the system uptime - s
long LinuxParser::UpTime() { 
  long uptime;
  string line;
  float up, idle;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up >> idle;
  }
  uptime = long(up);
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> js = CpuUtilization();

  //The  amount  of  time, measured in units of USER_HZ
  long user       = stoi(js[kUser_]); // Time spent in user mode
  long nice       = stoi(js[kNice_]); // Time spent in user mode with low priority (nice).
  long system     = stoi(js[kSystem_]); // Time spent in system mode.
  long idle       = stoi(js[kIdle_]); // Time spent in the idle task. This value should be USER_HZ times the second entry in the /proc/uptime pseudo-file.
  long iowait     = stoi(js[kIOwait_]); // Time waiting for I/O to complete
  long irq        = stoi(js[kIRQ_]); // Time servicing interrupts.
  long softirq    = stoi(js[kSoftIRQ_]); // Time servicing softirqs.
  long steal      = stoi(js[kSteal_]); // Stolen time, which is the time spent in other os when running in a virtualized environment
  long guest      = stoi(js[kGuest_]); // Time spent running a virtual CPU for guest operating systems under the control of the Linux kernel
  long guest_nice = stoi(js[kGuestNice_]); // Time  spent running a niced guest (virtual CPU for guest os under the control of the Linux kernel).
  return (user + nice + system + irq + softirq + steal + idle + iowait + guest + guest_nice);
}

void LinuxParser::SetProcessState(LinuxParser::PidStat &p, char state_int)
{
  switch (state_int) {
    case 'R':{
      p.state_ = kRunning_;
      break;
    }
    case 'S':{
      p.state_ = kSleeping_;
      break;    
    }
    case 'D':{
      p.state_ = kDisk_;
      break;    
    }
    case 'Z':{
      p.state_ = kZombie_;
      break;    
    }
    case 'T':{
      p.state_ = kStopped_;
      break;    
    }
    case 't':{
      p.state_ = kTrcaingStop_;
      break;    
    }
    case 'X':{
      p.state_ = kDead_;
      break;    
    }
    case 'x':{
      p.state_ = kDeadold_;
      break;    
    }
    case 'K':{
      p.state_ = kWakekillOld_;
      break;    
    }
    case 'W':{
      p.state_ = kWaking_;
      break;    
    }
    case 'P':{
      p.state_ = kParked_;
      break;    
    }
    default:
      p.state_ = kSleeping_;
      break;
  }

}


std::istream & LinuxParser::operator >> (std::istream &linestream,  LinuxParser::PidStat &p)
 {
  char state_int;

  //get pif field
  linestream  >> p.p_pid_;
  
  // command can have spaces (xxx yyy)
  std::string cmd_str;
  linestream  >> cmd_str;
  std::string s = cmd_str;

  //concatenate string until ')' detected
  while(cmd_str.compare(cmd_str.size() - 1, 1, ")")!= 0) {
    linestream  >> cmd_str;
    s = s + " " + cmd_str;
  } 
  p.comm_ = s;

  //set the state attribure
  linestream  >> state_int;
  SetProcessState(p, state_int);
  
  linestream  >> p.ppid_        >> p.pgrp_        >> p.session_      >>  p.tty_nr_     >> p.tpgid_ 
              >> p.flags_       >> p.minflt_      >> p.cminflt_      >> p.majflt_      >> p.cmajflt_     
              >> p.utime_       >> p.stime_       >> p.cutime_       >> p.cstime_      >> p.priority_    
              >> p.nice_        >> p.nun_threads_ >> p.itrealvalue_  >> p.starttime_   >> p.vsize_       
              >> p.rss_         >> p.rsslim_      >> p.startcode_    >> p.endcode_     >> p.startstack_  
              >> p.kstkesp_     >> p.kstkeip_     >> p.signal_       >> p.blocked_     >> p.sigignore_  
              >> p.sigcatch_    >> p.wchan_       >> p.nswap_        >> p.cnswap_      >> p.exit_signal_  
              >> p.processor_   >> p.rt_priority_ >> p.policy_       >> p.delayacct_blkio_ticks_  >> p.guest_time_  
              >> p.cguest_time_ >> p.start_data_  >> p.end_data_     >> p.start_brk_   >> p.arg_start_     
              >> p.arg_end_     >> p.env_start_   >> p.env_end_      >> p.exit_code_;

  return linestream;
 }

bool LinuxParser::ParseProcessStat (int pid, LinuxParser::PidStat& p) {
  bool ret = false;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> p;
    ret = true;
  }
  return ret;
}

// we need to look at : proc/[pid]/stat
// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long ajiffies;
  string line;

  PidStat p;
  LinuxParser::ParseProcessStat(pid, p); 

  ajiffies = (p.utime_ + p.stime_ + p.cutime_ + p.cstime_ + p.guest_time_ + p.cguest_time_);
  return ajiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> js = CpuUtilization();
  //The  amount  of  time, measured in units of USER_HZ
  long user       = stoi(js[kUser_]); // Time spent in user mode
  long nice       = stoi(js[kNice_]); // Time spent in user mode with low priority (nice).
  long system     = stoi(js[kSoftIRQ_]); // Time spent in system mode.
  long irq        = stoi(js[kIRQ_]); // Time servicing interrupts.
  long softirq    = stoi(js[kSoftIRQ_]); // Time servicing softirqs.
  long steal      = stoi(js[kSteal_]); // Stolen time, which is the time spent in other os when running in a virtualized environment
  long guest      = stoi(js[kGuest_]); // Time spent running a virtual CPU for guest operating systems under the control of the Linux kernel
  long guest_nice = stoi(js[kGuestNice_]); // Time  spent running a niced guest (virtual CPU for guest os under the control of the Linux kernel).
  long active_jiffies = (user + nice + system + irq + softirq + steal + guest + guest_nice);//*sysconf(_SC_CLK_TCK);
  return active_jiffies;
}


// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idle_jiffies;
  vector<string> js = CpuUtilization();
  //The  amount  of  time, measured in units of USER_HZ
  long idle   = stoi(js[3]); // Time spent in the idle task. This value should be USER_HZ times the second entry in the /proc/uptime pseudo-file.
  long iowait = stoi(js[4]); // Time waiting for I/O to complete
  idle_jiffies = (idle+iowait); //*sysconf(_SC_CLK_TCK);
  return idle_jiffies;
}

// DONE: Read and return CPU utilization (as vector of strings)
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_utilization_info;
  std::string line;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::smatch sm;
    // CPU,user,nice,system,idle,iowait,irq,softirq,steal,guest
    std::regex str_expr (R"(cpu\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))");
    std::regex_match(line,sm,str_expr); 

    for (unsigned int i = 1; i < sm.size() ; i++) {
      cpu_utilization_info.push_back(sm[i]);
    } 
    stream.close();
    //while (std::getline(stream, line)){} 
  }
  return cpu_utilization_info;
}


// Information about the total number of processes on the system exists in the /proc/meminfo file
// SONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int processes;
  string line;
  string processes_string;
  int number;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> processes_string >> number;
      if (processes_string=="processes")
        processes = number;
    }
  }
  return processes;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int procrunning;
  string line;
  string procrunning_string;
  int number;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> procrunning_string >> number;
      if (procrunning_string=="procs_running")
        procrunning = number;
    }
  }
  return procrunning;
}

// Linux stores the command used to launch the function in the /proc/[pid]/cmdline file.
// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string command="";
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (std::getline(stream, line)) {
    command = line;
  }
  return command;
}

// Return mem in mB
float calculateSize (std::string mem_size , std::string unit) {
  float memsize;
  if (unit=="kB")
    memsize = stof(mem_size)/1024;
  else if (unit=="mB")
    memsize = stof(mem_size);
  else if (unit=="gB")
    memsize = stof(mem_size)*1024;
  else if (unit=="B")
    memsize = stof(mem_size)/pow(1024,2);;
  return memsize; 
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string ram;
  string line;
  string key,val,unit;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val >> unit;
      //replace VmSize with VmData
      //it gives the exact physical memory being used as a part of Physical RAM
      //if (key=="VmSize:") {
      if (key=="VmData:") {
        ram = std::to_string(calculateSize(val,unit));
          break;
      }
    }
  }
  std::smatch sm;
  std::regex expr (R"((\d+)[.](\d{1,3}).*)");
  if (std::regex_match(ram, sm, expr)) {
    //align thr RAM to  \s\s\sX.YYY fromat
    std::string suffix,prefix;
    suffix = sm.str(2);
    for(long unsigned int i =0 ; i < 3 - std::size(sm.str(2)) ; i++)
      suffix += " "; 
    for(long unsigned int i =0 ; i < 5 - std::size(sm.str(1)) ; i++)
      prefix += " ";   
    ram = prefix + sm.str(1) + "." + suffix;
  }
  
  if (ram =="") ram = "    0.000";
  return ram;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string uid;
  string line;
  string key,val;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key=="Uid:")
        uid = val;
    }
  }

  return uid;   
}

// we need to look at /etc/passwd : 
// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string user;
  string line;
  string uid = Uid(pid);
  std::smatch sm;
  std::regex str_expr("(.*):x:"+ uid +":.*");
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {    
      std::regex_match(line,sm,str_expr);
      if (sm.size()>0)
        user = sm[1];
    }
  }
  return user;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {  
  string line;

  PidStat p;
  LinuxParser::ParseProcessStat(pid, p); 
  //  <---> boot <-----------> sys uptime
  //  <---> boot <--------> process start time  
  return UpTime() - p.starttime_/sysconf(_SC_CLK_TCK);
}
  // Check if pid exist in the system
bool LinuxParser::isExist(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid));
  return (stream.is_open());
}
