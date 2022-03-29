//This program is aiming to make the installation of software on our system much easier by implementing a GUI interface to pick one or more of the desired software, and then install it at a click of a button.

//This is the main function that starts the window
#include "window.h"
#include "globals.h"

//EXEC_PATH is a an extern defined in globals.h, it contains the absolute path of the current program,
//used to define the working directory to be the current executable's directory

std::string EXEC_PATH;
std::ofstream logfile;
std::string LOGPATH;

void handle_sigint(int sig);
int main(int argc, char ** argv) { 
        //in case of SIGINT, the fuction sigintHandle is in globals.cpp
        signal(SIGINT, sigintHandle);
        //gathering the current localtime
        time_t now = time(0);
        tm* ltm = localtime(&now);
        //deleting the temporary files and lockfiles
        void removeTmpFiles();
        //setting the Executables full path(getting it from the system)
        setExecPath(EXEC_PATH);
        setLogPath(LOGPATH);
        //opening logfile
        openLogFile(LOGPATH);
        //starting to log
        logfile << "-------------------------------------------------LOG START-----------------------------------------------------------" << '\n';
        //ltm->tm_min is an int, so if smaller than 10 it won't print the 0 in the beginning
        if(ltm->tm_min >=10){logfile << "TIMESTAMP: " << ltm->tm_mday<< "." << 1+ltm->tm_mon << "." <<1900 + ltm->tm_year << " - " << ltm->tm_hour << ":" << ltm->tm_min << '\n';}
        else { logfile << "TIMESTAMP: " << ltm->tm_mday<< "." << 1+ltm->tm_mon << "." <<1900 + ltm->tm_year << " - " << ltm->tm_hour << ":" << "0" << ltm->tm_min << '\n';}
        //making sure we can log into it after changing execution to the child process with the uid of 0
        std::string chmodcommand = "chmod 777 "+LOGPATH;
        shell::execCommand((char*)chmodcommand.c_str());

    //Check if we are running from the commandline or from the GUI. If not running from the GUI, close.
    //I might add an option for a cmd installer.
    if (isatty(fileno(stdin))) {
        logfile << "Running from terminal\n";
        //std::string arg = "";
        //if(argc ==2){arg = argv[1];}
            //if(arg != "--cli"){
                //std::cerr << "running from the command line... please run from desktop" << std::endl;
                //return -1;
            //}
        }


    int fd;
    try{

        //This function finds the current program's path and puts it into EXEC_PATH
        std::string uid = std::to_string(getuid()); 
        std::string lockfile = EXEC_PATH + "tmp/" + uid + ".lock";
        //Check if lockfile exists, if it does don't open it
        if(access(lockfile.c_str(), F_OK) ==0){
        logfile << "Lockfile exists, don't try to run the program twice" <<std::endl;
        logfile << "IF IT'S JUST A SINGLE INSTANCE, CLEAR THE LOCKFILE AT " << lockfile << '\n';
        std::cout << "Lockfile exists, don't try to run the program twice" <<std::endl;
        std::cout << "IF IT'S JUST A SINGLE INSTANCE, CLEAR THE LOCKFILE AT " << lockfile << '\n';
        return -1;
    }

    //Create a lockfile when the program is open and delete it when it finished running
        if(fd = open(lockfile.c_str(), O_CREAT) == -1){
            logfile << "error creating lockfile at "<<lockfile << std::endl;
            return -1;
        }

    //creating an object from window, a single instance for a single window.
        window();
        removeTmpFiles();
        logfile << "-------------------------------------------------LOG END-------------------------------------------------------------" << '\n'<< std::endl;
        logfile.close();
        close(fd);
        return 0;
    }
    catch(...){
        logfile << "some exception happened" <<'\n';
        removeTmpFiles();
        logfile << "-------------------------------------------------LOG END-------------------------------------------------------------" << '\n' <<std::endl;
        logfile.close();
        close(fd);
        std::cout << "Please check the logs at " + LOGPATH << "\n";
        return 0;
    }
}

