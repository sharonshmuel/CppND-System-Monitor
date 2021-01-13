#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "linux_parser.h"

class Processor {
 public:
  float Utilization();  
  void updateProcessor();
  // Declare any necessary private members
 private:
 float utilization_;
};

#endif