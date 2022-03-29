#include "window.h"

window::window() {
try{
    setstage();
    runWindow();
    }
catch (const tgui::Exception& exc)
    {
        logfile << "TGUI EXCEPTION: " << exc.what() << std::endl;
    }
}

//This function creates the the window, defines it and sets the correct position for every object within
    inline void window::setstage(){
    //the function uses the a RenderWindow object from the SFML library, and creates a black window with the resolution values stored in globals.h
    //Then sets the title and makes it unable to resize the window
    container_window.create(sf::VideoMode(resolution_x,resolution_y), "Linux Software Center", sf::Style::Close);
    //Sets the size of a RectangleShape object, and sets the color. This is the drawable and transformable object in the window.
    background.setSize(sf::Vector2f({resolution_x,resolution_y}));
    background.setFillColor(sf::Color{41,53,57});
    //This function sets the target for the TGUI library as the current window I am using (the container window)
    c_gui.setGuiTarget(container_window);
    //Setting the GUI theme from the available theme list
    c_gui.setGuiTheme();
    //This function takes the background rectangle, a margin for the width and a margin for the height
    //It returns a position (x,y) that's been calculated based on the given margins
    //For example: (background,1,5) will yield 1% of the x of the background and 5% of the y of the background into an sf::Vector2f object
    sf::Vector2f margin = calculateMargins(background,1,7);
    //This will create a child window inside the first window container, it uses the calculated margins for easier placement of the window
    c_gui.createChildWindow({background.getPosition().x + margin.x, background.getPosition().y + margin.y});
    //creates the install button
    createInstallButton();
    //sets the tilebar above the child window
    setTitleBar();
    //sets the credits under the install button
    setCredits();
    c_gui.waitFuture.wait();
    c_gui.createErrWindow();
}

