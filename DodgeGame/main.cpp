#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

const sf::Font font("arial.ttf");

class Button : public sf::Drawable
{
public:
    Button()
    {}

private:
    sf::RectangleShape button{ sf::Vector2f(100,50) };
    sf::Text label{ font, "text", 15 };
};


int main()
{
    sf::RenderWindow mainMenu(sf::VideoMode({ 200, 200 }), "SFML works!");

    while (mainMenu.isOpen())
    {
        while (const std::optional event = mainMenu.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                mainMenu.close();
        }

        //mainMenu.clear();
        mainMenu.display();
    }
}