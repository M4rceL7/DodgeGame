#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>


class Button : public sf::Drawable, sf::Transformable 
{
public:
    Button()
    {
        putButtonTextIntoButton();
    }

    /*sf::RectangleShape& getButtonShape()
    {
        return button;
    }*/

    

private:
    
    void putButtonTextIntoButton()
    {
        buttonText.setPosition(button.getOrigin());
        buttonText.setFillColor(sf::Color::Black);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(button, states);
        target.draw(buttonText, states);
    }

    std::string buttonID{" "};
    const sf::Font font{ "arial.ttf" };
    sf::RectangleShape button{ sf::Vector2f(100,50) };
    sf::Text buttonText{ font, "text" };
};


int main()
{
    
    Button button1{};

    sf::RenderWindow mainMenu(sf::VideoMode({ 1000, 1000 }), "SFML works!");

    while (mainMenu.isOpen())
    {
        while (const std::optional event = mainMenu.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                mainMenu.close();
        }

        //mainMenu.clear();
        mainMenu.draw(button1);
        mainMenu.display();
    }
}