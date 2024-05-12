#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <SFML/Graphics.hpp>
#include "include/ui.h"
#include <cmath>

class Planet: public sf::Transformable, public sf::Drawable
{
    public:
        std::string name;
        bool is_moon = false;
        double mass;
        double diameter;
        double x, y, z;
        double vx, vy, vz;
        double ax, ay, az;

        sf::Color color;

        float max_history = 100;
        std::vector<double> x_history;
        std::vector<double> y_history;
        bool draw_history = true;



        Planet(std::string name, double mass, double diameter, double x, double y, double z, double vx, double vy, double vz, sf::Color color = sf::Color(0, 0, 255), bool draw_history = true, bool is_moon = false)
        {
            this->name = name;
            this->mass = mass;
            this->diameter = diameter;
            
            this->x = x;
            this->y = y;
            this->z = z;
            
            this->vx = vx;
            this->vy = vy;
            this->vz = vz;

            this->ax = 0;
            this->ay = 0;
            this->az = 0;


            this->color = color;

            this->draw_history = draw_history;
            this->is_moon = is_moon;
        }

        Planet(){
        }

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            

            if(draw_history){
                uint n = x_history.size();
                sf::VertexArray shape(sf::LineStrip, n + 1);
                
                for(uint i = 0; i < n; i++){
                    shape[i].position = sf::Vector2f(x_history[i], -y_history[i]);
                    if(i == 0 && n == max_history){
                        sf::Vector2f v1 = sf::Vector2f(x_history[1], -y_history[1]) - sf::Vector2f(x_history[0], -y_history[0]);
                        float alpha = sqrt((x - x_history[n-1])*(x - x_history[n-1]) + (y - y_history[n-1])*(y - y_history[n-1])) / sqrt(v1.x * v1.x + v1.y * v1.y);
                        shape[i].position = sf::Vector2f(x_history[0], -y_history[0]) + alpha * v1;
                    }
                    
                    shape[i].color = color;
                }
                shape[n].position = sf::Vector2f(x, -y);
                shape[n].color = color;
                target.draw(shape, states.transform*getTransform());
            }


            float r = diameter / 2;

            float apparent_size = r / target.getView().getSize().y;
            float width = target.getView().getSize().x;
            float height = target.getView().getSize().y;
            
            sf::CircleShape circle(r);
            circle.setFillColor(sf::Color::Blue);
            circle.setPosition(x, -y);
            circle.setOrigin(r, r);
            circle.setFillColor(color);

            if(apparent_size < 0.01){

                circle.setScale(0.005/ apparent_size, 0.005 / apparent_size);
            
                if(name == "Sun"){
                    circle.setScale(0.01/ apparent_size, 0.01 / apparent_size);
                }

                if(is_moon){
                    circle.setScale(0.0025/ apparent_size, 0.0025 / apparent_size);
                }
            }

            target.draw(circle, states.transform*getTransform());

            sf::Text text;
            sf::Font font;
            font.loadFromFile("fonts/Prototype.ttf");
            text.setFont(font);
            text.setString(name);
            text.setCharacterSize(60);

            text.setPosition(x, -y);

            text.setScale(height / 3000, height / 3000);
            if(!is_moon){
                target.draw(text, states.transform*getTransform());
            }

        }
};

void simulate(Planet planets[], uint N, float delta, bool save_history = false)
{
    for (uint i = 0; i < N; i++)
    {
        planets[i].ax = 0;
        planets[i].ay = 0;
        planets[i].az = 0;
    }

    for (uint i = 0; i < N; i++)
    {
        for (uint j = 0; j < N; j++)
        {
            if (i != j)
            {
                double dx = planets[j].x - planets[i].x;
                double dy = planets[j].y - planets[i].y;
                double dz = planets[j].z - planets[i].z;

                double r = 1000.0f * sqrt(dx * dx + dy * dy + dz * dz);

                double F = 6.67430e-11 * planets[i].mass * planets[j].mass / (r * r);

                planets[i].ax += F * dx / r / planets[i].mass;
                planets[i].ay += F * dy / r / planets[i].mass;
                planets[i].az += F * dz / r / planets[i].mass;



            }
        }
    }

    for (uint i = 0; i < N; i++)
    {
        planets[i].vx += planets[i].ax * delta;
        planets[i].vy += planets[i].ay * delta;
        planets[i].vz += planets[i].az * delta;

        planets[i].x += planets[i].vx * delta;
        planets[i].y += planets[i].vy * delta;
        planets[i].z += planets[i].vz * delta;


        if(save_history){
            planets[i].x_history.push_back(planets[i].x);
            planets[i].y_history.push_back(planets[i].y);

            if(planets[i].x_history.size() > planets[i].max_history){
                planets[i].x_history.erase(planets[i].x_history.begin());
                planets[i].y_history.erase(planets[i].y_history.begin());
            }
        }
        
    }
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

    FPS fps;
	fps.setScale(0.5, 0.5);
	fps.frame_average = 60;

    uint SUBCALCULATIONS = 2000;

    sf::Clock clock;

    float modifier = 3600*24;
    float delta = 0.0;

    long long int frame = 0;

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
        sf::Font font;
        font.loadFromFile("fonts/Prototype.ttf");
        modifier_text.setFont(font);
        modifier_text.setString(std::to_string((long) modifier) + "x  (" + std::to_string((long) (modifier / (3600.0 * 24))) + " days/s)");
        modifier_text.setCharacterSize(25);
        modifier_text.setPosition(20, 20);
        window.draw(modifier_text);

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
