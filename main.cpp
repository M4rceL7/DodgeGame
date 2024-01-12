#include <SFML/Window/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main() {

    sf::RenderWindow window(sf::VideoMode(1280, 960), "Main Window",sf::Style::Close);
    window.setFramerateLimit(60);

    sf::FloatRect windowBounds({0.f, 0.f}, window.getDefaultView().getSize());

    sf::CircleShape triangle(30.f,3);
    triangle.setPosition((window.getView().getSize() - triangle.getOrigin())/2.f);

    // run the program as long as the window is open
    while(window.isOpen()) {

        //Key Inputs (Arrow Keys)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            triangle.move(-6.f, 0.f);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            triangle.move(6.f,0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            triangle.move(0.f, -6.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            triangle.move(0.f, 6.f);
        }

        //Out of Boundaries check
        int WINDOW_H = window.getSize().y;
        int WINDOW_W = window.getSize().x;

        //Left check
        if(triangle.getPosition().x < 0)
            triangle.setPosition(0, triangle.getPosition().y);

        //Top check
        if(triangle.getPosition().y < 0)
            triangle.setPosition(triangle.getPosition().x, 0);

        //Right check
        if(triangle.getPosition().x + triangle.getGlobalBounds().width > WINDOW_W)
                triangle.setPosition(WINDOW_W - triangle.getGlobalBounds().width, triangle.getPosition().y);

        //Bottom check
        if(triangle.getPosition().y + triangle.getGlobalBounds().height > WINDOW_H)
                    triangle.setPosition(triangle.getPosition().x, WINDOW_H - triangle.getGlobalBounds().height);


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