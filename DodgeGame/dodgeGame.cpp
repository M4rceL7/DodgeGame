#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

#include "Random.h"

class Bullet : public sf::Drawable, public sf::Transformable
{
public:
	bool isActive{ true };
	sf::Vector2f direction{};

	Bullet()
	{
		initialTransform();
	}

	void checkIfStillOnScreen(sf::RenderWindow& currentWindow, Bullet& bullet)
	{
		if (bullet.getPosition().x > currentWindow.getSize().x || bullet.getPosition().y > currentWindow.getSize().y || bullet.getPosition().x < 0 || bullet.getPosition().y < 0)
		{
			bullet.isActive = false;
		}
	}

	sf::FloatRect getGlobalBounds() const
	{
		return getTransform().transformRect(m_bullet.getLocalBounds());
	}

	float getRadius()
	{
		return m_bullet.getRadius();
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

//---------------------------------------------------------------------------------------------------------

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

	sf::FloatRect getGlobalBounds() const
	{
		return getTransform().transformRect(m_playerModel.getLocalBounds());
	}

	sf::Vector2f distanceToTopFromCenter()
	{
		//return {m_playerModel.getOrigin().x + (m_playerModel.getLocalBounds().size.x / 10.f) ,m_playerModel.getOrigin().y + (m_playerModel.getLocalBounds().size.y / 10.f) };
		return { m_playerModel.getOrigin().x + 1.f ,m_playerModel.getOrigin().y + 1.f };
	}

	int& getHealthPoints()
	{
		return m_healthPoints;
	}

	void setHealthPoints(int& hp)
	{
		m_healthPoints = hp;
	}

	void checkIfGameOver()
	{
		if (m_healthPoints <= 0)
		{
			std::cout << "Game Over \n";
		}
	}

	int& getScore()
	{
		return m_points;
	}

	void setScore(int& score)
	{
		m_points = score;
	}

	float getRadius()
	{
		return m_playerModel.getRadius();
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

	sf::CircleShape m_playerModel{ 30.f, 3 };
	int m_healthPoints{ 100 };
	int m_points{ 0 };
};

//----------------------------------------------------------------------------------------------
class damagingCloud : public sf::Drawable, public sf::Transformable
{
public:
	sf::Clock timeBeforeDamageDealt;
	sf::Clock timeAlive;
	float duration{static_cast<float>(Random::get(15,30))};

	damagingCloud() 
	{
		getRandomSize();
		getRandomRotation();
		initialTransform();
		m_damageArea.setFillColor(sf::Color::Red);
	}

	void initialTransform()
	{
		m_damageArea.setOrigin(m_damageArea.getGeometricCenter());
	}

	void getRandomRotation()
	{
		float randomAngleInDeg{ static_cast<float>(Random::get(0,359)) };
		m_damageArea.setRotation(sf::degrees(randomAngleInDeg));
	}

	void getRandomSize()
	{
		float x{static_cast<float>(Random::get(minSize, maxSize))};
		float y{ static_cast<float>(Random::get(minSize, maxSize)) };

		m_damageArea.setSize({ x,y });
	}

	sf::FloatRect getGlobalBounds() const
	{
		return getTransform().transformRect(m_damageArea.getLocalBounds());
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		target.draw(m_damageArea, states);
	}

	int minSize{ 50 };
	int maxSize{ 250 };
	sf::RectangleShape m_damageArea{};
};

//------------------------------------------------------------------------------------------------------------------

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
		if (Asteroid::getPosition().x > currentWindow.getSize().x || Asteroid::getPosition().y > currentWindow.getSize().y || Asteroid::getPosition().x < 0 || Asteroid::getPosition().y < 0)
		{
			isActive = false;
		}
	}

	sf::FloatRect getGlobalBounds() const
	{
		return getTransform().transformRect(m_asteriod.getLocalBounds());
	}

