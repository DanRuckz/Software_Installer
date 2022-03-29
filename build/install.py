#!/usr/bin/python3.7
import sys
import json
from subprocess import Popen
from subprocess import PIPE
import os
import requests

#this function is the end function, it kills the running program on the spot, should free all the memory and be gone.
#the os.kill function takes the PID of the running gnome-shell from the CPP file and kills that, this is to ensure proper killing procedure
def printexcept(e):
    logfile.write(f"Error happened: {e.__class__}\n Exiting...")
    logfile.write("")
    sb = Popen("$?", shell=True, encoding="utf-8", stdout=PIPE)
    aptexitcode = sb.stdout.read()
    sb.wait()
    sys.exit(aptexitcode)
    #os.kill(int(sys.argv[1]), signal.SIGTERM)

#the install itself happens here, apt-get install <package_name>
def apt_install(package):
    logfile.write(f"Installing the package {package}")
    sb = Popen(f"apt-get install {package} -y", shell=True, encoding="utf-8", stdout=PIPE)
    output = sb.stdout.read()
    logfile.write(output)
    sb.wait()

#apt update before each install to sync with the repos
def apt_update():
    logfile.write("updating apt\n")
    sb = Popen(f"apt-get update", shell=True, encoding="utf-8", stdout=PIPE)
    output = sb.stdout.read()
    logfile.write(output)
    sb.wait()

#loop through the json file, and check for the arguments, if the argument received in the program matches the a name in the json file, then a the whole dictionary of that program
#will be pushed to an object called programs
def parseArgsToDict():
    for dictlen in range(len(jsondata["programs"])):
        for argindex in range(len(sys.argv)):
            if sys.argv[argindex] == jsondata["programs"][dictlen]["name"]:
                install_type = jsondata["programs"][dictlen]["install_type"]
                package_name = jsondata["programs"][dictlen]["package_name"]
                program = {"install_type": install_type, "package_name": package_name}
                program["name"] = jsondata["programs"][dictlen]["name"]
                programs.append(program)
                logfile.write(f"The program that was detected from the json is {program}\n")
    logfile.write(f"The programs to install are: {programs}\n")

#here is the install function, currently is only has apt as an option, more options are planned in the future
def installByType(programs):
    for program in programs:
        logfile.write(f"Enumerating programs: {program['package_name']}\n")
        if program["install_type"] == "apt":
            apt_install(program["package_name"])



def setenvs():
    os.environ["PATH"] = "/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin"


#The execution starts here, could not get this to work within the function, will fix in the future
try:
    logfile = open(sys.argv[3], "a+")
    logfile.write("Child is now writing to logfile\n")
    #not using the jsonfile anymore, it's pulling the programs data from git
    #j = open(f"{os.path.dirname(sys.argv[0])}/programs.json")
    #jsondata = json.load(j)
    url = 'http://gitlab.mobileye.com/api/v4/projects/3985/repository/files/assets%2Fprograms.json/raw?ref=only-json'
    headers = {'PRIVATE-TOKEN': 'oq_Mh_cLuZ8zSyj7sxz4'}
    response = requests.get(url, headers=headers)
    jsondata = json.loads(response.text)
    #logfile.write(f"jsonfile received: {jsondata}\n")
    programs = []
except Exception as e:
    print("file loading failed")
    printexcept(e)

def main():
    try:
        setenvs()
        parseArgsToDict()
        apt_update()
        installByType(programs)
        logfile.write("")
    except Exception as e:
        printexcept(e)


if __name__ == '__main__':
        main()