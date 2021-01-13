#include "processor.h"

// return utilization 
void Processor::updateProcessor() {
  utilization_ = float(LinuxParser::ActiveJiffies()) / float(LinuxParser::Jiffies());
}

// Return the aggregate CPU utilization
float Processor::Utilization() { 
  return utilization_;
}