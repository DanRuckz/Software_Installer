#include "c_gui.h"

C_gui::C_gui(){
};

//Sets the gui theme for the drawable objects, this is provided by TGUI
void C_gui::setGuiTheme(){
    std::string themepath = EXEC_PATH + "assets/TGUI/themes/Black.txt";
    std::string errthemepath = EXEC_PATH + "assets/TGUI/themes/Black.txt.orig";
    theme.load(themepath);
    errTheme.load(errthemepath);
}
//takes the target to set the TGUI target to, to know which window to use with this library
void C_gui::setGuiTarget(sf::RenderWindow& target){
    gui.setTarget(target);
}
//the child window and all the objects within it. It's created here dynamically
void C_gui::createChildWindow(sf::Vector2f position){
    CheckBoxWindow = tgui::ChildWindow::create();
    CheckBoxWindow->setInheritedOpacity(0.5);
    CheckBoxWindow->setRenderer(theme.getRenderer("ChildWindow"));
    CheckBoxWindow->setSize(ChildWindowSize_x,ChildWindowSize_y);
    CheckBoxWindow->setPosition(position);
    CheckBoxWindow->setPositionLocked(true);
    CheckBoxWindow->setResizable(false);
    CheckBoxWindow->setKeepInParent(true);
    CheckBoxWindow->setTitleButtons(0);
    //CheckBoxWindow->setTitle("Please choose your software");
    CheckBoxWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Center);
    CheckBoxWindow->setFocusable(false);
    //adding the checkboxwindow to the gui object, so it will be drawable in the main loop
    gui.add(CheckBoxWindow);
    //creating the tickboxes
    //createTickBoxes();
    openProgramsFile();
    //cannot send the json structure by reference because this function will run in another thread
    waitFuture = std::async(std::launch::async, &C_gui::createTickBoxes,this);
    //creating the scrollbar
    createScrollBar();
}

//Under construction
void C_gui::createErrWindow(){
    errWindow = tgui::ChildWindow::create();
    errWindow->setRenderer(errTheme.getRenderer("ChildWindow"));
    errWindow->setSize({errWindow_x,errWindow_y});
    errWindow->setPosition({getChildWindow()->getSize().x/2 - errWindow_x/2, getChildWindow()->getSize().y/2 - errWindow_y/2});
    errWindow->setResizable(false);
    errWindow->setKeepInParent(false);
    errWindow->setTitleButtons(0);
    errWindow->setTitle("Error");
    errWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Center);
    errWindow->setVisible(false);
    errWindow->setInheritedOpacity(1);
    gui.add(errWindow);
    createErrButton();
    createErrMsg();
}

inline void C_gui::createErrButton(){
    errButton = tgui::Button::create();
    errButton->setRenderer(errTheme.getRenderer("Button"));
    errButton->setSize({errButtonSize_x,errButtonSize_y});
    errButton->setPosition({errWindow->getSize().x/2 - errButtonSize_x/2, errWindow_y - errButtonSize_y - 10});
    errButton->setText("OK");
    errButton->setVisible(false);
    //under construction
    errButton->connect("pressed", &C_gui::buttonPressedSignal,&CheckBoxWindow, &errWindow);
    errWindow->add(errButton);
}

inline void C_gui::createErrMsg(){
    errLabel = tgui::Label::create();
    std::string labeltxt = "Some error has happened. Please check the Log at \n\n" + LOGPATH +".";
    errLabel->setRenderer(theme.getRenderer("Label"));
    errLabel->setText(labeltxt);
    errLabel->setTextSize(10);
    errLabel->setPosition(10,0);
    errWindow->add(errLabel);
}

void C_gui::showErr(){
    errWindow->setVisible(true);
    errButton->setVisible(true);
    errWindow->setFocused(true);
    gui.moveWidgetToFront(errWindow);
}
//This function is not actually called, but it's being implemented in the button click signal handling function
void C_gui::hideErr(){
    errWindow->setVisible(false);
}

bool C_gui::queryErr(){
    return errWindow->isVisible();
}

