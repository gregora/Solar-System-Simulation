#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <SFML/Graphics.hpp>
#include "include/ui.h"
#include "include/Planet.h"
#include <cmath>
#include <ctime>


std::string timestamp_to_date(long double timestamp) {
    // Convert the Unix timestamp to time_t
    time_t ts = static_cast<time_t>(timestamp);

    // Convert time_t to a struct tm
    struct tm* timeinfo = std::gmtime(&ts);

    // Format the date as yyyy-mm-dd
    char buffer[11]; // Buffer to hold formatted date (including null terminator)
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);

    return std::string(buffer);
}


int main() 
{
    std::ifstream ifs("planets.json");
    Json::Reader reader;
    Json::Value planets_json;

    reader.parse(ifs, planets_json);

    uint N = planets_json.getMemberNames().size();

    Planet planets[N];

    uint i = 0;
    for(auto planet : planets_json.getMemberNames())
    {
        sf::Color c;

        bool is_moon = false;
        bool draw_history = true;

        if(planet == "Sun"){
            c = sf::Color::Yellow;
        }else if(planet == "Mercury"){
            c = sf::Color::White;
        }else if(planet == "Venus"){
            c = sf::Color(255, 165, 0);
        }else if(planet == "Earth"){
            c = sf::Color::Green;
        }else if(planet == "Mars"){
            c = sf::Color::Red;
        }else if(planet == "Jupiter"){
            c = sf::Color::Magenta;
        }else if(planet == "Saturn"){
            c = sf::Color::Yellow;
        }else if(planet == "Uranus"){
            c = sf::Color::Cyan;
        }else if(planet == "Neptune"){
            c = sf::Color::Blue;
        }else if(planet == "Moon"){
            c = sf::Color::White;
            is_moon = true;
            draw_history = false;
        }

        planets[i] = Planet(planet, planets_json[planet]["mass"].asDouble(), planets_json[planet]["diameter"].asDouble(), planets_json[planet]["x"].asDouble(), planets_json[planet]["y"].asDouble(), planets_json[planet]["z"].asDouble(), planets_json[planet]["vx"].asDouble(), planets_json[planet]["vy"].asDouble(), planets_json[planet]["vz"].asDouble(), c, draw_history, is_moon);
        i++;
    }


    uint W = 800;
    uint H = 600;
    sf::RenderWindow window(sf::VideoMode(W, H), "Planets");
    sf::View view_static(sf::FloatRect(0, 0, W, H));
    sf::View view(sf::FloatRect(0, 0, W, H));
    view.setCenter(0, 0);
    float zoom = 1000000.0;
    view.zoom(zoom);
    window.setView(view);

    sf::Font font;
    font.loadFromFile("fonts/Prototype.ttf");


    FPS fps;
	fps.setScale(0.5, 0.5);
	fps.frame_average = 60;

    uint SUBCALCULATIONS = 2000;

    sf::Clock clock;

    float modifier = 3600*24;
    float delta = 0.0;

    long long int frame = 0;
    long double time = 0.0;
    long double time0 = 1715464800.0; // 12th of May 2024

    float time_from_save = 3600*24*2;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }

            if (event.type == sf::Event::Resized){
                
                float H_ratio = ((float) event.size.height) / (float) H;
                float W_ratio = ((float) event.size.width) / (float) W;

                view.setSize(
                    view.getSize().x * W_ratio,
                    view.getSize().y * H_ratio
                );
                view.setCenter(
                    view.getCenter().x * W_ratio,
                    view.getCenter().y * H_ratio
                );

                
                H = event.size.height;
                W = event.size.width;
                sf::FloatRect visibleArea(0, 0, W, H);
                view_static.setSize(visibleArea.width, visibleArea.height);
                view_static.setCenter(visibleArea.width / 2, visibleArea.height / 2);
            

            }


        }

        for(uint i = 0; i < SUBCALCULATIONS; i++){
            bool save = false;
            time_from_save += delta*modifier/SUBCALCULATIONS;
            if(time_from_save > 3600*24*2){
                save = true;
                time_from_save = 0.0;
            }

            simulate(planets, N, delta * modifier / SUBCALCULATIONS, save);
        }

        delta = clock.restart().asSeconds();
        time += delta * modifier;

        window.clear();
        float rect_width = view.getSize().x;

        for (uint i = 0; i < N; i++)
        {
            window.draw(planets[i]);
        }

        window.setView(view_static);

        fps.update();
        fps.setPosition(W - 100, 20);
        window.draw(fps);

        sf::Text modifier_text;
        modifier_text.setFont(font);
        modifier_text.setString(std::to_string((long) modifier) + "x  (" + std::to_string((long) (modifier / (3600.0 * 24))) + " days/s)");
        modifier_text.setCharacterSize(20);
        modifier_text.setPosition(20, 60);
        window.draw(modifier_text);

        sf::Text date_text;
        date_text.setFont(font);
        std::string date = timestamp_to_date(time0 + time);
        date_text.setString(date);
        date_text.setCharacterSize(25);
        date_text.setPosition(20, 20);
        window.draw(date_text);

        window.setView(view);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            view.move(0, - delta * 0.2 *  rect_width);
            window.setView(view);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            view.move(0, delta * 0.2 * rect_width);
            window.setView(view);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            view.move(-delta * 0.2 * rect_width, 0);
            window.setView(view);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            view.move(delta * 0.2 * rect_width, 0);
            window.setView(view);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
        {
            view.zoom(1.0 - delta*3.0);
            window.setView(view);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
        {

            view.zoom(1.0 + delta*3.0);
            window.setView(view);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
        {
            modifier /= 1 + delta * 1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            modifier *= 1 + delta * 1;
        }
        

        window.display();
        frame++;
    }
}
