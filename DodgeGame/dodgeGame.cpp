#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

#include "Random.h"
#include "button.h"

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
		target.draw(m_front, states);
	}

	void initialTransform()
	{
		m_playerModel.setOrigin(m_playerModel.getGeometricCenter());
		m_front.setFillColor(sf::Color::Magenta);
		m_front.setPosition({ -8.f,-30.f });
	}

	sf::CircleShape m_playerModel{ 30.f, 3 };
	sf::CircleShape m_front{ 8.f, 3};
	int m_healthPoints{ 100 };
	int m_points{ 0 };
};

//----------------------------------------------------------------------------------------------
class damagingCloud : public sf::Drawable, public sf::Transformable
{
public:
	sf::Clock timeBeforeDamageDealt;
	sf::Clock timeAlive;
	float duration{ 50 };

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
		float x{ static_cast<float>(Random::get(minSize, maxSize)) };
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
	float ignoringCollisionTime{ 0.1f };
	float speed{};

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

	void setRadius(float newRad)
	{
		m_asteriod.setRadius(newRad);
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

	int m_asteroidRandomSizeMin{ 15 };
	int m_asteroidRandomSizeMax{ 40 };

	sf::CircleShape m_asteriod{ static_cast<float>(Random::get(m_asteroidRandomSizeMin, m_asteroidRandomSizeMax)) };
};

//---------------------------------------------------------------------------------------------------------------------

static bool checkIfGameOver(sf::Clock& gameTimer, Player& player)
{
	if (player.getHealthPoints() <= 0)
	{
		return true;
	}
	return false;
}

static sf::Vector2f normalize(sf::Vector2f vec)
{
	float magnitude{ std::sqrt((vec.x * vec.x) + (vec.y * vec.y)) };

	return { vec.x / magnitude, vec.y / magnitude };
}

//Random Asteroids here
static sf::Vector2f randomPositionAndDirectionOutsideOfScreen(int randomNumber, sf::RenderWindow& currentWindow, Asteroid& asteroid)
{
	float x{ };
	float y{ };
	sf::Vector2f temp{};
	switch (randomNumber)
	{
	case 1: // Left of screen
		x = static_cast<float>(Random::get(-50, 0));
		y = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y)));
		asteroid.direction = normalize({ static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x))), y });
		return { x, y };
	case 2: // Right of screen
		x = static_cast<float>(Random::get(static_cast<int>(currentWindow.getSize().x), static_cast<int>(currentWindow.getSize().x) + 50));
		y = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y)));
		temp = normalize({ static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x))), y });
		asteroid.direction = { (temp.x) * -1.0f, temp.y };
		return { x, y };
	case 3: // Top of screen
		x = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x)));
		y = static_cast<float>(Random::get(-50, 0));
		asteroid.direction = normalize({ x, static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y))) });
		return { x, y };
	case 4: // Bottom of screen
		x = static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().x)));
		y = static_cast<float>(Random::get(static_cast<int>(currentWindow.getSize().y), static_cast<int>(currentWindow.getSize().y) + 50));
		temp = normalize({ x, static_cast<float>(Random::get(0, static_cast<int>(currentWindow.getSize().y))) });
		asteroid.direction = { temp.x , (temp.y) * -1.0f };
		return { x, y };
	}
}