inline void C_gui::openProgramsFile(){
        //loading and reading the json file that contains the software to be installed from the git
        std::string curl = shell::execCommand("curl -s --request GET --header 'PRIVATE-TOKEN: oq_Mh_cLuZ8zSyj7sxz4' http://gitlab.mobileye.com/api/v4/projects/3985/repository/files/assets%2Fprograms.json/raw?ref=only-json");

        //this code is for reading from a file, currently leave it like this here.
        //std::string path = EXEC_PATH + "assets/programs.json";
        //std::ifstream ifs(path);

        //parsing the json from git and returning it to the programs variable
        programs = json::parse(curl);
        //get the amount of dictionaries in the dictionary list

        numberofTickBoxes = programs["programs"].size();
        //closing the ifstream pipe after finishing reading the json file
        //ifs.close();
}


        void C_gui::createTickBoxes(){
        std::lock_guard<std::mutex> guard(loaderMutex);
        //Creating a vector that stores all the dictionaries of the json for ease of access later
        //sorting the programs alphanumerically, to do that we are creating a vector and pushing the strings to it
        std::vector<std::string> sortedNames;
        for (int i=0;i<numberofTickBoxes;i++){
            programDataContainer.push_back(programs["programs"][i]);
            //taking the package names from json and putting them into a vector to check for which is already installed
            sortedNames.push_back(programs["programs"][i]["name"]);
        }
        //check what is currently installed, so then we can set the ticks correctly
        checkInstalled();
        //here we are sorting the vector
            sort(sortedNames.begin(), sortedNames.end());
        
            tgui::Layout layout_x;
            tgui::Layout layout_y;
            
        for (int i=0;i<numberofTickBoxes;i++){
            //create a checkbox according to the number of programs in the json file
            checkbox = tgui::CheckBox::create();
            //creating labels so we can use it later as an indication that the packages are being installed or are installed
            //creating a smart pointer for each one of the labels, so it gets erased when it's out of scope
            label = std::make_shared<Labels>();
            label->installingLabel = tgui::Label::create();
            label->installedLabel = tgui::Label::create();
            //this is required by TGUI for disaply
            checkbox->setRenderer(theme.getRenderer("CheckBox"));
            label->installingLabel->setRenderer(theme.getRenderer("Label"));
            label->installedLabel->setRenderer(theme.getRenderer("Label"));
            //each checkbox has it's own position and they are created with an iterval, to prevent creating on top of each other
            //labels are bound to the position of each checkbox with an offset
            checkbox->setPosition({0, 0 + i*checkBoxInterval});
            layout_x = tgui::bindLeft(checkbox);
            layout_y = tgui::bindTop(checkbox);
            //each checkbox has the same size, labels have the same text size too
            checkbox->setSize(CheckBoxSize,CheckBoxSize);
            label->installingLabel->setTextSize(installTextSize);
            label->installedLabel->setTextSize(installTextSize);
            //set the position of each label
            label->installingLabel->setPosition(layout_x + installingTextOffset,layout_y + checkbox->getSize().y/2 - checkboxTextSize);
            label->installedLabel->setPosition(layout_x + installingTextOffset,layout_y + checkbox->getSize().y/2 - checkboxTextSize);
            //each checkbox gets the name of each program in the json as the set text after the sort
            std::string item = sortedNames.at(i);
            //setting the widget name, can be retrieved with a get function later by name
            checkbox->setWidgetName(item);
            //setting the text of the tickbox here
            checkbox->setText(item);
            //hide the label on initial creation, will later show them when relevant
            label->installingLabel->setVisible(false);
            label->installedLabel->setVisible(false);
            //user data is later retrieved as a C++ template, so we can store tgui::Any, which includes classes,structs,primitives,pointers and smart pointers. 
            //we store a shared pointer to each instance of the Labels struct we made earlier 
            checkbox->setUserData(label);
            //adding the checkbox to the child window, so it displays it inside of the child window and the positions are relative to the child window and not the main window
            CheckBoxWindow->add(checkbox);
            //saving each checkbox in my own vector for future easy reference and access
            checkboxes.push_back(checkbox);
            CheckBoxWindow->add(label->installingLabel);
            CheckBoxWindow->add(label->installedLabel);
        }
        handleInstalled();
}

