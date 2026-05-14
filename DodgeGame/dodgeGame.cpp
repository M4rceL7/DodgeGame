#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>


class Bullet : public sf::Drawable, public sf::Transformable
{
public:
	Bullet()
	{
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();
		
		target.draw(m_bullet, states);
		
	}

	sf::CircleShape m_bullet{ 2.f };
	
};

class Player : public sf::Drawable, public sf::Transformable
{
public:
	Player()
	{
		initialTransform();
	}

	Bullet shoot()
	{
		
		Bullet bullet{};

		return bullet;
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		target.draw(m_playerModel, states);
	}

	void initialTransform()
	{
		m_playerModel.setOrigin(m_playerModel.getGeometricCenter());

	}

	sf::CircleShape m_playerModel{30.f, 3};
	
};


void dodgeGame()
{
	sf::RenderWindow dodgeGameWindow(sf::VideoMode({ 1000,1000 }), "Dodge Game");

	Player player{};
	player.setPosition({ 150,150 });
	std::vector<Bullet> bulletsOnScreen{};
	sf::Vector2f mousePosition{};
	sf::Vector2f playerPosition{};
	

	while (dodgeGameWindow.isOpen())
	{
		dodgeGameWindow.clear();

		while (const std::optional event = dodgeGameWindow.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				dodgeGameWindow.close();

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scan::Escape)
				{
					dodgeGameWindow.close();
				}
			}

			if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButtonPressed->button == sf::Mouse::Button::Right)
				{
					std::cout << "( " << player.getPosition().x << " / " << player.getPosition().y << " )" << '\n';
				}
			}
		}
		//Player Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
		{
			player.move({ 0.f, -0.1 });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
		{
			player.move({ -0.1, 0.f });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
		{
			player.move({ 0.f, 0.1 });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
		{
			player.move({ 0.1, 0.f });
		}

		//Player Direction Update
		mousePosition = dodgeGameWindow.mapPixelToCoords(sf::Mouse::getPosition(dodgeGameWindow));
		playerPosition = player.getPosition();
		sf::Angle directionOfPlayerInRad = sf::radians(atan2(playerPosition.x - mousePosition.x, playerPosition.y - mousePosition.y));
		float directionOfPlayerInDeg = directionOfPlayerInRad.asDegrees();

		player.setRotation(sf::degrees(-directionOfPlayerInDeg));

		//Player Actions
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			
		}
		
		
		
		dodgeGameWindow.draw(player);
		
		dodgeGameWindow.display();
		
	}
}