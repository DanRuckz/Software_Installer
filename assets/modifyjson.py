#!/usr/bin/python3.7
import sys
import json
import os
import subprocess
import traceback

REPO='modifyjson'

def end(e):
    print(f"Error happened: {e.__class__}\n Here's the traceback:")
    print(traceback.format_exc())
    sys.exit()

def gitFetch(REPO):
    if os.path.isdir(os.path.join(os.getcwd(), REPO)) == False:
        sb = subprocess.Popen(f"mkdir {REPO}", shell=True, encoding="utf-8")
        sb.wait()
        os.chdir(os.path.join(os.getcwd(), REPO))
        sb = subprocess.Popen(f"git init .", shell=True, encoding="utf-8")
        sb.wait()
        sb = subprocess.Popen(f"git remote add origin git@gitlab.mobileye.com:bogdano/linux-software-center.git", shell=True, encoding="utf-8")
        sb.wait()
        sb = subprocess.Popen(f"git fetch", shell=True, encoding="utf-8")
        sb.wait()
        sb = subprocess.Popen(f"git checkout only-json", shell=True, encoding="utf-8")
        sb.wait()

    else:
        os.chdir(os.path.join(os.getcwd(), REPO))
        sb = subprocess.Popen(f"git pull", shell=True, encoding="utf-8")
        sb.wait()

def extractKeyValue(dict):
    return list(dict.keys()), list(dict.values())


def askInput(operations):
    key_list, value_list = extractKeyValue(operations)
    print(f"please choose operation: ")
    for k, v in zip(key_list, value_list):
        print(f"{k}) {v}")
    operation = input()
    while operation not in key_list:
        print("Wrong value, try again")
        operation = input()
    return operation

def handleInput(operation,install_types):
    if operation == "Add":
        addItem(install_types)
    if operation == "Delete":
        removeItem()
    if operation == "Update json":
        updateJson("all-programs", "all-programs")
    if operation == "Show json":
        outputJson()


def writeToJson(jsonfile, jsondata):
    jsonfile.truncate(0)
    jsonfile.seek(0)
    json.dump(jsondata, jsonfile, indent=4)
    global jsonWritten
    jsonWritten = True

def outputJson():
    with open(f"{os.path.dirname(os.path.abspath(sys.argv[0]))}/assets/programs.json", "r") as jsonfile:
        print(jsonfile.read())


def addItem(install_types):
    name = input("Please input the name of the program\n").capitalize()
    key_list, value_list = extractKeyValue(install_types)
    print("Please choose the install type")
    for k, v in zip(key_list, value_list):
        print(f"{k}) {v}")
    install_type_number = input()
    while install_type_number not in key_list:
        print("Wrong Input")
        install_type_number = input()
    install_type = install_types[install_type_number]
    package_name = input("Please enter the package name\n")

    with open(f"{os.path.dirname(os.path.abspath(sys.argv[0]))}/assets/programs.json", "r+") as jsonfile:
        jsondata = json.load(jsonfile)
        new_item = {
            "name": f"{name}",
            "install_type": f"{install_type}",
            "package_name": f"{package_name}"
        }
        jsondata["programs"].append(new_item)
        writeToJson(jsonfile, jsondata)
        jsonfile.close()
    updateJson(name, package_name)
    outputJson()


def removeItem():
    with open(f"{os.path.dirname(os.path.abspath(sys.argv[0]))}/assets/programs.json", "r+") as jsonfile:
        jsondata = json.load(jsonfile)
        program = input("Please input the name of the program to remove\n").capitalize()
        jsondata["programs"].pop(searchProgram(jsondata, program))
        writeToJson(jsonfile, jsondata)
        print("Deleted")
        jsonfile.close()
    outputJson()

def searchProgram(jsondata, program):
    program_found = False
    while True:
        for dictionary in range(len(jsondata["programs"])):
            if program in jsondata["programs"][dictionary]["name"]:
                return dictionary
        if not program_found:
            print("Wrong input, please try again")
            program = input().capitalize()
        else:
            break

def findVersion(package):
    sb = subprocess.Popen(f"apt policy {package} 2>/dev/null |grep '500'", shell=True, encoding="utf-8", stdout=subprocess.PIPE)
    programVersion = sb.stdout.read()
    programVersion = programVersion.split("\n")[0].split(" ")
    programVersion = programVersion[:-1]
    programVersion = " ".join(programVersion)
    programVersion = programVersion[5:]
    return programVersion

def updateJson(program_name, package_name):
    if package_name != "all-programs":
        programVersion = findVersion(package_name)
        with open(f"{os.path.dirname(os.path.abspath(sys.argv[0]))}/assets/programs.json", "r+") as jsonfile:
            jsondata = json.load(jsonfile)
            jsondata["programs"][searchProgram(jsondata, program_name.capitalize())]["version"] = programVersion
            writeToJson(jsonfile, jsondata)
            jsonfile.close()
    else:
        with open(f"{os.path.dirname(os.path.abspath(sys.argv[0]))}/assets/programs.json", "r+") as jsonfile:
            jsondata = json.load(jsonfile)
            for dictionary in range(len(jsondata["programs"])):
                jsondata["programs"][dictionary]["version"] = findVersion(jsondata["programs"][dictionary]["package_name"])
            writeToJson(jsonfile, jsondata)
            jsonfile.close()

def gitPush():
    sb = subprocess.Popen(f"git add *", shell=True, encoding="utf-8")
    sb.wait()
    sb = subprocess.Popen(f'git commit -m"json file changed"', shell=True, encoding="utf-8")
    sb.wait()
    sb = subprocess.Popen(f'git push', shell=True, encoding="utf-8")
    sb.wait()


def main():
    try:
        gitFetch(REPO)
        operations = {
        "1": "Add",
        "2": "Delete",
        "3": "Update json",
        "4": "Show json"
        }
        install_types = {"1": "apt"}
        input = askInput(operations)
        operation = operations[input]
        handleInput(operation, install_types)
        if jsonWritten:
            gitPush()

    except Exception as e:
        end(e)

if __name__ == "__main__":
    jsonWritten = False
    main()