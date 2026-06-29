#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>

#include "button.h"

void dodgeGame(bool& playAgain);

int main()
{
    sf::RenderWindow mainMenu(sf::VideoMode({ 500, 500 }), "Main Menu");

    sf::Font font{ "arial.ttf" };

//Buttons created for later use__________________________________
    Button button1{ "ButtonGame1" };
    std::string Button1Text{ "Play" };
    button1.setButtonText(Button1Text);
    
    Button quitButton{ "ButtonQuit" };
    std::string quitButtonText{ "Quit" };
    quitButton.setButtonText(quitButtonText);
    

    sf::Text gameText{ font, "Dodge Game", 80 };
    gameText.setPosition({ 12.f, 60.f });

    //Button Layout______________________________________________
   /* std::vector<Button> buttonsForMiddleLayout{button1, button2};
    int amountOfButtons{ static_cast<int>(std::ssize(buttonsForMiddleLayout)) };

    for (int index{ 0 }; index < std::ssize(buttonsForMiddleLayout); ++index)
    {
        
    }*/
    
    button1.setPosition({ 230,300 });
    quitButton.setPosition({ 230,400 });

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
                        mainMenu.setVisible(false);
                        bool playAgain{ true };
                        while (playAgain)
                        {
                            dodgeGame(playAgain);
                        }
                        mainMenu.setVisible(true);
                    }

                    if (quitButton.onPressed(mainMenu))
                    {
                        mainMenu.close();
                    }
                    
                    
                }
            }

        }

        mainMenu.clear();
        mainMenu.draw(gameText);
        mainMenu.draw(button1);
        mainMenu.draw(quitButton);
        mainMenu.display();
    }
}