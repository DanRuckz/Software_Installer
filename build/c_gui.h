#pragma once
#include "globals.h"
#include "inits.h"

class C_gui{

    public:
    C_gui();
    //functions are explained in the cpp file
    tgui::Gui gui;
    tgui::ChildWindow::Ptr getChildWindow();
    void setGuiTheme();
    void setGuiTarget(sf::RenderWindow&);
    void createChildWindow(sf::Vector2f position);
    void moveTickBoxes();
    void updateCurrentScrollBarValue();
    void scrollInChildWindow(float delta);
    void checkInstalled();
    bool getScrollBarState();
    std::vector<tgui::CheckBox::Ptr> getTickedWidgets();
    uint getNewScrollbarValue();   
    uint getCurrentScrollBarValue();
    tgui::Scrollbar::Ptr getScrollBar();
    void handleInstalled();
    std::future<void> waitFuture;
    void disableCheckBoxes();
    void enableCheckBoxes();
    void showInstallingText(bool done);
    void createErrWindow();
    void showErr();
    bool queryErr();
    void disableCheckboxWindow();
    ~C_gui();

    private:
    //functions
    void hideErr();
    void createTickBoxes();
    inline void createScrollBar();
    tgui::ChildWindow::Ptr getCheckBoxWindow();
    static void signalHandler_ScrollbarScrolled(bool *scrollbarValueChangedPtr,tgui::Widget::Ptr ScrollBar, const std::string& signalName);
    //objects and variables
    tgui::Theme theme;
    tgui::Theme errTheme;
    struct InstalledProgramData;
    struct Labels;  
    std::vector<tgui::Widget::Ptr> widgets;
    std::vector<tgui::CheckBox::Ptr> checkboxes;
    std::vector<json> programDataContainer;
    std::vector<InstalledProgramData> installedPrograms;
    tgui::ChildWindow::Ptr CheckBoxWindow;
    static tgui::ChildWindow::Ptr (*CheckBoxWindowPtr);
    tgui::Scrollbar::Ptr scrollbar;
    tgui::CheckBox::Ptr checkbox;
    tgui::ChildWindow::Ptr errWindow;
    tgui::Label::Ptr errLabel;
    tgui::Button::Ptr errButton;
    std::vector<tgui::CheckBox::Ptr> checked_vec;
    sf::RenderWindow window_target;
    sf::RenderWindow m_background; 
    json programs;
    static constexpr float checkBoxInterval = 40;
    uint currentScrollBarValue;
    uint numberofElements=0;
    static constexpr uint ScrollPixels = 40;
    static constexpr uint CheckBoxSize = 40;
    static constexpr uint ChildWindowSize_x = 500;
    static constexpr uint ChildWindowSize_y = 400;
    static constexpr float installedTextOffset = 320;
    static constexpr float installingTextOffset = 180;
    static constexpr uint checkboxTextSize = 7;
    static constexpr uint installTextSize = 12;
    bool * scrollValueStatePtr;
    bool scrollValueState = false;
    static constexpr uint scrollbarSize_x = 18;
    uint numberofTickBoxes;
    inline void openProgramsFile();
    std::mutex loaderMutex;
    static constexpr float installingOpacity = 0.8;
    static constexpr float installedOpacity = 0.5;
    std::shared_ptr<Labels> label;
    void setTextIterationToZero();
    inline void createErrButton();
    static constexpr uint errWindow_x = 450;
    static constexpr uint errWindow_y = 90;
    void popError(std::string message);
    static constexpr uint errButtonSize_x = 70;
    static constexpr uint errButtonSize_y = 35;
    //under construction
    static void buttonPressedSignal(tgui::ChildWindow::Ptr *CheckBoxWindow,tgui::ChildWindow::Ptr *errWindowptr,tgui::Widget::Ptr widget, const std::string& signalName);
    inline void createErrMsg();
};

struct C_gui::InstalledProgramData{

    std::string programName;
    std::string packageName;
    std::string repoVersion;
};

struct C_gui::Labels{

    tgui::Label::Ptr installingLabel;
    tgui::Label::Ptr installedLabel;
    static constexpr uint installingTextDots = 70;
    uint installingTextIteration = 0;
};