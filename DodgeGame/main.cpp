#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

//Button class will be put later into Header file for use in mutliple areas
class Button : public sf::Drawable, public sf::Transformable 
{
public:
    Button(std::string_view buttonID) : m_buttonID(buttonID)
    {
        putButtonTextIntoButton();
    }

    std::string& getButtonID() { return m_buttonID; }
    void setButtonID(std::string_view buttonID){ m_buttonID = buttonID; }

    sf::Text& getButtonText() { return m_buttonText; }
    void setButtonText(std::string& buttonText){ m_buttonText.setString(buttonText); }

private:
    
    void putButtonTextIntoButton()
    {
        sf::FloatRect textBounds = m_buttonText.getLocalBounds();

        sf::Vector2f buttonTextOrigin{ textBounds.position.x + textBounds.size.x / 2.f,
            textBounds.position.y + textBounds.size.y / 2.f };
        sf::Vector2f buttonTextPoition{ m_button.getPosition().x + m_button.getSize().x / 2.f,
        m_button.getPosition().y + m_button.getSize().y / 2.f };

        m_buttonText.setOrigin(buttonTextOrigin);

        m_buttonText.setPosition(buttonTextPoition);

        m_buttonText.setFillColor(sf::Color::Black);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        target.draw(m_button, states);
        target.draw(m_buttonText, states);
    }

    std::string m_buttonID{"button"};
    const sf::Font m_font{ "arial.ttf" };

    sf::RectangleShape m_button{sf::Vector2f(100,50)};
    sf::Text m_buttonText{ m_font, "text" };
};


int main()
{
    
    Button button1{"Test1"};
    button1.setPosition({ 20,50 });

    sf::RenderWindow mainMenu(sf::VideoMode({ 1000, 1000 }), "Main Menu");

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