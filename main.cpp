#include <SFML/Window/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main() {

    sf::RenderWindow window(sf::VideoMode(1280, 960), "Main Window",sf::Style::Close);
    window.setFramerateLimit(60);

    sf::CircleShape triangle(60.f,3);
    triangle.setPosition(460,480);

    // run the program as long as the window is open
    while(window.isOpen()) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            triangle.move(-5.f, 0.f);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            triangle.move(5.f,0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            triangle.move(0.f, -5.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            triangle.move(0.f, 5.f);
        }

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event{};
        while(window.pollEvent(event)) {

            
            // "close requested" event: we close the window
            if(event.type == sf::Event::Closed) window.close();
        }

        window.clear(sf::Color::Black);

        triangle.setFillColor(sf::Color(0,0,150));
        window.draw(triangle);


        // End the current frame and display its contents on screen
        window.display();


    }

    return 0;
}