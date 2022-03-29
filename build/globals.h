#pragma once
#include "inits.h"
#include <string>
//This function takes the external variable and sets it's value to the path of the current program, so that value is updated once
//and used through the whole program
void setExecPath(std::string&);
std::streambuf* getLogBuffer();
extern std::string EXEC_PATH;
extern std::string LOGPATH;
void setLogPath(std::string &LOGPATH);
//the name of the logfile
extern std::ofstream logfile;
void openLogFile(std::string logpath);
namespace shell{
   std::string execCommand(const char*); 
};
inline void removeTmpFiles();
void sigintHandle(int signum);