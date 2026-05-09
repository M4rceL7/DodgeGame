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
        m_buttonText.setFillColor(sf::Color::Black);
    }

    std::string& getButtonID() { return m_buttonID; }
    void setButtonID(std::string_view buttonID){ m_buttonID = buttonID; }

    sf::Text& getButtonText() { return m_buttonText; }
    void setButtonText(std::string& buttonText)
    { 
        m_buttonText.setString(buttonText);
        putButtonTextIntoButton();
    }

    void setButtonFillColor(sf::Color color)
    {
        m_button.setFillColor(color);
    }

    sf::FloatRect getGlobalBounds() const
    {
        return getTransform().transformRect(m_button.getLocalBounds());
    }

    bool isHovered(sf::RenderWindow& currentWindow)
    {
        sf::Vector2f currentMousePosition{ currentWindow.mapPixelToCoords(sf::Mouse::getPosition(currentWindow))};
        if (Button::getGlobalBounds().contains(currentMousePosition))
        {
            return true;
        }
        return false;
    }

    bool onPressed(sf::RenderWindow& currentWindow)
    {
        sf::Vector2f currentMousePosition{ sf::Mouse::getPosition(currentWindow) };
        if (Button::getGlobalBounds().contains(currentMousePosition))
        {
            return true;
        }
        return false;
    }

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
    sf::Text m_buttonText{ m_font, "text", 20 };
};

void buttonLayout(std::vector<Button>& buttons)
{

}

int main()
{
    sf::RenderWindow mainMenu(sf::VideoMode({ 500, 500 }), "Main Menu");

//Buttons created for later use__________________________________
    Button button1{ "Game1" };
    std::string Button1Text{ "Game 1" };
    button1.setButtonText(Button1Text);
    button1.setPosition({ 100,250 });

    Button button2{ "Game2" };
    std::string Button2Text{ "Game 2" };
    button2.setButtonText(Button2Text);
    button2.setPosition({ 300,250 });

    Button quitButton{ "Quit" };
    std::string quitButtonText{ "Quit" };
    quitButton.setButtonText(quitButtonText);
    quitButton.setPosition( {200,350} );

    std::vector<std::reference_wrapper<Button>> buttonListForHorizontalLayout1{ button1, button2 };

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