#include "globals.h"
std::string getexepath();
//sets the EXEC_PATH function to the location of the current program and removes the program name, so that we are only left with the directory name
void setExecPath(std::string &EXEC_PATH){
    EXEC_PATH = getexepath();
    EXEC_PATH = EXEC_PATH.substr(0, EXEC_PATH.find_last_of("/"));
    EXEC_PATH += '/';
}

std::string getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}

std::string shell::execCommand(const char* cmd){
   char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

void openLogFile(std::string LOGPATH){
    logfile.open(LOGPATH,std::ios_base::app);
    if(! logfile.is_open()){std::cerr << "Log failed to open" << '\n';}
}

void sigintHandle( int signum ){
    std::cout << "SIGINT RECEIVED, CLOSING...\n";
    logfile << "SIGINT RECEIVED, CLOSING...\n";
    removeTmpFiles();
    exit(signum);
}

//This function is used to clear out the unused instances of the executable file in the tmp directory.
//This is done to be able to update the installer even if there is an instance running somewhere on the storage
inline void removeTmpFiles(){
    std::string command = "rm -rf " + EXEC_PATH + "tmp/*";
    system(command.c_str());
}

void setLogPath(std::string& LOGPATH){
    int i_uid = getuid();
    std::string uid = std::to_string(i_uid);
    //getting the path of our log file
    char hostname[HOST_NAME_MAX+1];
    gethostname(hostname, HOST_NAME_MAX+1);
    LOGPATH = EXEC_PATH + "logs/"+uid+"-"+hostname+".log";
}