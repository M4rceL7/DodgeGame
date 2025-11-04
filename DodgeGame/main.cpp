#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280, 960 }), "DodgeGame",sf::State::Windowed);
    window.setFramerateLimit(30);

    window.setActive(true);

    bool gameLoop{ true };
    while (gameLoop) {
        // handle events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                // end the program
                gameLoop = false;
            }
        }
    }

}