#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
  public:
    int Pid() const;
    std::string User() const;
    std::string Command() const;
    float CpuUtilization() const;
    std::string Ram() const;
    long int UpTime() const;

    //operators overloading
    bool operator<(Process const& a) const;
    bool operator>(Process const& a) const;

    //update process with acording to a given pid
    bool updateProcess(int pid);

    //update process acording to its pid member
    bool updateProcess();

    //check if process exist or not (to delete)
    bool isExist() const{
      return exist_;
    }

    // set for deletion
    void notExist() {
      exist_ = false;
    }

    //proces sis valid
    void Exist()  {
      exist_ = true;
    }

    float CpuUtilization() {
      return cpu_utilization_;
    }

    //initilise attributes
    Process() {
      prev_pv_ajp_ = 0;
      prev_pv_aj_ = 0; 
      exist_ = false;
      cpu_utilization_ = 0.0;
    }

  private:
    int pid_;
    std::string user_;
    std::string command_;
    float cpu_utilization_;
    std::string ram_;
    long int uptime_;

    //valid pid attribute
    bool exist_;

    // for cpu calculation/pid
    unsigned long prev_pv_ajp_; //store previous Active Jiffies/pid
    unsigned long prev_pv_aj_;  //store previous Jiffies/
    unsigned long new_pv_ajp_;  //store new Active Jiffies/pid
    unsigned long new_pv_aj_;   //store news Jiffies
    bool calcUtil() ;
};



#endif