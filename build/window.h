#pragma once
#include "inits.h"
#include "c_gui.h"
#include "error.h"
#include "globals.h"

class window
{
public:
window();
~window();
private:
//the functions are explained in the cpp file
inline void setstage();
void runWindow();
void setText();
sf::Vector2f calculateMargins(sf::RectangleShape, float margin_w, float margin_h);
void createInstallButton();
void install();
void setIcon();
void setCredits();
void setTitleBar();
void retrieveColor();
//the sf::* are the drawable/transformable objects that are used, except the RenderWindow, Font and Texture, which are loaded for the various transformables/drawables.
sf::RenderWindow container_window;
sf::RectangleShape background;
sf::Text title;
sf::Text installText;
sf::Text credits;
sf::Font font;
sf::Texture arrowTexture;
sf::Sprite frontArrow;
sf::Sprite backArrow;
sf::Image icon;
sf::Text titleBar;
sf::RectangleShape installButton;
//event handling object
sf::Event event;
//mouse coordinations object
sf::Vector2i mouseCoords;
bool colorChanged = false;
//The resolution of the screen is defined here
static constexpr uint resolution_x = 510;
static constexpr uint resolution_y = 582;
std::mutex installMutex;
volatile bool isDone = true;
sf::Clock globalTimer;
sf::Time globalTime;
bool successInstall = true;
//the object that contains the TGUI functionality implementation within the program
C_gui c_gui;
};