void C_gui::handleInstalled(){
    
    //Check if a program is already installed, then make it impossible to click and set the opacity to 0.5.
    //This is done by looping through the checkboxes vector and checking which element in the installedPrograms vector has the same
    //name as each element in the checkboxes vector. what's been found the same will be crossed out and marked as already installed
    //if an element is the checkboxes is not found in the installedPrograms vector, then it will be enabled - for a true check.

    //Creating layouts to bind them to the position of the checkboxes - so that each time the position of each checkbox moves, it makes the installed label move too
    tgui::Layout layout_x;
    tgui::Layout layout_y;
    std::string userVersion;
    std::string package;
    for (int i=0;i<checkboxes.size();i++){
        for (int j=0; j<installedPrograms.size();j++){
            std::string checkboxName = checkboxes.at(i)->getText() + '\n';
            auto data = checkboxes.at(i)->getUserData<std::shared_ptr<Labels>>(); 
            if(installedPrograms.at(j).programName == checkboxName){
                //this is the part where the label is enabled, set it only if the checkbox is enabled (ready for install). 
                    auto data = checkboxes.at(i)->getUserData<std::shared_ptr<Labels>>();             
                   
                    //Here we want to allow the user to always stay on the latest version, even if he has something already installed.
                    //So we are only blocking the option if the latest version is already installed

                    //getting the package name to check the currently installed version of
                    package = installedPrograms.at(j).packageName;
                    //getting rid of the last endline so our comparison is good
                    package.erase(std::remove(package.begin(), package.end(), '\n'), package.end());
                    //executing the command without the newline, so it executes without issues
                    userVersion = "apt policy " + package + " 2>/dev/null |grep Installed |awk '{print $2}'";
                    userVersion = shell::execCommand((char*)userVersion.c_str());
                    //getting the input back from the command execution and removing the newline again so it will compare the lines properly
                    userVersion.erase(std::remove(userVersion.begin(), userVersion.end(), '\n'), userVersion.end());
                    //setting the currently used version on the label
                    data.get()->installedLabel->setVisible(true);
                    data.get()->installedLabel->setText("Installed version: "+userVersion);
                    //If we are on a later version than what's on the repo, don't offer to upgrade
                    if(userVersion >= installedPrograms.at(j).repoVersion){
                        //Here we are disabling the widgets so they don't receive the signals and setting opacity to less, so it looks greyed out.
                        //Afterwards we are enabling the "Installed" label.
                        checkboxes.at(i)->setInheritedOpacity(installedOpacity);
                        checkboxes.at(i)->setEnabled(false);
                        checkboxes.at(i)->setChecked(false);
                    }
                    break;                    
            }
            //If the names of each installed programs are not within the text of the checkboxes, then enable them - that means the program is not installed
            //this is done so that we are truly checking each program, not just assuming it's installed just because we previously detected/install it - it will also detect
            //outside changes 
            else if(installedPrograms.at(j).programName != checkboxName){
                checkboxes.at(i)->setInheritedOpacity(1);
                checkboxes.at(i)->setEnabled(true);
                data.get()->installedLabel->setVisible(false);
            }
        }
    }
}

