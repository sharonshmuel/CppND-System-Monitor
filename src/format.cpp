#include <string>
#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    string format;
    long minutes = seconds / 60;
    long hours = minutes / 60;
    long h = hours;// (hours % 24) ;
    string hs = (h < 10) ? "0" + std::to_string(h): std::to_string(h); 
    long m = (minutes % 60);
    string ms = (m < 10) ? "0" + std::to_string(m): std::to_string(m); 
    long s = (seconds % 60);
    string ss = (s < 10) ? "0" + std::to_string(s): std::to_string(s); 
    format = hs+":"+ms+":"+ss;
    return format; 
}