//Targeted Asteroids here
static sf::Vector2f randomPositionForTargetedAst(int randomNumber, sf::RenderWindow& currentWindow, Asteroid& asteroid)
{
	float x{ };
	float y{ };
	switch (randomNumber)
	{
	case 1: // Left of screen
		x = static_cast<float>(Random::get(-50, 0));
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
static bool bulletCollisionHandling(Bullet& bullet, Asteroid& asteroid, Player& player)
{
	float dx{ asteroid.getPosition().x - bullet.getPosition().x };
	float dy{ asteroid.getPosition().y - bullet.getPosition().y };
	float dr{ asteroid.getRadius() + bullet.getRadius() };
	if ((dx * dx) + (dy * dy) < dr * dr)
	{
		bullet.isActive = false;
		asteroid.isActive = false;
		if (asteroid.getRadius() <= 15.0f)
		{
			player.setScore(player.getScore() += 3);
		}
		else if (asteroid.getRadius() >= 16.0f && asteroid.getRadius() <= 25.0f)
		{
			player.setScore(player.getScore() += 2);
		}
		else if (asteroid.getRadius() >= 26.0f)
		{
			player.setScore(player.getScore() += 1);
		}
		return true;
	}
	return false;
}

static void astOnAstCollision(Asteroid& ast1, Asteroid& ast2)
{
	float dx{ ast1.getPosition().x - ast2.getPosition().x };
	float dy{ ast1.getPosition().y - ast2.getPosition().y };
	float dr{ ast1.getRadius() + ast2.getRadius() };
	if ((dx * dx) + (dy * dy) < dr * dr)
	{
		sf::Vector2f normal{ normalize(ast1.getPosition() - ast2.getPosition()) };

		ast1.direction = ast1.direction - 2 * (ast1.direction.x * normal.x + ast1.direction.y * normal.y) * normal;
		ast2.direction = ast2.direction - 2 * (ast2.direction.x * (normal.x * -1.0f) + ast2.direction.y * (normal.y * -1.0f)) * (normal * -1.0f);
	}
}

static bool playerCollision(Asteroid& asteroid, Player& player)
{
	float dx{ asteroid.getPosition().x - player.getPosition().x };
	float dy{ asteroid.getPosition().y - player.getPosition().y };
	float dr{ asteroid.getRadius() + player.getRadius() };
	if ((dx * dx) + (dy * dy) < dr * dr)
	{
		asteroid.isActive = false;
		return true;
	}
	return false;
}

static bool playerInCloud(damagingCloud& dmgc, Player& player)
{
	auto playerCenter{ player.getPosition() };
	auto playerRadius{ player.getRadius() - 15.f };

	sf::Vector2f cloudCenter{ dmgc.getPosition() };
	sf::Angle cloudRotation{ dmgc.getRotation() };
	float cloudHalfX{ (dmgc.getGlobalBounds().size.x / 2.f) };
	float cloudHalfY{ (dmgc.getGlobalBounds().size.y / 2.f) };

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

static float astSpeedBasedOnSize(float& speed, Asteroid& ast)
{
	float astSize{ ast.getRadius() };
	if (astSize <= 15.f)
	{
		return speed * 1.2f;
	}

	if (astSize >= 16.f && astSize <= 29.f)
	{
		return speed;
	}

	if (astSize >= 30.f)
	{
		return speed * 0.6f;
	}
}

//-------------------------------------------------------------------------

void dodgeGame(bool& playAgain)
{
	sf::RenderWindow dodgeGameWindow(sf::VideoMode({ 1000,900 }), "Dodge Game");

	sf::Clock gameTimer;
	sf::Clock difficultyTimer;
	sf::Clock delta;
	sf::Clock astSpawnTimer;
	sf::Clock targetedSpawnTimer;
	//sf::Clock damagingCloudSpawnTimer;
	//sf::Clock cloudDamageCoolDownTimer;
	sf::Clock GameOverTextTime{};
	GameOverTextTime.stop();

	Player player{};

	const sf::Font font{ "arial.ttf" };
	sf::Text hpText{ font, "HP: " };
	sf::Text scoreText{ font, "Score: " };
	sf::Text timeText{ font, "Time: " };
	int displayTime{};

	scoreText.setOrigin(scoreText.getLocalBounds().getCenter());
	scoreText.setPosition({ dodgeGameWindow.getSize().x / 2.f, 20.0f });
	hpText.setPosition({ static_cast<float>(dodgeGameWindow.getSize().x - 110), 0.0f });


	player.setPosition({ dodgeGameWindow.getSize().x / 2.f, dodgeGameWindow.getSize().y / 2.f });
	float shotCooldown{ 0.38f };

	std::vector<Bullet> bulletsOnScreen{};
	std::vector<Asteroid> asteroidsOnScreen{};
	std::vector<Asteroid> targetedAsteroidOnScreen{};

	std::vector<damagingCloud> cloudsOnScreen{};
	int amountOfCloudsAllowed{ 3 };

	sf::Vector2f directionOfBullet{};
	sf::Vector2f mousePosition{};
	sf::Vector2f playerPosition{};

	float directionOfPlayerInDeg{};

	float speed{ 500.0f };
	float bulletSpeed{ 1200.0f };
	float asteroidSpeed{ 700.f };

	float randSpawnTimerAst{ 1.0f };
	float randSpawnTimerAstTargeted{ 2.0f };
	float randSpawnTimerDagamingCloud{ 10.f };

	float iterationOfDifficutlyTimer1{ 1.0f };
	float iterationOfDifficutlyTimer2{ 1.0f };


	while (dodgeGameWindow.isOpen() && gameTimer.isRunning())
	{

		//Game over check before anything continues
		if (checkIfGameOver(gameTimer, player))
		{
			GameOverTextTime.start();
			if (GameOverTextTime.getElapsedTime().asSeconds() >= 0.5f)
			{
				gameTimer.stop();
			}
			sf::Text gameOverText{ font, "Game Over", 100 };
			gameOverText.setPosition({ 250,350 });

			dodgeGameWindow.clear();
			dodgeGameWindow.draw(gameOverText);
			dodgeGameWindow.display();
		}
		else
		{
			//Game difficutly increase

			if (difficultyTimer.getElapsedTime().asSeconds() > 10.0f * iterationOfDifficutlyTimer1)
			{
				asteroidSpeed += 15.f;
				randSpawnTimerAst -= 0.025f;
				randSpawnTimerAstTargeted -= 0.05f;
				randSpawnTimerDagamingCloud -= 0.1f;
				if (iterationOfDifficutlyTimer1 <= 20)
				{
					iterationOfDifficutlyTimer1++;
				}

			}

			if (difficultyTimer.getElapsedTime().asSeconds() > 120.0f * iterationOfDifficutlyTimer2)
			{
				amountOfCloudsAllowed += 1;
				if (iterationOfDifficutlyTimer2 <= 20)
				{
					iterationOfDifficutlyTimer2++;
				}

			}



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
					bulletsOnScreen.erase(bulletsOnScreen.begin() + index);
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
				asteriod.setPosition({ randomPositionAndDirectionOutsideOfScreen(Random::get(1,4), dodgeGameWindow, asteriod) });
				asteriod.speed = astSpeedBasedOnSize(asteroidSpeed, asteriod);
				asteroidsOnScreen.push_back(asteriod);

				astSpawnTimer.restart();
			}

			if (randSpawnTimerAstTargeted <= targetedSpawnTimer.getElapsedTime().asSeconds())
			{
				Asteroid asteriod{};
				asteriod.deleteSaveTimeC.start();
				asteriod.setPosition({ randomPositionForTargetedAst(Random::get(1,4), dodgeGameWindow, asteriod) });
				asteriod.direction = normalize(playerPosition - asteriod.getPosition());
				asteriod.speed = astSpeedBasedOnSize(asteroidSpeed, asteriod);
				targetedAsteroidOnScreen.push_back(asteriod);

				targetedSpawnTimer.restart();
			}

			//Deactivated for now, might activate if after rethinking this mechanic
			/*if (randSpawnTimerDagamingCloud <= damagingCloudSpawnTimer.getElapsedTime().asSeconds() && std::ssize(cloudsOnScreen) < amountOfCloudsAllowed)
			{
				damagingCloud dmgc{};
				dmgc.setPosition( { static_cast<float>(Random::get(0,dodgeGameWindow.getSize().x)), static_cast<float>(0,Random::get(0,dodgeGameWindow.getSize().x))} );
				dmgc.timeBeforeDamageDealt.start();
				dmgc.timeAlive.start();
				damagingCloudSpawnTimer.restart();

				cloudsOnScreen.push_back(dmgc);
			}*/

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
				{
					playAgain = false;
					return;
					dodgeGameWindow.close();
				}
					
				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->scancode == sf::Keyboard::Scan::Escape)
					{
						bool optionsOpen{ true };
						while (optionsOpen)
						{
							//Pausing all Clocks so nothing in the background continues running (asteroids flying etc..)
							gameTimer.stop();
							difficultyTimer.stop();
							delta.stop();
							astSpawnTimer.stop();
							targetedSpawnTimer.stop();


							sf::Text OptionsHeadline{ font, "OPTIONS" ,50 };
							OptionsHeadline.setPosition({400, 50});

							sf::Text upText{ font, "W:  Up/Forward", 30 };
							upText.setPosition({200, 150});

							sf::Text leftText{ font, "A:  Left", 30 };
							leftText.setPosition({ 200, 200 });

							sf::Text downText{ font, "S:  Down/Backwards", 30 };
							downText.setPosition({ 200, 250 });

							sf::Text rightText{ font, "D:  Right", 30 };
							rightText.setPosition({ 200, 300 });

							sf::Text mouseMoveText{ font, "Moving Mouse:  Aim", 30 };
							mouseMoveText.setPosition({ 200, 350 });

							sf::Text leftMouseText{ font, "Left Mouse Button:  Shoot", 30 };
							leftMouseText.setPosition({ 200, 400 });

							Button continueButton{ "continueButton" };
							std::string continueButtonText{ "Continue" };
							continueButton.setButtonText(continueButtonText);
							continueButton.setPosition({ 550.f,600.f });

							Button quitButton{ "quit" };
							std::string quitButtonText{ "Quit" };
							quitButton.setButtonText(quitButtonText);
							quitButton.setPosition({ 350.f,600.f });

							while (const std::optional event = dodgeGameWindow.pollEvent())
							{
								if (event->is<sf::Event::Closed>())
								{
									playAgain = false;
									return;
									dodgeGameWindow.close();
								}
									
								if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
								{
									if (keyPressed->scancode == sf::Keyboard::Scan::Escape)
									{
										optionsOpen = false;
									}
								}

								if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
								{
									if (continueButton.isHovered(dodgeGameWindow))
									{
										continueButton.setButtonFillColor(sf::Color::Cyan);
									}
									else
									{
										continueButton.setButtonFillColor(sf::Color::White);
									}

									if (quitButton.isHovered(dodgeGameWindow))
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
										if (continueButton.onPressed(dodgeGameWindow))
										{
											optionsOpen = false;
										}

										if (quitButton.onPressed(dodgeGameWindow))
										{
											playAgain = false;
											return;
										}
									}
								}
								dodgeGameWindow.clear();

								dodgeGameWindow.draw(OptionsHeadline);
								dodgeGameWindow.draw(upText);
								dodgeGameWindow.draw(downText);
								dodgeGameWindow.draw(rightText);
								dodgeGameWindow.draw(leftText);
								dodgeGameWindow.draw(mouseMoveText);
								dodgeGameWindow.draw(leftMouseText);

								dodgeGameWindow.draw(continueButton);
								dodgeGameWindow.draw(quitButton);

								dodgeGameWindow.display();
							}	
						}
						gameTimer.start();
						difficultyTimer.start();
						delta.start();
						astSpawnTimer.start();
						targetedSpawnTimer.start();
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
							if (bulletCollisionHandling(bullet, ast, player))
							{
								if (ast.getRadius() >= 35.f)
								{
									sf::Vector2f perp{ -ast.direction.y, ast.direction.x };

									Asteroid asteroid{};
									asteroid.speed = ast.speed;
									asteroid.direction = normalize(ast.direction + perp * 0.25f);
									asteroid.setRadius(ast.getRadius() / 2.f);
									asteroid.setPosition(ast.getPosition() + perp * (asteroid.getRadius() + 20.f));
									asteroid.deleteSaveTime = 0.0f;
									asteroidsOnScreen.push_back(asteroid);

									Asteroid asteroid2{};
									asteroid2.speed = ast.speed;
									asteroid2.direction = normalize(ast.direction - perp * 0.25f);
									asteroid2.setRadius(ast.getRadius() / 2.f);
									asteroid2.setPosition(ast.getPosition() - perp * (asteroid.getRadius() + 20.f));
									asteroid2.deleteSaveTime = 0.0f;
									asteroidsOnScreen.push_back(asteroid2);

								}
							}
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
							if (bulletCollisionHandling(bullet, ast, player))
							{
								if (ast.getRadius() >= 35.f)
								{
									sf::Vector2f perp{ -ast.direction.y, ast.direction.x };

									Asteroid asteroid{};
									asteroid.speed = ast.speed;
									asteroid.direction = normalize(ast.direction - perp * 0.25f);
									asteroid.setRadius(ast.getRadius() / 2.f);
									asteroid.setPosition(ast.getPosition() + perp * (asteroid.getRadius() + 20.f));
									asteroid.deleteSaveTime = 0.0f;
									asteroidsOnScreen.push_back(asteroid);

									Asteroid asteroid2{};
									asteroid2.speed = ast.speed;
									asteroid2.direction = normalize(ast.direction + perp * 0.25f);
									asteroid2.setRadius(ast.getRadius() / 2.f);
									asteroid2.setPosition(ast.getPosition() - perp * (asteroid.getRadius() + 20.f));
									asteroid2.deleteSaveTime = 0.0f;
									asteroidsOnScreen.push_back(asteroid2);

								}
							}
						}
					}
				}
			}

			/*if (std::ssize(cloudsOnScreen) > 0)
			{
				for (auto& dmgc : cloudsOnScreen)
				{
					if (playerInCloud(dmgc, player))
					{
						if (cloudDamageCoolDownTimer.getElapsedTime().asSeconds() > 0.25f)
						{
							player.setHealthPoints(player.getHealthPoints() -= 2);
							cloudDamageCoolDownTimer.restart();
						}
					}

				}
			}*/

			if (std::ssize(asteroidsOnScreen) >= 2)
			{
				for (int i1{ 0 }; std::ssize(asteroidsOnScreen) > i1 + 1; i1++)
				{
					if (asteroidsOnScreen.at(i1).isActive && asteroidsOnScreen.at(i1 + 1).isActive)
					{
						astOnAstCollision(asteroidsOnScreen.at(i1), asteroidsOnScreen.at(i1 + 1));
					}
				}
			}

			if (std::ssize(targetedAsteroidOnScreen) >= 2)
			{
				for (int i1{ 0 }; std::ssize(targetedAsteroidOnScreen) > i1 + 1; i1++)
				{
					if (targetedAsteroidOnScreen.at(i1).isActive && targetedAsteroidOnScreen.at(i1 + 1).isActive)
					{
						astOnAstCollision(targetedAsteroidOnScreen.at(i1), targetedAsteroidOnScreen.at(i1 + 1));
					}
				}
			}

			if (std::ssize(asteroidsOnScreen) >= 1 && std::ssize(targetedAsteroidOnScreen) >= 1)
			{
				for (auto& ast1 : asteroidsOnScreen)
				{
					for (auto& ast2 : targetedAsteroidOnScreen)
					{
						astOnAstCollision(ast1, ast2);
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
					bullet.move({ bullet.direction * bulletSpeed * deltaT.asSeconds() });
					dodgeGameWindow.draw(bullet);
				}
			}

			for (auto& ast : asteroidsOnScreen)
			{
				//float adjustedSpeed{ astSpeedBasedOnSize(asteroidSpeed, ast) };
				if (ast.deleteSaveTimeC.getElapsedTime().asSeconds() >= ast.deleteSaveTime)
				{
					ast.checkIfStillOnScreen(dodgeGameWindow);
				}
				if (ast.isActive)
				{
					ast.move({ ast.direction * ast.speed * deltaT.asSeconds() });
					if (playerCollision(ast, player))
					{
						player.setHealthPoints(player.getHealthPoints() -= 10);
					}
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
					ast.move({ ast.direction * ast.speed * deltaT.asSeconds() });
					if (playerCollision(ast, player))
					{
						player.setHealthPoints(player.getHealthPoints() -= 10);

					}
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
	//End of Game, now the highscore will be shown in a new Window
	dodgeGameWindow.setVisible(false);
	sf::RenderWindow highscoreWindow(sf::VideoMode({ 500,500 }), "Dodge Game Highscore");

	int highscore{player.getScore() + static_cast<int>((gameTimer.getElapsedTime().asSeconds() * 1.5))};

	sf::Text highscoreNumber{ font, "Highscore: " + std::to_string(highscore), 30};
	highscoreNumber.setPosition({ 130.f, 200.f });

	Button playAgainButton{ "playAgain" };
	std::string continueButtonText{ "Play Again" };
	playAgainButton.setButtonText(continueButtonText);
	playAgainButton.setPosition({350.f,400.f});

	Button quitButton{ "quit" };
	std::string quitButtonText{ "Quit" };
	quitButton.setButtonText(quitButtonText);
	quitButton.setPosition({ 50.f,400.f });

	while (highscoreWindow.isOpen())
	{
		while (const std::optional event = highscoreWindow.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				playAgain = false;
				highscoreWindow.close();
			}
				

			if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
			{
				if (playAgainButton.isHovered(highscoreWindow))
				{
					playAgainButton.setButtonFillColor(sf::Color::Cyan);
				}
				else
				{
					playAgainButton.setButtonFillColor(sf::Color::White);
				}

				if (quitButton.isHovered(highscoreWindow))
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
					if (playAgainButton.onPressed(highscoreWindow))
					{
						playAgain = true;
						return;
					}

					if (quitButton.onPressed(highscoreWindow))
					{
						 playAgain = false;
						 return;
					}
				}
			}
		}

		highscoreWindow.clear();

		highscoreWindow.draw(highscoreNumber);
		highscoreWindow.draw(playAgainButton);
		highscoreWindow.draw(quitButton);

		highscoreWindow.display();
	}
}