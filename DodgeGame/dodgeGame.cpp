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

	int animationSpriteAmount{ 10 };
	int currentAnimationSprite{ 1 };
	int animationSpriteSize{ 32 };
	float animationSpeed{ 0.05f };
	sf::Clock bulletAnimationPlayTimer{};

	sf::Sprite bulletSprite;

	Bullet(sf::Texture& texture) : bulletSprite{texture}
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

	void updateSpriteSize()
	{
		float diameter{ m_bullet.getRadius() * 2.f };
		//The 32 here is the size of the image in pixel
		float newScale{ (diameter * 2.f) / 32 };
		bulletSprite.setScale({ newScale, newScale });
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		//target.draw(m_bullet, states);
		target.draw(bulletSprite, states);

	}

	void initialTransform()
	{
		m_bullet.setOrigin(m_bullet.getGeometricCenter());
		bulletSprite.setTextureRect(sf::IntRect({ 0,0 }, { animationSpriteSize ,animationSpriteSize }));

		bulletSprite.setOrigin(bulletSprite.getLocalBounds().getCenter());
		bulletSprite.setPosition(m_bullet.getGlobalBounds().getCenter());

		updateSpriteSize();
	}

	

	sf::CircleShape m_bullet{ 5.f };
};

//---------------------------------------------------------------------------------------------------------

class Player : public sf::Drawable, public sf::Transformable
{
public:
	sf::Clock shootingCoowldown;

	int animationSpriteAmount{ 4 };
	int currentAnimationSprite{ 1 };
	int animationSpriteSize{ 48 };
	float animationSpeed{ 0.1f };
	sf::Texture m_EngineFlames{ "./EngineFire.png" };
	sf::Sprite m_EngineFlamesSprite{ m_EngineFlames };
	

	Player()
	{
		initialTransform();
	}