inline void C_gui::createScrollBar(){
//creating the scroll bar and defining it
scrollbar = tgui::Scrollbar::create();
scrollbar->setRenderer(theme.getRenderer("Scrollbar"));
scrollbar->setWidgetName("Scrollbar");
//setting the size to be 18 pixels wide and as hight as the checkboxwindow (the child window)
scrollbar->setSize(18, CheckBoxWindow->getSize().y);
//the position will be on the right side of the child window 
scrollbar->setPosition({CheckBoxWindow->getSize().x - scrollbar->getSize().x, CheckBoxWindow->getSize().y - scrollbar->getSize().y});
//the size of the viewport, the child window part that's shown on the screen
scrollbar->setViewportSize(ChildWindowSize_y);
//get the ratio between the viewport size and each checkbox
uint ViewportElementSize = scrollbar->getViewportSize()/CheckBoxSize;
//the maximum possible for the scrollbar to sroll is the viewport size plus the number of checkboxes minus the ratio between the viewport and a checkbox
scrollbar->setMaximum(scrollbar->getViewportSize()+numberofTickBoxes-ViewportElementSize);
//set the scrollbar at the very top
scrollbar->setValue(0);
//this is a signal function, this is triggered when the scrollbar is moved
scrollbar->connect("ValueChanged", &C_gui::signalHandler_ScrollbarScrolled, &scrollValueState);
//add the scrollbar to be drawn as a gui element
CheckBoxWindow->add(scrollbar);
//update the CurrentScrollBarValue variable upon creation
updateCurrentScrollBarValue();
}

tgui::ChildWindow::Ptr C_gui::getCheckBoxWindow(){
    return CheckBoxWindow;
}

tgui::ChildWindow::Ptr C_gui::getChildWindow(){
    return CheckBoxWindow;
}
//if the scrollbar value is changed set scrollbarValueState to true
void C_gui::signalHandler_ScrollbarScrolled(bool *scrollbarValueStatePtr, tgui::Widget::Ptr ScrollBar, const std::string& signalName){
    *(scrollbarValueStatePtr) = true;
}
//under contruction
void C_gui::buttonPressedSignal(tgui::ChildWindow::Ptr *CheckBoxWindow,tgui::ChildWindow::Ptr *errWindowptr,tgui::Widget::Ptr widget, const std::string& signalName){
    CheckBoxWindow->get()->setEnabled(true);
    errWindowptr->get()->setVisible(false);
}
//Wrapper function to be used outside the class to get the scrollbarvalue
uint C_gui::getNewScrollbarValue(){
    return scrollbar->getValue();
}
//This function is used in the main loop outside this class. It moves the tickboxes when the the value has been changed
void C_gui::moveTickBoxes(){
   
   //going through the whole vector, checking if the element is indeed a checkbox and setting the new position to each checkbox when the scrollbar value has changed
   for (int i=0; i< checkboxes.size();i++){
           checkboxes.at(i)->setPosition(checkboxes.at(i)->getPosition().x,checkboxes.at(i)->getSize().y*i - scrollbar->getValue()*CheckBoxSize);
     }
     //setting the scrollbarvalue to false after finishing the movement, to be pending for the next time the scrollbar is moved
   scrollValueState = false;
   //update the CurrentScrollBarValue to be ready for the next check
   updateCurrentScrollBarValue();
}



uint C_gui::getCurrentScrollBarValue(){
    return currentScrollBarValue;
}

void C_gui::updateCurrentScrollBarValue(){
    currentScrollBarValue = scrollbar->getValue();
}


bool C_gui::getScrollBarState(){
    return scrollValueState;
    
}
// loop through each one of the checkboxes and see if they are checked, append the checked widgets to a vector called checked_vec and return that vector
//this is used in the main loop to check which widgets to send to the install binary
std::vector<tgui::CheckBox::Ptr> C_gui::getTickedWidgets(){
   checked_vec.clear();
   for (int i=0;i<checkboxes.size();i++){
       if(checkboxes.at(i)->isChecked()){
           checked_vec.push_back(checkboxes.at(i));    
           }
    }
    return checked_vec;
}

tgui::Scrollbar::Ptr C_gui::getScrollBar(){
    return scrollbar;
}

//this is done to enable scrolling in the child window, the signal is sent from the main loop
void C_gui::scrollInChildWindow(float delta){
    if (delta == -1 && scrollbar->getValue() != scrollbar->getMaximum()) {
        scrollbar->setValue(scrollbar->getValue()+1);
    }
    else if (delta == 1 && scrollbar->getValue() != 0) {
        scrollbar->setValue(scrollbar->getValue()-1);
    }
}