	float getRadius()
	{
		return m_asteriod.getRadius();
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

	sf::CircleShape m_asteriod{ static_cast<float>(Random::get(15, 35))};
};

//---------------------------------------------------------------------------------------------------------------------

sf::Vector2f normalize(sf::Vector2f vec)
{
	float magnitude{ std::sqrt((vec.x * vec.x) + (vec.y * vec.y)) };

	return { vec.x / magnitude, vec.y / magnitude }; 
}

//Random Asteroids here
sf::Vector2f randomPositionAndDirectionOutsideOfScreen(int randomNumber, sf::RenderWindow& currentWindow, Asteroid& asteroid)
{
	float x{ };
	float y{ };
	sf::Vector2f temp{};
	switch (randomNumber)
	{
	case 1: // Left of screen
		x = static_cast<float>(Random::get(-50, 0));
		y = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y)));
		asteroid.direction = normalize({static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x))), y});
		return { x, y };
	case 2: // Right of screen
		x = static_cast<float>(Random::get(static_cast<int>(currentWindow.getSize().x), static_cast<int>(currentWindow.getSize().x) + 50));
		y = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y)));
		temp = normalize({ static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x))), y });
		asteroid.direction = { (temp.x) * -1.0f, temp.y };
		return { x, y };
	case 3: // Top of screen
		x = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x)));
		y = static_cast<float>(Random::get(- 50, 0));
		asteroid.direction = normalize({ x, static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y))) });
		return { x, y };
	case 4: // Bottom of screen
		x = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x)));
		y = static_cast<float>(Random::get(static_cast<int>(currentWindow.getSize().y), static_cast<int>(currentWindow.getSize().y) + 50));
		temp = normalize({x, static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y))) });
		asteroid.direction = { temp.x , (temp.y) * -1.0f };
		return { x, y };
	}
}

//Targeted Asteroids here
sf::Vector2f randomPositionForTargetedAst(int randomNumber, sf::RenderWindow& currentWindow, Asteroid& asteroid)
{
	float x{ };
	float y{ };
	switch (randomNumber)
	{
	case 1: // Left of screen
		x = static_cast<float>( Random::get(-50, 0));
		y = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y)));
		return { static_cast<float>(x), static_cast<float>(y) };
	case 2: // Right of screen
		x = static_cast<float>(Random::get(static_cast<int>(currentWindow.getSize().x), static_cast<int>(currentWindow.getSize().x) + 50));
		y = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y)));
		return { static_cast<float>(x), static_cast<float>(y) };
	case 3: // Top of screen
		x = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x)));
		y = static_cast<float>(Random::get(-50, 0));
		return { static_cast<float>(x), static_cast<float>(y) };
	case 4: // Bottom of screen
		x = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x)));
		y = static_cast<float>(Random::get(static_cast<int>(currentWindow.getSize().y), static_cast<int>(currentWindow.getSize().y) + 50));
		return { x, y };
	}
}

//Collision handling
void bulletCollisionHandling(Bullet& bullet, Asteroid& asteroid, Player& player)
{
	float dx{ asteroid.getPosition().x - bullet.getPosition().x };
	float dy{ asteroid.getPosition().y - bullet.getPosition().y };
	float dr{ asteroid.getRadius() + bullet.getRadius() };
	if ((dx * dx) + (dy * dy) < dr * dr)
	{
		bullet.isActive = false;
		asteroid.isActive = false;
		player.setScore(player.getScore() += 1);
	}
	
}

void playerCollision(Asteroid& asteroid, Player& player)
{
	float dx{ asteroid.getPosition().x - player.getPosition().x };
	float dy{ asteroid.getPosition().y - player.getPosition().y };
	float dr{ asteroid.getRadius() + player.getRadius() };
	if ((dx * dx) + (dy * dy) < dr * dr)
	{
		asteroid.isActive = false;
		player.setHealthPoints(player.getHealthPoints() -= 10);
		player.checkIfGameOver();
	}
}