	Bullet shoot(sf::Texture& bulletTexture)
	{
		Bullet bullet{bulletTexture};
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

	void statusOfShip()
	{
		if (m_healthPoints <= 80 && m_healthPoints >= 60)
		{
			m_playerSprite.setTexture(m_playerTextureSD);
		}

		if (m_healthPoints <= 50 && m_healthPoints >= 30)
		{
			m_playerSprite.setTexture(m_playerTextureD);
		}

		if (m_healthPoints <= 20 && m_healthPoints >= 10)
		{
			m_playerSprite.setTexture(m_playerTextureVD);
		}
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		//target.draw(m_playerModel, states);
		target.draw(m_gunSprite, states);
		target.draw(m_EngineFlamesSprite, states);
		target.draw(m_engineEffectsSprite, states);
		target.draw(m_playerSprite, states); 
		
		
	}

	void initialTransform()
	{
		m_playerModel.setOrigin(m_playerModel.getGeometricCenter());

		m_gunSprite.setTextureRect(sf::IntRect({ 0,0 }, { 48,48 }));
		m_EngineFlamesSprite.setTextureRect(sf::IntRect({ 0,0 }, { 48,48 }));

		m_playerSprite.setScale({ 2.f,2.f });
		m_playerSprite.setPosition({-48.f,-50.f});

		m_engineEffectsSprite.setScale({ 2.f,2.f });
		m_engineEffectsSprite.setPosition({ -48.f,-50.f });

		m_gunSprite.setPosition({ -24.f,-40.f });

		m_EngineFlamesSprite.setScale({ 2.f,2.f });
		m_EngineFlamesSprite.setPosition({ -48.f,-53.f });

	}

	sf::Texture m_playerTexture{ "./FullHealth.png" };
	sf::Texture m_playerTextureSD{ "./Slightdamage.png" };
	sf::Texture m_playerTextureD{ "./Damaged.png" };
	sf::Texture m_playerTextureVD{ "./Verydamaged.png" };
	sf::Sprite m_playerSprite{m_playerTexture};

	sf::Texture m_engineEffectsTexture{"./BaseEngine.png"};
	sf::Sprite m_engineEffectsSprite{m_engineEffectsTexture};

	sf::Texture m_gunTexture{ "./Cannon.png"};
	sf::Sprite m_gunSprite{ m_gunTexture };

	
	

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
	bool animationPlayed{ false };
	sf::Vector2f direction{};
	sf::Clock deleteSaveTimeC;
	float deleteSaveTime{ 2.0f };
	float ignoringCollisionTime{ 0.1f };
	float speed{};

	int animationSpriteAmount{ 7 };
	int currentAnimationSprite{ 1 };
	int animationSpriteSize{ 96 };
	float animationSpeed{ 0.05f };
	sf::Clock asteroidAnimationPlayTimer{};

	sf::Sprite m_asteroidSprite;

	Asteroid(sf::Texture& texture) : m_asteroidSprite{texture}
	{
		initialTransform();
		asteroidAnimationPlayTimer.stop();
	}

	void checkIfStillOnScreen(sf::RenderWindow& currentWindow)
	{
		if (isActive)
		{
			if (Asteroid::getPosition().x > currentWindow.getSize().x || Asteroid::getPosition().y > currentWindow.getSize().y || Asteroid::getPosition().x < 0 || Asteroid::getPosition().y < 0)
			{
				isActive = false;
				animationPlayed = true;
			}
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

	void updateSpriteSize()
	{
		float diameter{ m_asteriod.getRadius() * 2.f };
		//The 96 here is the size of the image in pixel
		float newScale{ ((diameter * 3.f) + 10.f) / 96 };
		m_asteroidSprite.setScale({ newScale, newScale });

		float randomAngleInDeg{ static_cast<float>(Random::get(0,359)) };
		m_asteroidSprite.setRotation(sf::degrees(randomAngleInDeg));
	}

	void setNewFrame(sf::IntRect& newRect)
	{
		m_asteroidSprite.setTextureRect(newRect);
	}

	void setNewTexture(sf::Texture& newTexture)
	{
		m_asteroidSprite.setTexture(newTexture);
	}

	sf::Sprite& getSprite()
	{
		return m_asteroidSprite;
	}

private:

	void initialTransform()
	{
		m_asteriod.setOrigin(m_asteriod.getGeometricCenter());

		m_asteroidSprite.setOrigin(m_asteroidSprite.getLocalBounds().getCenter());
		m_asteroidSprite.setPosition({ m_asteriod.getPosition().x ,  m_asteriod.getPosition().y});

		updateSpriteSize();

		float randomAngleInDeg{ static_cast<float>(Random::get(0,359)) };
		m_asteroidSprite.setRotation(sf::degrees(randomAngleInDeg));
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		//target.draw(m_asteriod, states);
		target.draw(m_asteroidSprite, states);
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
	if ((dx * dx) + (dy * dy) < dr * dr && asteroid.isActive && bullet.isActive)
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
	if ((dx * dx) + (dy * dy) < dr * dr && ast1.isActive && ast2.isActive)
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
	if ((dx * dx) + (dy * dy) < dr * dr && asteroid.isActive)
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
	return speed;
}

static void playAnimation(sf::Clock& animationTimer, float& animationSpeed, sf::Sprite& sprite, int& animationSpriteSize, int& currentAnimationSprite, int& animationSpriteAmount)
{
	if (animationTimer.getElapsedTime().asSeconds() >= animationSpeed)
	{

		sprite.setTextureRect(sf::IntRect({ 0 + (animationSpriteSize * currentAnimationSprite), 0 }, { animationSpriteSize ,animationSpriteSize }));
		currentAnimationSprite++;
		if (currentAnimationSprite >= animationSpriteAmount)
		{
			currentAnimationSprite = 1;
		}

		animationTimer.restart();
	}
}

static void playAnimationAst(sf::Clock& animationTimer, float& animationSpeed, sf::Sprite& sprite, int& animationSpriteSize, int& currentAnimationSprite, int& animationSpriteAmount, Asteroid& ast)
{
	if (animationTimer.getElapsedTime().asSeconds() >= animationSpeed)
	{
		sprite.setTextureRect(sf::IntRect({ 0 + (animationSpriteSize * currentAnimationSprite), 0 }, { animationSpriteSize ,animationSpriteSize }));
		currentAnimationSprite++;
		if (currentAnimationSprite > animationSpriteAmount)
		{
			
			ast.animationPlayed = true;
		}

		animationTimer.restart();
	}
}

//-------------------------------------------------------------------------

void dodgeGame(bool& playAgain)
{
	sf::RenderWindow dodgeGameWindow(sf::VideoMode({ 1000,900 }), "Dodge Game");

	sf::Texture background{ "./Starfield.png" };
	sf::Sprite backgroundSprite{ background };

	int animationSpriteAmount_BH{ 60 };
	int currentAnimationSprite_BH{ 1 };
	int animationSpriteSize_BH{ 250 };
	sf::Clock animationTimer_BH{};
	float animationSpeed_BH{ 0.03f };
	sf::Texture blackHole{ "BlackHole.png" };
	sf::Sprite blackholeSprite{ blackHole };
	blackholeSprite.setTextureRect(sf::IntRect({ 0,0 }, { animationSpriteSize_BH,animationSpriteSize_BH }));
	blackholeSprite.setScale({ 0.35f,0.35f });
	blackholeSprite.setPosition({136.f, 90.f});
	blackholeSprite.setColor(sf::Color(230, 230, 230));

	int animationSpriteAmount_G1{ 60 };
	int currentAnimationSprite_G1{ 1 };
	int animationSpriteSize_G1{ 200 };
	sf::Clock animationTimer_G1{};
	float animationSpeed_G1{ 0.05f };
	sf::Texture galaxy1Texture{"./Galaxy1.png"};
	sf::Sprite galaxy1Sprite{ galaxy1Texture };
	galaxy1Sprite.setTextureRect(sf::IntRect({ 0,0 }, { 200,200 }));
	galaxy1Sprite.setScale({ 0.5f,0.5f });
	galaxy1Sprite.setPosition({799.f,689.f});
	galaxy1Sprite.setColor(sf::Color(200, 200, 200));
	

	int animationSpriteAmount_G2{ 60 };
	int currentAnimationSprite_G2{ 1 };
	int animationSpriteSize_G2{ 200 };
	sf::Clock animationTimer_G2{};
	float animationSpeed_G2{ 0.05f };
	sf::Texture galaxy2Texture{ "./Galaxy2.png" };
	sf::Sprite galaxy2Sprite{ galaxy2Texture };
	galaxy2Sprite.setTextureRect(sf::IntRect({ 0,0 }, { 200,200 }));
	galaxy2Sprite.setScale({ 0.3f,0.3f });
	galaxy2Sprite.setPosition({ 159.f,629.f });

	int animationSpriteAmount_WP{ 60 };
	int currentAnimationSprite_WP{ 1 };
	int animationSpriteSize_WP{ 200 };
	sf::Clock animationTimer_WP{};
	float animationSpeed_WP{ 0.2f };
	sf::Texture wetPlanetTexture{ "./WetPlanet.png" };
	sf::Sprite wetPlanetSprite{ wetPlanetTexture };
	wetPlanetSprite.setTextureRect(sf::IntRect({ 0,0 }, { 200,200 }));
	wetPlanetSprite.setScale({ 0.8f,0.8f });
	wetPlanetSprite.setPosition({ 420.f,500.f });
	wetPlanetSprite.setColor(sf::Color(160, 160, 160));

	int animationSpriteAmount_LP{ 60 };
	int currentAnimationSprite_LP{ 1 };
	int animationSpriteSize_LP{ 200 };
	sf::Clock animationTimer_LP{};
	float animationSpeed_LP{ 0.2f };
	sf::Texture lavaPlanetTexture{ "./LavaPlanet.png" };
	sf::Sprite lavaPlanetSprite{ lavaPlanetTexture };
	lavaPlanetSprite.setTextureRect(sf::IntRect({ 0,0 }, { 200,200 }));
	lavaPlanetSprite.setScale({ 0.5f,0.5f });
	lavaPlanetSprite.setPosition({ 320.f,300.f });
	lavaPlanetSprite.setColor(sf::Color(180, 180, 180));

	int animationSpriteAmount_GG{ 60 };
	int currentAnimationSprite_GG{ 1 };
	int animationSpriteSize_GG{ 270 };
	sf::Clock animationTimer_GG{};
	float animationSpeed_GG{ 0.2f };
	sf::Texture gasGiantTexture{ "./GasGiant.png" };
	sf::Sprite gasGiantSprite{ gasGiantTexture };
	gasGiantSprite.setTextureRect(sf::IntRect({ 0,0 }, { 200,200 }));
	gasGiantSprite.setScale({ 0.45f,0.45f });
	gasGiantSprite.setPosition({ 620.f,190.f });

	int animationSpriteAmount_NS{ 60 };
	int currentAnimationSprite_NS{ 1 };
	int animationSpriteSize_NS{ 200 };
	sf::Clock animationTimer_NS{};
	float animationSpeed_NS{ 0.01f };
	sf::Texture neutronStarTexture{ "./NeutronStar.png" };
	sf::Sprite neutronStarSprite{ neutronStarTexture };
	neutronStarSprite.setTextureRect(sf::IntRect({ 0,0 }, { 200,200 }));
	neutronStarSprite.setScale({ 0.2f,0.2f });
	neutronStarSprite.setPosition({ 520.f,360.f });

	//Textures for objects that will be destroyed during the loop of the Game
	sf::Texture m_asteroidTexture{ "./Asteroid.png" };
	sf::Texture m_asteroidtextureHit{ "./AsteroidExplode.png", false, sf::IntRect({96,0}, {96,96}) };
	sf::Texture asteroidExplodeTexture{ "./AsteroidExplode.png" };

	sf::Texture bulletTexture{ "./BulletBall.png" };

	sf::Clock gameTimer;
	sf::Clock difficultyTimer;
	sf::Clock delta;
	sf::Clock astSpawnTimer;
	sf::Clock targetedSpawnTimer;
	//sf::Clock damagingCloudSpawnTimer;
	//sf::Clock cloudDamageCoolDownTimer;
	sf::Clock GameOverTextTime{};
	GameOverTextTime.stop();
	sf::Clock playerAnimationTimer{};
	

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
			player.statusOfShip();
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
				if (!asteroidsOnScreen.at(index).isActive && asteroidsOnScreen.at(index).animationPlayed)
				{
					asteroidsOnScreen.erase(asteroidsOnScreen.begin() + index);
				}
			}

			for (int index{ 0 }; index < std::ssize(targetedAsteroidOnScreen); index++)
			{
				if (!targetedAsteroidOnScreen.at(index).isActive && targetedAsteroidOnScreen.at(index).animationPlayed)
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
				Asteroid asteriod{m_asteroidTexture};
				asteriod.deleteSaveTimeC.start();
				asteriod.setPosition({ randomPositionAndDirectionOutsideOfScreen(Random::get(1,4), dodgeGameWindow, asteriod) });
				asteriod.speed = astSpeedBasedOnSize(asteroidSpeed, asteriod);
				asteroidsOnScreen.push_back(asteriod);

				astSpawnTimer.restart();
			}

			if (randSpawnTimerAstTargeted <= targetedSpawnTimer.getElapsedTime().asSeconds())
			{
				Asteroid asteriod{ m_asteroidTexture };
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
					Bullet bullet{ player.shoot(bulletTexture) };
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
								if (ast.getRadius() >= 32.f)
								{
									ast.animationPlayed = true;

									sf::Vector2f perp{ -ast.direction.y, ast.direction.x };

									Asteroid asteroid{ m_asteroidTexture };
									asteroid.speed = ast.speed;
									asteroid.direction = normalize(ast.direction + perp * 0.25f);
									asteroid.setRadius(ast.getRadius() / 2.f);
									asteroid.setPosition(ast.getPosition() + perp * (asteroid.getRadius() + 20.f));
									asteroid.deleteSaveTime = 0.0f;
									asteroid.setNewTexture(m_asteroidtextureHit);
									asteroid.updateSpriteSize();
									asteroidsOnScreen.push_back(asteroid);

									Asteroid asteroid2{ m_asteroidTexture };
									asteroid2.speed = ast.speed;
									asteroid2.direction = normalize(ast.direction - perp * 0.25f);
									asteroid2.setRadius(ast.getRadius() / 2.f);
									asteroid2.setPosition(ast.getPosition() - perp * (asteroid.getRadius() + 20.f));
									asteroid2.deleteSaveTime = 0.0f;
									asteroid2.setNewTexture(m_asteroidtextureHit);
									asteroid2.updateSpriteSize();
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
								if (ast.getRadius() >= 32.f)
								{
									ast.animationPlayed = true;

									sf::Vector2f perp{ -ast.direction.y, ast.direction.x };

									Asteroid asteroid{ m_asteroidTexture };
									asteroid.speed = ast.speed;
									asteroid.direction = normalize(ast.direction + perp * 0.25f);
									asteroid.setRadius(ast.getRadius() / 2.f);
									asteroid.setPosition(ast.getPosition() + perp * (asteroid.getRadius() + 20.f));
									asteroid.deleteSaveTime = 0.0f;
									asteroid.setNewTexture(m_asteroidtextureHit);
									asteroid.updateSpriteSize();
									asteroidsOnScreen.push_back(asteroid);

									Asteroid asteroid2{ m_asteroidTexture };
									asteroid2.speed = ast.speed;
									asteroid2.direction = normalize(ast.direction - perp * 0.25f);
									asteroid2.setRadius(ast.getRadius() / 2.f);
									asteroid2.setPosition(ast.getPosition() - perp * (asteroid.getRadius() + 20.f));
									asteroid2.deleteSaveTime = 0.0f;
									asteroid2.setNewTexture(m_asteroidtextureHit);
									asteroid2.updateSpriteSize();
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
				for (auto i1{ 0 }; std::ssize(asteroidsOnScreen) > i1 + 1; i1++)
				{
					if (asteroidsOnScreen.at(i1).isActive && asteroidsOnScreen.at(i1 + 1).isActive)
					{
						astOnAstCollision(asteroidsOnScreen.at(i1), asteroidsOnScreen.at(i1 + 1));
					}
				}
			}

			if (std::ssize(targetedAsteroidOnScreen) >= 2)
			{
				for (auto i1{ 0 }; std::ssize(targetedAsteroidOnScreen) > i1 + 1; i1++)
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
			dodgeGameWindow.draw(backgroundSprite);

			playAnimation(animationTimer_BH, animationSpeed_BH, blackholeSprite, animationSpriteSize_BH, currentAnimationSprite_BH, animationSpriteAmount_BH);
			dodgeGameWindow.draw(blackholeSprite);

			playAnimation(animationTimer_G1, animationSpeed_G1, galaxy1Sprite, animationSpriteSize_G1, currentAnimationSprite_G1, animationSpriteAmount_G1);
			dodgeGameWindow.draw(galaxy1Sprite);
			
			playAnimation(animationTimer_G2, animationSpeed_G2, galaxy2Sprite, animationSpriteSize_G2, currentAnimationSprite_G2, animationSpriteAmount_G2);
			dodgeGameWindow.draw(galaxy2Sprite);

			playAnimation(animationTimer_WP, animationSpeed_WP, wetPlanetSprite, animationSpriteSize_WP, currentAnimationSprite_WP, animationSpriteAmount_WP);
			dodgeGameWindow.draw(wetPlanetSprite);

			playAnimation(animationTimer_LP, animationSpeed_LP, lavaPlanetSprite, animationSpriteSize_LP, currentAnimationSprite_LP, animationSpriteAmount_LP);
			dodgeGameWindow.draw(lavaPlanetSprite);

			playAnimation(animationTimer_GG, animationSpeed_GG, gasGiantSprite, animationSpriteSize_GG, currentAnimationSprite_GG, animationSpriteAmount_GG);
			dodgeGameWindow.draw(gasGiantSprite);

			playAnimation(animationTimer_NS, animationSpeed_NS, neutronStarSprite, animationSpriteSize_NS, currentAnimationSprite_NS, animationSpriteAmount_NS);
			dodgeGameWindow.draw(neutronStarSprite);

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
					playAnimation(bullet.bulletAnimationPlayTimer, bullet.animationSpeed, bullet.bulletSprite, bullet.animationSpriteSize, bullet.currentAnimationSprite ,bullet.animationSpriteAmount);
					dodgeGameWindow.draw(bullet);
				}
				
			}

			for (auto& ast : asteroidsOnScreen)
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
				}

				if(!ast.isActive && !ast.animationPlayed)
				{
					if (!ast.asteroidAnimationPlayTimer.isRunning())
					{
						ast.asteroidAnimationPlayTimer.start();
						ast.setNewTexture(asteroidExplodeTexture);
					}
					
					playAnimationAst(ast.asteroidAnimationPlayTimer, ast.animationSpeed, ast.m_asteroidSprite, ast.animationSpriteSize, ast.currentAnimationSprite, ast.animationSpriteAmount, ast);
				}
				dodgeGameWindow.draw(ast);
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
				}

				if (!ast.isActive && !ast.animationPlayed)
				{
					if (!ast.asteroidAnimationPlayTimer.isRunning())
					{
						ast.asteroidAnimationPlayTimer.start();
						ast.setNewTexture(asteroidExplodeTexture);
					}

					playAnimationAst(ast.asteroidAnimationPlayTimer, ast.animationSpeed, ast.m_asteroidSprite, ast.animationSpriteSize, ast.currentAnimationSprite, ast.animationSpriteAmount, ast);
				}
				dodgeGameWindow.draw(ast);
			}

			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)
				&& !sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
			{
				playAnimation(playerAnimationTimer, player.animationSpeed, player.m_EngineFlamesSprite, player.animationSpriteSize, player.currentAnimationSprite, player.animationSpriteAmount);
			}
			else
			{
				if (playerAnimationTimer.getElapsedTime().asSeconds() >= player.animationSpeed)
				{

					player.m_EngineFlamesSprite.setTextureRect(sf::IntRect({ 0 + (player.animationSpriteSize * player.currentAnimationSprite), player.animationSpriteSize }, { player.animationSpriteSize ,player.animationSpriteSize }));
					player.currentAnimationSprite++;
					if (player.currentAnimationSprite >= player.animationSpriteAmount)
					{
						player.currentAnimationSprite = 1;
					}

					playerAnimationTimer.restart();
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