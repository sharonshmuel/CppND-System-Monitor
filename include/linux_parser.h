#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

enum ProcessStates {
  kRunning_,      //Running
  kSleeping_,     //Sleeping in an interruptible wait
  kDisk_,         //Waiting in uninterruptible disk sleep
  kZombie_,       //Zombie
  kStopped_,      //Stopped (on a signal) or (before Linux 2.6.33) trace stopped
  kTrcaingStop_,  //Tracing stop (Linux 2.6.33 onward)
  kPaging_,       //Paging (only before Linux 2.6.0)
  kDead_,         //Dead (from Linux 2.6.0 onward)
  kDeadold_,      //Dead (Linux 2.6.33 to 3.13 only)
  kWakekillOld_,  //Wakekill (Linux 2.6.33 to 3.13 only)
  kWaking_,       //Waking (Linux 2.6.33 to 3.13 only)
  kParked_,       //Parked (Linux 3.9 to 3.13 only)
};


struct PidStat {
  int p_pid_;             // The process ID
  std::string comm_;      // The filename of the executable, in parentheses
  ProcessStates state_;   // characters, indicating process state
  int ppid_,pgrp_,session_,tty_nr_,tpgid_;
  unsigned int  flags_;
  unsigned long minflt_,cminflt_,majflt_,cmajflt_;
  unsigned long utime_;  // Amount of time that this process has been scheduled in user mode,measured in clock ticks
  unsigned long stime_;  // Amount of time that this process has been scheduled in kernel mode,measured in clock ticks
  unsigned long cutime_; // Amount of time that this process's waited-for children have been scheduled in user mode
  unsigned long cstime_; // Amount of time that this process's waited-for children have been scheduled in kernel mode
  unsigned long priority_,nice_,nun_threads_,itrealvalue_;
  unsigned long long starttime_; //The time the process started after system boot
  unsigned long vsize_;
  long rss_;
  unsigned long rsslim_, startcode_,endcode_,startstack_,kstkesp_,kstkeip_,signal_,blocked_,sigignore_,sigcatch_,wchan_,nswap_,cnswap_;
  int exit_signal_,processor_;
  unsigned int rt_priority_,policy_;
  unsigned long long delayacct_blkio_ticks_;
  long guest_time_;   // Guest  time  of the process
  long cguest_time_;  // Guest  time of the process's children, measured in clock ticks
  unsigned long start_data_,end_data_,start_brk_,arg_start_,arg_end_,env_start_,env_end_;
  int exit_code_;

};

// Overload the operator in order to stream to PidStat object 
std::istream & operator >> (std::istream &linestream,  PidStat &p);

std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);

// Check if pid exist in the system
bool isExist(int pid);

// Function to parse all the fields in the /proc/[pid]/stat
bool ParseProcessStat (int pid, PidStat& p);

//set process state enum
void SetProcessState(PidStat &pidstat ,char state_int);

};  // namespace LinuxParser


#endif