void window::runWindow() {
    
    globalTimer.restart();
    //This is the loop that's run as long as the window is active
   while (container_window.isOpen())
    {
        //get the mouse coordination inside the container window
        mouseCoords = sf::Mouse::getPosition(container_window);
        //Check if the scrollbar has been moved, if it did then update the tick boxes to their appropriate position
        //This functionality is for the scrollbar, when the scrollbar value is different from the previous one, move the tick boxes accordingly to the new value
        if(c_gui.getScrollBarState()){
            c_gui.moveTickBoxes();
        }
         globalTime = globalTimer.getElapsedTime();
        if(globalTime.asMilliseconds() > 500){
            globalTimer.restart();
            if(!isDone) c_gui.showInstallingText(isDone);
        }

        //This is checking if the mouse left the install button (not inside of it anymore), if it did then it should retrieve the original color of the install button
        if (colorChanged)
        retrieveColor();

        sf::Event event;
        //This is an event listener loop, events are when something happens to the X window, for example if it gets resized, closed, minimized or anything that's happening to it.
        while (container_window.pollEvent(event))
        {
            //if X has been clicked on the window, then the container is closing and everything will be destroyed and freed.
            if (event.type == sf::Event::Closed)
                container_window.close();
            //The TGUI library provides it's own event handling, this function sends the events to that be handled to that library too.
            c_gui.gui.handleEvent(event);
            //In case the mouse moved, check if it's inside the install button, and if it is then hightlight it(adding to the alpha value)
           if (event.type == sf::Event::MouseMoved){
                if(installButton.getGlobalBounds().contains(sf::Vector2f(mouseCoords))){
                    installButton.setFillColor(sf::Color(installButton.getFillColor().r,installButton.getFillColor().g,installButton.getFillColor().b,200));
                    colorChanged = true;}
            }
            //Unfortunately the TGUI library does not provide (or at least not easily) a way to send a signal from the scrollbar to the child window, therefore
            //for this specific case I have to send a signal from SFML, and force the value of the scrollbar being changed manually from that signal
            //which will call the signal handler of the scrollbar itself and trigger the whole mechanism
            if(event.type == sf::Event::MouseWheelScrolled && c_gui.getChildWindow()->mouseOnWidget(sf::Vector2f(mouseCoords))){
                c_gui.scrollInChildWindow(event.mouseWheelScroll.delta);
            }
            //if the left click is pressed on the install button, then initiate the install
            if(event.type == sf::Event::MouseButtonPressed){
                    if (installButton.getGlobalBounds().contains(sf::Vector2f(mouseCoords))){
                        //install function returns true if the installer is installing, returns false if it's not - so that no install can happen
                        //if there are no arguments sent to the installer binary
                        //check if the function is running, if it does, don't run it
                            if(isDone)std::thread(&window::install,this).detach();
                        }
            }
        }

        //update the values and draw the objects every turn in the loop.
        container_window.clear();
        container_window.draw(background);
        container_window.draw(title);
        container_window.draw(installButton);
        container_window.draw(installText);
        container_window.draw(titleBar);
        container_window.draw(credits);
        c_gui.gui.draw();
        container_window.display();
    }
}
//This function takes the sample target and the margins to calculate
//the divisor is the result of (100 divided by the given margin) which effectively returns the number by which to divide the surface
sf::Vector2f window::calculateMargins(sf::RectangleShape target, float procent_margin_w, float procent_margin_h ){
    sf::Vector2f divisor = {100/procent_margin_w,100/procent_margin_h};
    //returns the position after the caltulated margins
    return sf::Vector2f({target.getSize().x/divisor.x,target.getSize().y/divisor.y}); 
}
//Create the install button
void window::createInstallButton(){
    //this value determines how much above the main window the install button will be drawn, starting from the bottom point
    const uint offset_y = 20;
    const uint installText_y_difference = 11;
    //set the position, size, outline color, color and thickness of the install button
    installButton.setOutlineThickness(2);
    installButton.setSize(sf::Vector2f({c_gui.getChildWindow()->getSize().x - installButton.getOutlineThickness(),100}));
    installButton.setPosition({c_gui.getChildWindow()->getPosition().x + installButton.getOutlineThickness()/2, resolution_y - installButton.getSize().y - credits.getLocalBounds().height -offset_y});
    installButton.setOutlineColor(sf::Color(0,153,76,50));
    installButton.setFillColor(sf::Color(204,255,255,50));
    //Set the font,size,color and position of the Text that will appear on the install button.
    std::string fontpath = EXEC_PATH + "assets/fonts/Aller_Bd.ttf";
    if (!font.loadFromFile(fontpath)){logfile<< "Faled to load Font" << std::endl; exit(EXIT_FAILURE);}
    installText.setFont(font);
    installText.setFillColor(sf::Color::Black);
    installText.setCharacterSize(50);
    installText.setString("INSTALL");
    installText.setPosition(sf::Vector2f({installButton.getPosition().x + installButton.getSize().x/2 - installText.getLocalBounds().width/2,
    installButton.getPosition().y + installButton.getSize().y/2 - installText.getLocalBounds().height/2 - 11}));
}


    //This function is effectively activated on click and activates the binary responsible for the installation of the ticked software
    void window::install() {
        //lock mutex for thread protection
        std::lock_guard<std::mutex> guard(installMutex);

        //if there are no ticked checkboxes, do nothing.
        if(c_gui.getTickedWidgets().size() == 0){
            return;
        }
        if(c_gui.queryErr() == true){
            return;
        }
        //making sure we are not able to make another thread before the current one is finished
        isDone = false;
        //disable the checkboxes for input while installing
        c_gui.disableCheckBoxes();
        //Here we will get each one of the strings from the ticked widgets, then push them into the commandVector
        //we will then create a pointer to the data of commandVector, so that we can get a C-style vector array pointer
        //then we will send it to execv as it expects an array pointer
        std::string widgetString;
        std::vector<char*> commandVector;
        char* dynamicptr;
        size_t sizeofString;
        std::string path = EXEC_PATH + "assets/install";
        int i_uid = getuid();
        std::string uid = std::to_string(i_uid);

        //defining the first known and static commands on the stack
        //the command is as such: /usr/bin/sudo /path/to/executable arg1 arg2 arg3 ... NULL
        //fisrt part
        commandVector.push_back(const_cast<char*>("/usr/bin/sudo"));
        // /path/to/executable
        commandVector.push_back(const_cast<char*>(path.c_str()));
        //this is the first argument of the program
        commandVector.push_back(const_cast<char*>("$$"));
        //this is the second argument
        commandVector.push_back(const_cast<char*>(uid.c_str()));
        //this is the third argument
        commandVector.push_back(const_cast<char*>(LOGPATH.c_str()));
        int dynamicAllocIndex = commandVector.size();
       

        //now we are getting to the actual arguments which are the tickboxes, since those are not static, those will be dynamically allocated
        //looping through the ticketWidgets and pushing the strings to the vector
        for (int i=0; i<c_gui.getTickedWidgets().size();i++){
            widgetString = c_gui.getTickedWidgets().at(i)->getText();
            //adding a null terminator to each string, as it looks like it's not null terminated by default
            widgetString.push_back('\0');
            sizeofString = widgetString.size();
            //allocating new memory with the size of each string
            dynamicptr = (char*)malloc(sizeofString);
            if(dynamicptr == NULL){logfile << "Failed to allocate memory" <<std::endl;exit(-1);}
            //copying the string from each iteration of the string that we get from each ticked widget to the newly allocated memory.
            //we have to use strcpy because we want to copy the contencts that the string pointer is pointing on, not the pointer itself
            strcpy(dynamicptr, (char*)widgetString.c_str());
            //finally pushing back the newly allocated memory to the vector
            commandVector.push_back(dynamicptr);
        }
        //here for the last place, we are pushing back a null pointer as the execv function expects
        commandVector.push_back(NULL);
        //creating a pointer to point to the address of underlying data inside the vector
        char** command = commandVector.data();

        //closing the logfile, so that our fork won't write into it again
        logfile.close();
        //forking the process, so we can run a new process in the child process
        pid_t pid = fork();
        if(pid == -1) {
            logfile << "Failed to fork the process" << std::endl; 
            exit(-1);
        }
        int status;
        if(pid == 0){
            //we are openning the logfile again, but we are opening it with an fd from the child process, so now the child writes to it through his own fd.
            //only logging the stderr from opening the file into the logfile, the stdout will be logged directly from the program
            int logfilefd = open((char*)LOGPATH.c_str(), O_WRONLY|O_APPEND);
            if(logfilefd == -1) {std::cerr << "Failed to open the logfile" << '\n';}
            //we are redirecting all stderr to the file
            dup2(logfilefd, STDERR_FILENO);
            //closing the fd
            close(logfilefd);
            //executing the child process command
            execv(command[0], command);
            }
        //waiting for it to finish, then updating the tickboxes on the changes
        waitpid(pid, &status, 0);
        //opening the logfile again to enable logging
        openLogFile(LOGPATH);
                        
        //here we are getting the exist status of the child process
        if(WIFEXITED(status)){
            int exit_status = WEXITSTATUS(status);
            logfile << "Exit status of the child was: " << exit_status << std::endl; 
            //freeing only the memory that's been allocated with malloc
            for (int i=dynamicAllocIndex;i<commandVector.size()-1;i++){
                free(commandVector.at(i));  
            }
            //clearing the vector
            commandVector.clear();
            if(exit_status != 0){
                std::cout << "Some error has happened. Please check the Log at \n" + LOGPATH +".\n";
                c_gui.showErr();
                c_gui.disableCheckboxWindow();
            }
        }
        

        //marking the installation attempt as done
        isDone=true;
        //check for changes
        c_gui.checkInstalled();
        //disable the installing text
        c_gui.showInstallingText(isDone);
        //update the GUI to the newly installed SW
        c_gui.handleInstalled();
        //enable checkboxes after installing
        c_gui.enableCheckBoxes();

    }

//This function checks if the mouse is hovering inside the install button, if not then return to the original color
void window::retrieveColor(){
    if(!installButton.getGlobalBounds().contains(sf::Vector2f(mouseCoords)))
    installButton.setFillColor(sf::Color(204,255,255,50));
}

//This function sets the little text under the install button
void window::setCredits(){
credits.setFont(font);
credits.setString("Created By Bogdan Okner");
credits.setCharacterSize(13);
credits.setPosition(resolution_x/2 - credits.getGlobalBounds().width/2,installButton.getPosition().y + installButton.getSize().y + 1);
credits.setFillColor(sf::Color::Black);
}

//This function creates a titlebar just above the child window, so it looks more natural
void window::setTitleBar(){
    std::string name = shell::execCommand("getent passwd `id -u` |awk -F: '{print $5}' |awk '{print $1}'");
    name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
    titleBar.setFont(font);
    titleBar.setString("Hello " + name + ", have a nice day!");
    titleBar.setCharacterSize(18);
    titleBar.setPosition(resolution_x/2 - titleBar.getGlobalBounds().width/2, 6);
    titleBar.setFillColor(sf::Color::Black);
}

window::~window() {
//all dynamically allocated objects will get destroyed since they are shared objects.
}