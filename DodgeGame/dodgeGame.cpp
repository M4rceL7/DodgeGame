#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

#include "Random.h"

class Asteroid : public sf::Drawable, public sf::Transformable
{
public:
	bool isActive{ true };
	sf::Vector2f direction{};
	sf::Clock deleteSaveTimeC;
	float deleteSaveTime{ 2.0f };

	Asteroid()
	{
		initialTransform();
	}

	void checkIfStillOnScreen(sf::RenderWindow& currentWindow)
	{
		if (Asteroid::getPosition().x > currentWindow.getSize().x || Asteroid::getPosition().y > currentWindow.getSize().y)
		{
			isActive = false;
		}
	}
private:
	void initialTransform()
	{
		m_asteriod.setOrigin(m_asteriod.getGeometricCenter());
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		target.draw(m_asteriod, states);
	}

	sf::CircleShape m_asteriod{ 20.0f };
};

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

sf::Vector2f randomPositionAndDirectionOutsideOfScreen(int randomNumber, sf::RenderWindow& currentWindow, Asteroid& asteroid)
{
	float x{ };
	float y{ };
	sf::Vector2f temp{};
	switch (randomNumber)
	{
	case 1: // Left of screen
		x = Random::get(-50, 0);
		y = Random::get(0, static_cast<int>(currentWindow.getSize().y));
		asteroid.direction = normalize({static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x))), y});
		return { static_cast<float>(x), static_cast<float>(y) };
	case 2: // Right of screen
		x = Random::get(static_cast<int>(currentWindow.getSize().x), static_cast<int>(currentWindow.getSize().x) + 50);
		y = Random::get(0, static_cast<int>(currentWindow.getSize().y));
		temp = normalize({ static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x))), y });
		asteroid.direction = { (temp.x) * -1.0f, temp.y };
		return { static_cast<float>(x), static_cast<float>(y) };
	case 3: // Top of screen
		x = Random::get(0, static_cast<int>(currentWindow.getSize().x));
		y = Random::get(- 50, 0);
		asteroid.direction = normalize({ x, static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y))) });
		return { static_cast<float>(x), static_cast<float>(y) };
	case 4: // Bottom of screen
		x = Random::get(0, static_cast<int>(currentWindow.getSize().x));
		y = Random::get(static_cast<int>(currentWindow.getSize().y), static_cast<int>(currentWindow.getSize().y) + 50);
		temp = normalize({x, static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y))) });
		asteroid.direction = { temp.x , (temp.y) * -1.0f };
		return { static_cast<float>(x), static_cast<float>(y) };
	}
}

sf::Vector2f randomPositionAndDirectionForTargetedAst(int randomNumber, sf::RenderWindow& currentWindow, Asteroid& asteroid)
{
	float x{ };
	float y{ };
	sf::Vector2f temp{};
	switch (randomNumber)
	{
	case 1: // Left of screen
		x = Random::get(-50, 0);
		y = Random::get(0, static_cast<int>(currentWindow.getSize().y));
		
		return { static_cast<float>(x), static_cast<float>(y) };
	case 2: // Right of screen
		x = Random::get(static_cast<int>(currentWindow.getSize().x), static_cast<int>(currentWindow.getSize().x) + 50);
		y = Random::get(0, static_cast<int>(currentWindow.getSize().y));
		

		return { static_cast<float>(x), static_cast<float>(y) };
	case 3: // Top of screen
		x = Random::get(0, static_cast<int>(currentWindow.getSize().x));
		y = Random::get(-50, 0);
		
		return { static_cast<float>(x), static_cast<float>(y) };
	case 4: // Bottom of screen
		x = Random::get(0, static_cast<int>(currentWindow.getSize().x));
		y = Random::get(static_cast<int>(currentWindow.getSize().y), static_cast<int>(currentWindow.getSize().y) + 50);
		

		return { static_cast<float>(x), static_cast<float>(y) };
	}
}


