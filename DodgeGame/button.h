#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

class Button : public sf::Drawable, public sf::Transformable
{
public:
    Button(std::string_view buttonID) : m_buttonID(buttonID)
    {
        putButtonTextIntoButton();
        m_buttonText.setFillColor(sf::Color::Black);
    }

    std::string& getButtonID() { return m_buttonID; }
    void setButtonID(std::string_view buttonID) { m_buttonID = buttonID; }

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
        sf::Vector2f currentMousePosition{ currentWindow.mapPixelToCoords(sf::Mouse::getPosition(currentWindow)) };
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

        m_button.setSize({ m_buttonText.getLocalBounds().size.x + m_textPaddingToButton, m_buttonText.getLocalBounds().size.y + (m_textPaddingToButton *2) });

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

    std::string m_buttonID{ "button" };
    const sf::Font m_font{ "arial.ttf" };

    sf::RectangleShape m_button{ sf::Vector2f(100,50) };
    sf::Text m_buttonText{ m_font, "text", 20 };

    float m_textPaddingToButton{ 10.f };
};
