#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>

#include "button.h"

void dodgeGame();

int main()
{

    dodgeGame();

    return 0;
    sf::RenderWindow mainMenu(sf::VideoMode({ 500, 500 }), "Main Menu");

//Buttons created for later use__________________________________
    Button button1{ "ButtonGame1" };
    std::string Button1Text{ "Game 1" };
    button1.setButtonText(Button1Text);
    
    Button button2{ "ButtonGame2" };
    std::string Button2Text{ "Game 2" };
    button2.setButtonText(Button2Text);
  
    Button quitButton{ "ButtonQuit" };
    std::string quitButtonText{ "Quit" };
    quitButton.setButtonText(quitButtonText);
    

    //Button Layout______________________________________________
   /* std::vector<Button> buttonsForMiddleLayout{button1, button2};
    int amountOfButtons{ static_cast<int>(std::ssize(buttonsForMiddleLayout)) };

    for (int index{ 0 }; index < std::ssize(buttonsForMiddleLayout); ++index)
    {
        
    }*/
    
    button1.setPosition({ 100,250 });
    button2.setPosition({ 300,250 });
    quitButton.setPosition({ 200,350 });

//Start of Loop__________________________________________________
    while (mainMenu.isOpen())
    {
        while (const std::optional event = mainMenu.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                mainMenu.close();

            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                if (button1.isHovered(mainMenu))
                {
                    button1.setButtonFillColor(sf::Color::Cyan);
                }
                else
                {
                    button1.setButtonFillColor(sf::Color::White);
                }

                if (button2.isHovered(mainMenu))
                {
                    button2.setButtonFillColor(sf::Color::Cyan);
                }
                else
                {
                    button2.setButtonFillColor(sf::Color::White);
                }

                if (quitButton.isHovered(mainMenu))
                {
                    quitButton.setButtonFillColor(sf::Color::Cyan);
                }
                else
                {
                    quitButton.setButtonFillColor(sf::Color::White);
                }
            }

            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Right || mouseButtonPressed->button == sf::Mouse::Button::Left)
                { 
                    if (button1.onPressed(mainMenu))
                    {
                        
                    }

                    if (button2.onPressed(mainMenu))
                    {
                        
                    }

                    if (quitButton.onPressed(mainMenu))
                    {
                        mainMenu.close();
                    }
                    
                    
                }
            }

        }

        mainMenu.clear();
        mainMenu.draw(button1);
        mainMenu.draw(button2);
        mainMenu.draw(quitButton);
        mainMenu.display();
    }
}