void dodgeGame()
{
	sf::RenderWindow dodgeGameWindow(sf::VideoMode({ 1000,1000 }), "Dodge Game");
	dodgeGameWindow.setFramerateLimit(144);

	sf::Clock gameTimer;
	sf::Clock delta;
	sf::Clock spawnTimer;
	sf::Clock targetedSpawnTimer;
	
	Player player{};
	
	player.setPosition({ dodgeGameWindow.getSize().x / 2.f, dodgeGameWindow.getSize().y / 2.f });
	float shotCooldown{ 0.5f }; 

	std::vector<Bullet> bulletsOnScreen{};
	std::vector<Asteroid> asteroidsOnScreen{};
	std::vector<Asteroid> targetedAsteroidOnScreen{};

	sf::Vector2f directionOfBullet{};
	sf::Vector2f mousePosition{};
	sf::Vector2f playerPosition{};

	float directionOfPlayerInDeg{};

	float speed{ 400.0f};
	float bulletSpeed{ 600.0f };
	float asteroidSpeed{ static_cast<float>(Random::get(150, 800)) };

	float randSpawnTimerAst{ 1.0f };
	float randSpawnTimerAstTargeted{ 2.0f };
	

	while (dodgeGameWindow.isOpen() && gameTimer.isRunning()) 
	{
		//Preset Data that is needed every loop rotation
		mousePosition = dodgeGameWindow.mapPixelToCoords(sf::Mouse::getPosition(dodgeGameWindow));
		playerPosition = player.getPosition();
		directionOfPlayerInDeg = sf::radians(atan2(playerPosition.x - mousePosition.x, playerPosition.y - mousePosition.y)).asDegrees();

		//DeltaT
		sf::Time deltaT = delta.getElapsedTime();
		delta.restart();

		//Removing inactive objects
		for (int index{ 0 }; index < std::ssize(bulletsOnScreen); ++index)
		{
			if (!bulletsOnScreen.at(index).isActive)
			{
				bulletsOnScreen.erase(bulletsOnScreen.begin()+index);
			}
		}

		for (int index{ 0 }; index < std::ssize(asteroidsOnScreen); ++index)
		{
			if (!asteroidsOnScreen.at(index).isActive)
			{
				asteroidsOnScreen.erase(asteroidsOnScreen.begin() + index);
			}
		}

		//GamePlay settings (spawning and moving of asteriods, damaging clouds)
		if (randSpawnTimerAst <= spawnTimer.getElapsedTime().asSeconds())
		{
			Asteroid asteriod{};
			asteriod.deleteSaveTimeC.start();
			asteriod.setPosition({randomPositionAndDirectionOutsideOfScreen(Random::get(1,4), dodgeGameWindow, asteriod)});
			asteroidsOnScreen.push_back(asteriod);

			spawnTimer.restart();
		}

		if (randSpawnTimerAstTargeted <= targetedSpawnTimer.getElapsedTime().asSeconds())
		{
			Asteroid asteriod{};
			asteriod.deleteSaveTimeC.start();
			asteriod.setPosition({ randomPositionAndDirectionOutsideOfScreen(Random::get(1,4), dodgeGameWindow, asteriod) });
			targetedAsteroidOnScreen.push_back(asteriod);

			targetedSpawnTimer.restart();
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
		player.setRotation(sf::degrees(-directionOfPlayerInDeg));

		
		//Player Actions
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			if (player.shootingCoowldown.getElapsedTime().asSeconds() >= shotCooldown)
			{
				directionOfBullet = normalize(mousePosition - playerPosition);
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

		for (auto& ast : asteroidsOnScreen)
		{
			
			if (ast.deleteSaveTimeC.getElapsedTime().asSeconds() >= ast.deleteSaveTime)
			{
				ast.checkIfStillOnScreen(dodgeGameWindow);
			}
			if(ast.isActive)
			{
				ast.move({ ast.direction * asteroidSpeed  * deltaT.asSeconds() });
				dodgeGameWindow.draw(ast);
			}
		}

		for (auto& ast : targetedAsteroidOnScreen)
		{

			if (ast.deleteSaveTimeC.getElapsedTime().asSeconds() >= ast.deleteSaveTime)
			{
				ast.checkIfStillOnScreen(dodgeGameWindow);
			}
			if (ast.isActive)
			{
				ast.move({ ast.direction * asteroidSpeed * deltaT.asSeconds() });
				dodgeGameWindow.draw(ast);
			}
		}

		dodgeGameWindow.display();
		
	}
}