bool playerInCloud(damagingCloud& dmgc, Player& player)
{
	auto playerCenter{ player.getPosition() };
	auto playerRadius{ player.getRadius() -15.f};

	sf::Vector2f cloudCenter{ dmgc.getPosition() };
	sf::Angle cloudRotation{ dmgc.getRotation() };
	float cloudHalfX{ (dmgc.getGlobalBounds().size.x / 2.f)};
	float cloudHalfY{ (dmgc.getGlobalBounds().size.y / 2.f)};

	sf::Vector2f CloudToPlayer{ playerCenter - cloudCenter };

	float localX{ (CloudToPlayer.x * cos(-cloudCenter.angle().asRadians()) - CloudToPlayer.y * sin(-cloudCenter.angle().asRadians())) };
	float localY{ (CloudToPlayer.x * sin(-cloudCenter.angle().asRadians()) + CloudToPlayer.y * cos(-cloudCenter.angle().asRadians())) };

	auto closestX{ std::clamp(localX, -cloudHalfX, cloudHalfX) };
	auto closestY{ std::clamp(localY, -cloudHalfY, cloudHalfY) };

	auto dx{ localX - closestX };
	auto dy{ localY - closestY };

	if ((dx * dx) + (dy * dy) <= (playerRadius * playerRadius))
	{
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------
void dodgeGame()
{
	sf::RenderWindow dodgeGameWindow(sf::VideoMode({ 1000,1000 }), "Dodge Game");

	sf::Clock gameTimer;
	sf::Clock delta;
	sf::Clock astSpawnTimer;
	sf::Clock targetedSpawnTimer;
	sf::Clock damagingCloudSpawnTimer;
	sf::Clock cloudDamageCoolDownTimer;
	
	Player player{};
	const sf::Font font{ "arial.ttf" };
	sf::Text hpText{font, "HP: "};
	sf::Text scoreText{ font, "Score: "};
	sf::Text timeText{ font, "Time: "};
	int displayTime{};

	scoreText.setOrigin(scoreText.getLocalBounds().getCenter());
	scoreText.setPosition({ dodgeGameWindow.getSize().x / 2.f, 20.0f});
	timeText.setPosition({ static_cast<float>(dodgeGameWindow.getSize().x - 200), 0.0f});

	
	player.setPosition({ dodgeGameWindow.getSize().x / 2.f, dodgeGameWindow.getSize().y / 2.f });
	float shotCooldown{ 0.5f }; 

	std::vector<Bullet> bulletsOnScreen{};
	std::vector<Asteroid> asteroidsOnScreen{};
	std::vector<Asteroid> targetedAsteroidOnScreen{};

	std::vector<damagingCloud> cloudsOnScreen{};
	int amountOfCloudsAllowed{ 3 };

	sf::Vector2f directionOfBullet{};
	sf::Vector2f mousePosition{};
	sf::Vector2f playerPosition{};

	float directionOfPlayerInDeg{};

	float speed{ 400.0f};
	float bulletSpeed{ 900.0f };
	float asteroidSpeed{ static_cast<float>(Random::get(150, 800)) };

	float randSpawnTimerAst{ 1.0f };
	float randSpawnTimerAstTargeted{ 2.0f };
	float randSpawnTimerDagamingCloud{10.f};

	while (dodgeGameWindow.isOpen() && gameTimer.isRunning()) 
	{
		//Preset Data that is needed every loop rotation
		displayTime = static_cast<int>(gameTimer.getElapsedTime().asSeconds());
		timeText.setString("Time: " + std::to_string(displayTime));
		scoreText.setString("Score: " + std::to_string(player.getScore()));
		hpText.setString("HP: " + std::to_string(player.getHealthPoints()));

		mousePosition = dodgeGameWindow.mapPixelToCoords(sf::Mouse::getPosition(dodgeGameWindow));
		playerPosition = player.getPosition();
		directionOfPlayerInDeg = sf::radians(atan2(playerPosition.x - mousePosition.x, playerPosition.y - mousePosition.y)).asDegrees();

		//DeltaT
		sf::Time deltaT = delta.getElapsedTime();
		delta.restart();

		dodgeGameWindow.clear();

		//Removing inactive objects
		for (int index{ 0 }; index < std::ssize(bulletsOnScreen); index++)
		{
			if (!bulletsOnScreen.at(index).isActive)
			{
				bulletsOnScreen.erase(bulletsOnScreen.begin()+index);
			}
		}

		for (int index{ 0 }; index < std::ssize(asteroidsOnScreen); index++)
		{
			if (!asteroidsOnScreen.at(index).isActive)
			{
				asteroidsOnScreen.erase(asteroidsOnScreen.begin() + index);
			}
		}

		for (int index{ 0 }; index < std::ssize(targetedAsteroidOnScreen); index++)
		{
			if (!targetedAsteroidOnScreen.at(index).isActive)
			{
				targetedAsteroidOnScreen.erase(targetedAsteroidOnScreen.begin() + index);
			}
		}

		for (int index{ 0 }; index < std::ssize(cloudsOnScreen); index++)
		{
			if (cloudsOnScreen.at(index).timeAlive.getElapsedTime().asSeconds() > cloudsOnScreen.at(index).duration)
			{
				cloudsOnScreen.erase(cloudsOnScreen.begin() + index);
			}
		}
		

		//GamePlay settings (spawning and moving of asteriods, damaging clouds)
		if (randSpawnTimerAst <= astSpawnTimer.getElapsedTime().asSeconds())
		{
			Asteroid asteriod{};
			asteriod.deleteSaveTimeC.start();
			asteriod.setPosition({randomPositionAndDirectionOutsideOfScreen(Random::get(1,4), dodgeGameWindow, asteriod)});
			asteroidsOnScreen.push_back(asteriod);

			astSpawnTimer.restart();
		}

		if (randSpawnTimerAstTargeted <= targetedSpawnTimer.getElapsedTime().asSeconds())
		{
			Asteroid asteriod{};
			asteriod.deleteSaveTimeC.start();
			asteriod.setPosition({ randomPositionForTargetedAst(Random::get(1,4), dodgeGameWindow, asteriod) });
			asteriod.direction = normalize(playerPosition - asteriod.getPosition());
			targetedAsteroidOnScreen.push_back(asteriod);

			targetedSpawnTimer.restart();
		}

		if (randSpawnTimerDagamingCloud <= damagingCloudSpawnTimer.getElapsedTime().asSeconds() && std::ssize(cloudsOnScreen) < amountOfCloudsAllowed)
		{
			damagingCloud dmgc{};
			dmgc.setPosition( { static_cast<float>(Random::get(0,dodgeGameWindow.getSize().x)), static_cast<float>(0,Random::get(0,dodgeGameWindow.getSize().x))} );
			dmgc.timeBeforeDamageDealt.start();
			dmgc.timeAlive.start();
			damagingCloudSpawnTimer.restart();

			cloudsOnScreen.push_back(dmgc);
		}

		//Player Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
		{
			if (player.getPosition().y > 0.0f + player.getRadius() && player.getPosition().x > 0.0f)
			{
				player.move({ 0.f, -(speed * deltaT.asSeconds()) });
			}
			
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
		{
			if (player.getPosition().y > 0.0f && player.getPosition().x > 0.0f + player.getRadius())
			{
				player.move({ -(speed * deltaT.asSeconds()), 0.f });
			}
			
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
		{
			if (player.getPosition().y < dodgeGameWindow.getSize().y - player.getRadius() && player.getPosition().x > 0.0f)
			{
				player.move({ 0.f, (speed * deltaT.asSeconds()) });
			}
			
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
		{
			if (player.getPosition().x < dodgeGameWindow.getSize().x - player.getRadius() && player.getPosition().y > 0.0f)
			{
				player.move({ (speed * deltaT.asSeconds()), 0.f });
			}
			
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
					std::cout << std::ssize(bulletsOnScreen);
				}
			}
		}

		//Collision handling
		if (std::ssize(bulletsOnScreen) > 0)
		{
			for (auto& bullet : bulletsOnScreen)
			{
				for (auto& ast : asteroidsOnScreen)
				{	
					if (bullet.isActive && ast.isActive)
					{
						bulletCollisionHandling(bullet, ast, player);
					}

				}
			}
		}
		
		if (std::ssize(bulletsOnScreen) > 0)
		{
			for (auto& bullet : bulletsOnScreen)
			{
				for (auto& ast : targetedAsteroidOnScreen)
				{
					if (bullet.isActive && ast.isActive)
					{
						bulletCollisionHandling(bullet, ast, player);
					}

				}
			}
		}

		if (std::ssize(cloudsOnScreen) > 0)
		{
			for (auto& dmgc : cloudsOnScreen)
			{
				if (playerInCloud(dmgc, player))
				{
					if (cloudDamageCoolDownTimer.getElapsedTime().asSeconds() > 0.5f)
					{
						player.setHealthPoints(player.getHealthPoints() -= 2);
						cloudDamageCoolDownTimer.restart();
						player.checkIfGameOver();
					}
				}
				
			}
		}

		//Rendering
		for (auto& dmgc : cloudsOnScreen)
		{
			if (dmgc.timeAlive.getElapsedTime().asSeconds() < dmgc.duration)
			{
				dodgeGameWindow.draw(dmgc);
			}
		}

		for (auto& bullet : bulletsOnScreen)
		{
			
			bullet.checkIfStillOnScreen(dodgeGameWindow, bullet);
			if (bullet.isActive)
			{
				bullet.move({ bullet.direction * bulletSpeed * deltaT.asSeconds()});
				dodgeGameWindow.draw(bullet);
			}
		}

		for (auto& ast : asteroidsOnScreen)
		{
			asteroidSpeed = static_cast<float>(Random::get(150, 800));
			if (ast.deleteSaveTimeC.getElapsedTime().asSeconds() >= ast.deleteSaveTime)
			{
				ast.checkIfStillOnScreen(dodgeGameWindow);
			}
			if(ast.isActive)
			{
				ast.move({ ast.direction * asteroidSpeed  * deltaT.asSeconds() });
				playerCollision(ast, player);
				dodgeGameWindow.draw(ast);
			}
		}

		for (auto& ast : targetedAsteroidOnScreen)
		{
			asteroidSpeed = static_cast<float>(Random::get(150, 800));
			if (ast.deleteSaveTimeC.getElapsedTime().asSeconds() >= ast.deleteSaveTime)
			{
				ast.checkIfStillOnScreen(dodgeGameWindow);
			}
			if (ast.isActive)
			{
				ast.move({ ast.direction * asteroidSpeed * deltaT.asSeconds() });
				playerCollision(ast, player);
				dodgeGameWindow.draw(ast);
			}
		}

		dodgeGameWindow.draw(player);
		dodgeGameWindow.draw(scoreText);
		dodgeGameWindow.draw(hpText);
		dodgeGameWindow.draw(timeText);
		dodgeGameWindow.display();
		
	}
}