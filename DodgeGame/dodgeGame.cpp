#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cmath>


class Bullet : public sf::Drawable, public sf::Transformable
{
public:
	bool isActive{ true };
	sf::Vector2f direction{};

	Bullet()
	{
		initialTransform();
	}

	void checkIfStillOnScreen(sf::RenderWindow& currentWindow)
	{
		if (Bullet::getPosition().x > currentWindow.getSize().x || Bullet::getPosition().y > currentWindow.getSize().y)
		{
			isActive = false;
		}
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();
		
		target.draw(m_bullet, states);
		
	}

	void initialTransform()
	{
		m_bullet.setOrigin(m_bullet.getGeometricCenter());
	}

	sf::CircleShape m_bullet{ 2.f };
};

class Player : public sf::Drawable, public sf::Transformable
{
public:
	sf::Clock shootingCoowldown;

	Player()
	{
		initialTransform();
	}

	Bullet shoot()
	{
			Bullet bullet{};
			shootingCoowldown.restart();
			return bullet;
				
	}

	sf::Vector2f distanceToTopFromCenter()
	{
		//return {m_playerModel.getOrigin().x + (m_playerModel.getLocalBounds().size.x / 10.f) ,m_playerModel.getOrigin().y + (m_playerModel.getLocalBounds().size.y / 10.f) };
		return { m_playerModel.getOrigin().x + 1.f ,m_playerModel.getOrigin().y + 1.f };
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

sf::Vector2f normalize(sf::Vector2f vec)
{
	float magnitude{ std::sqrt((vec.x * vec.x) + (vec.y * vec.y)) };

	return { vec.x / magnitude, vec.y / magnitude }; 
}


void dodgeGame()
{
	sf::RenderWindow dodgeGameWindow(sf::VideoMode({ 1000,1000 }), "Dodge Game");
	dodgeGameWindow.setFramerateLimit(144);

	sf::Clock gameTimer;
	sf::Clock delta;
	
	Player player{};
	
	player.setPosition({ dodgeGameWindow.getSize().x / 2.f, dodgeGameWindow.getSize().y / 2.f });
	float shotCooldown{ 0.5f };

	std::vector<Bullet> bulletsOnScreen{};
	sf::Vector2f directionOfBullet{};

	sf::Vector2f mousePosition{};
	sf::Vector2f playerPosition{};

	float directionOfPlayerInDeg{};

	float speed{ 400.0f};
	float bulletSpeed{ 600.0f };
	

	while (dodgeGameWindow.isOpen() && gameTimer.isRunning()) 
	{
		//DeltaT
		sf::Time deltaT = delta.getElapsedTime();
		delta.restart();

		//Removing inactive bullets
		for (int index{ 0 }; index < std::ssize(bulletsOnScreen); ++index)
		{
			if (!bulletsOnScreen.at(index).isActive)
			{
				bulletsOnScreen.erase(bulletsOnScreen.begin()+index);
			}
		}

		dodgeGameWindow.clear();

		//Player Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
		{
			player.move({ 0.f, -(speed * deltaT.asSeconds())});
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
		{
			player.move({ -(speed * deltaT.asSeconds()), 0.f });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
		{
			player.move({ 0.f, (speed * deltaT.asSeconds()) });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
		{
			player.move({ (speed * deltaT.asSeconds()), 0.f });
		}

		//Player Direction Update
		mousePosition = dodgeGameWindow.mapPixelToCoords(sf::Mouse::getPosition(dodgeGameWindow));
		playerPosition = player.getPosition();
		directionOfPlayerInDeg = sf::radians(atan2(playerPosition.x - mousePosition.x, playerPosition.y - mousePosition.y)).asDegrees();

		player.setRotation(sf::degrees(-directionOfPlayerInDeg));

		directionOfBullet = normalize(mousePosition - playerPosition);
		//Player Actions
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			if (player.shootingCoowldown.getElapsedTime().asSeconds() >= shotCooldown)
			{
				sf::Vector2f offset{ (directionOfBullet.x * player.distanceToTopFromCenter().x), (directionOfBullet.y * player.distanceToTopFromCenter().y) };
				Bullet bullet{ player.shoot() };
				bullet.setPosition(playerPosition + offset);
				bullet.direction = directionOfBullet;

				bulletsOnScreen.push_back(bullet);
			}
			
		}

		
		
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
					
				}
			}

			if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButtonPressed->button == sf::Mouse::Button::Left)
				{
					
				}
			}
		}
		
		dodgeGameWindow.draw(player);

		for (auto& bullet : bulletsOnScreen)
		{
			bullet.checkIfStillOnScreen(dodgeGameWindow);
			if (bullet.isActive)
			{
				bullet.move({ bullet.direction * bulletSpeed * deltaT.asSeconds()});
				dodgeGameWindow.draw(bullet);
			}
		}

		
		dodgeGameWindow.display();
		
	}
}