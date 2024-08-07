#include <iostream>
#include <fstream>
// #include <jsoncpp/json/json.h> // Ubuntu
#include <json/json.h> // Fedora

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

    sf::Text modifier_text;
    modifier_text.setFont(font);
    modifier_text.setCharacterSize(20);
    modifier_text.setPosition(20, 60);

    sf::Text date_text;
    date_text.setFont(font);
    date_text.setCharacterSize(25);
    date_text.setPosition(20, 20);

    sf::Text tracking_text;
    tracking_text.setFont(font);

    sf::RectangleShape tracking_background(sf::Vector2f(250, 110));
    tracking_background.setPosition(10, 140);
    tracking_background.setFillColor(sf::Color(255, 255, 255, 20));


    FPS fps;
	fps.setScale(0.5, 0.5);
	fps.frame_average = 60;

    uint SUBCALCULATIONS = 2000;

    sf::Clock clock;

    float modifier = 3600*24;
    float pause_modifier = 1.0;
    float delta = 0.0;

    long long int frame = 0;
    long double time = 0.0;
    long double time0 = 1715464800.0; // 12th of May 2024

    float time_from_save = 3600*24*2;

    int tracking = -1;

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
            
            if (event.type == sf::Event::MouseButtonReleased){
                if (event.mouseButton.button == sf::Mouse::Left){
                    
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                    sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos);

                    bool found = false;
                    for(uint i = 0; i < N; i++){

                        if(planets[i].is_moon){
                            continue;
                        }

                        float px = planets[i].x;
                        float py = -planets[i].y;

                        sf::Vector2i pos = window.mapCoordsToPixel(sf::Vector2f(px, py));


                        float d = sqrt(pow(pos.x - mouse_pos.x, 2) + pow(pos.y - mouse_pos.y, 2));

                        if(d < 10){
                            tracking = i;
                            found = true;
                        }

                    }

                    if(!found){
                        tracking = -1;
                    }
                }
            }

            if(event.type == sf::Event::MouseWheelScrolled){
                if (event.mouseWheelScroll.delta > 0)
                {
                    view.zoom(1.0 - 0.3);
                }
                else if (event.mouseWheelScroll.delta < 0)
                {
                    view.zoom(1.0 + 0.3);
                }
            }

            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Space){
                    if(pause_modifier == 0.0){
                        pause_modifier = 1.0;
                    }else{
                        pause_modifier = 0.0;
                    }
                }

                if(event.key.code == sf::Keyboard::BackSpace){
                    pause_modifier *= -1.0;

                    if(pause_modifier == 0.0){
                        pause_modifier = -1.0;
                    }

                    time_from_save = 3600*24*2;

                    for(Planet& p : planets){
                        p.x_history.clear();
                        p.y_history.clear();
                    }

                }
            }
        }


        for(uint i = 0; i < SUBCALCULATIONS; i++){
            bool save = false;
            time_from_save += delta*modifier*abs(pause_modifier)/SUBCALCULATIONS;
            if(time_from_save > 3600*24*2){
                save = true;
                time_from_save = 0.0;
            }

            simulate(planets, N, delta * modifier*pause_modifier / SUBCALCULATIONS, save);
        }

        delta = clock.restart().asSeconds();
        time += delta * pause_modifier * modifier;

        window.clear();
        float rect_width = view.getSize().x;

        if(tracking != -1){
            view.setCenter(planets[tracking].x, -planets[tracking].y);
            window.setView(view);
        }

        for (uint i = 0; i < N; i++){
            window.draw(planets[i]);
        }


        window.setView(view_static);

        fps.update();
        fps.setPosition(W - 100, 20);
        window.draw(fps);

        modifier_text.setString(std::to_string((long) modifier) + "x  (" + std::to_string((long) (modifier / (3600.0 * 24))) + " days/s)");
        window.draw(modifier_text);
        
        std::string date = timestamp_to_date(time0 + time);
        date_text.setString(date);
        window.draw(date_text);

        if(tracking != -1){
            window.draw(tracking_background);

            tracking_text.setCharacterSize(20);
            tracking_text.setPosition(20, 150);

            std::string tracking_name = planets[tracking].name;
            tracking_text.setString(tracking_name);
            window.draw(tracking_text);

            tracking_text.setCharacterSize(15);
            tracking_text.setPosition(20, 180);
            std::string tracking_body = "";
            tracking_body += "Mass: " + std::to_string((int) (planets[tracking].mass / 10e20)) + " x 10^20 kg\n";
            tracking_body += "Radius: " + std::to_string((int) planets[tracking].diameter / 2) + " km\n";
            tracking_body += "Orbital velocity: " + std::to_string((int) sqrt(planets[tracking].vx*planets[tracking].vx + planets[tracking].vy*planets[tracking].vy)) + " km/s\n";
            tracking_text.setString(tracking_body);
            window.draw(tracking_text);
        }


        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            view.move(0, - delta * 0.2 *  rect_width);
            tracking = -1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            view.move(0, delta * 0.2 * rect_width);
            tracking = -1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            view.move(-delta * 0.2 * rect_width, 0);
            tracking = -1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            view.move(delta * 0.2 * rect_width, 0);
            tracking = -1;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
        {
            view.zoom(1.0 - delta*3.0);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
        {

            view.zoom(1.0 + delta*3.0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
        {
            modifier /= 1 + delta * 1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            modifier *= 1 + delta * 1;
        }

        if(pause_modifier == 0.0){
            sf::RectangleShape pause_symbol(sf::Vector2f(10, 40));
            pause_symbol.setFillColor(sf::Color::White);

            pause_symbol.setPosition(W - 50, 60);
            window.draw(pause_symbol);
            pause_symbol.setPosition(W - 70, 60);
            window.draw(pause_symbol);
        }

        if(pause_modifier == -1.0){
            //draw two triangles
            sf::ConvexShape triangle(3);
            triangle.setPoint(0, sf::Vector2f(W - 20, 60));
            triangle.setPoint(1, sf::Vector2f(W - 20, 100));
            triangle.setPoint(2, sf::Vector2f(W - 60, 80));

            triangle.setFillColor(sf::Color::White);
            window.draw(triangle);

            triangle.setPoint(0, sf::Vector2f(W - 50, 60));
            triangle.setPoint(1, sf::Vector2f(W - 50, 100));
            triangle.setPoint(2, sf::Vector2f(W - 90, 80));

            window.draw(triangle);
        }


        window.setView(view);

        window.display();
        frame++;
    }
}