//This function goes through the json file and takes the package name out of it, then it queries the package manager to see if it's there.
//after some sort we should get the exact package name. later we push the name and the package name to the program struct and then push that struct in the InstalledPrograms vector.
void C_gui::checkInstalled(){
    std::string command;
    InstalledProgramData program;
    std::string package;
    std::string version;
    installedPrograms.clear();
    for(int i=0;i<programDataContainer.size();i++){
        package = programDataContainer.at(i)["package_name"];
        version = programDataContainer.at(i)["version"];
        command = "dpkg --get-selections |grep " + package + "|grep -v deinstall |awk '{print $1}' |grep -x " + package;
        std::string result = shell::execCommand(command.c_str());
        if(result != ""){
            program.packageName = result;
            program.programName = programDataContainer.at(i)["name"];
            program.programName += '\n';
            program.repoVersion = version;
            installedPrograms.push_back(program);
            }
        }
}

//disabling checkboxes, this is done during the time of the installation process, so that they aren't accessible by the user during the time of install
void C_gui::disableCheckBoxes(){
    for(int i=0;i<checkboxes.size();i++){
        if(checkboxes.at(i)->isChecked() == false && checkboxes.at(i)->getInheritedOpacity() == 1){
        checkboxes.at(i)->setEnabled(false);
        checkboxes.at(i)->setInheritedOpacity(installingOpacity);
        }
    }
}
//after install we can enable the same checkboxes that were disabled
void C_gui::enableCheckBoxes(){
    for (int i=0;i<checkboxes.size();i++)
    {
        if(checkboxes.at(i)->getInheritedOpacity() == installingOpacity) {checkboxes.at(i)->setEnabled(true); checkboxes.at(i)->setInheritedOpacity(1);}
    }
}

void C_gui::disableCheckboxWindow(){
    CheckBoxWindow->setEnabled(false);
}
//when installing, show the label we created earlier to show that the program is installing
void C_gui::showInstallingText(bool done){
    if(!done){
        std::string tmpstring;
        for(int i=0;i<checked_vec.size();i++){
            auto data = checked_vec.at(i)->getUserData<std::shared_ptr<Labels>>();
            data.get()->installingLabel->setVisible(true);
            
            if(data.get()->installedLabel->getText() == ""){
                if(data.get()->installingTextIteration % data.get()->installingTextDots == 0){
                    data.get()->installingLabel->setText("Installing");
                    data.get()->installingTextIteration +=1;
                }

                else{
                    tmpstring = data.get()->installingLabel->getText();
                    tmpstring.append(".");
                    data.get()->installingLabel->setText(tmpstring);
                    data.get()->installingTextIteration +=1;
                    }
            }
            if(data.get()->installedLabel->getText() != ""){
                data.get()->installedLabel->setText("");
                if(data.get()->installingTextIteration % data.get()->installingTextDots == 0){
                    data.get()->installingLabel->setText("Upgrading");
                    data.get()->installingTextIteration +=1;
                }
                else{
                    tmpstring = data.get()->installingLabel->getText();
                    tmpstring.append(".");
                    data.get()->installingLabel->setText(tmpstring);
                    data.get()->installingTextIteration +=1;
                    } 
            }
        }
    }
//if we are done installing, the function is also responsible to set the labels to be invisible again, we just need to send it the "done" variable.
    else if(done){
         for(int i=0;i<checked_vec.size();i++){
            auto data = checked_vec.at(i)->getUserData<std::shared_ptr<Labels>>();
            data.get()->installingLabel->setVisible(false);
            setTextIterationToZero();
        }    
    }
}

void C_gui::setTextIterationToZero(){
    for(int i=0;i<checked_vec.size();i++){
        checked_vec.at(i)->getUserData<std::shared_ptr<Labels>>()->installingTextIteration = 0;
    }   
}

C_gui::~C_gui(){
    //Every dynamically allocated objects will get destroyed as soon as the destructor is called, since they are all smart pointers.
};