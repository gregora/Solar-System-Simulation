#include "../include/Planet.h"

Planet::Planet(std::string name, double mass, double diameter, double x, double y, double z, double vx, double vy, double vz, sf::Color color, bool draw_history, bool is_moon)
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

    font.loadFromFile("fonts/Prototype.ttf");

}

Planet::Planet(){}

void Planet::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    

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
    text.setFont(font);
    text.setString(name);
    text.setCharacterSize(60);

    text.setPosition(x, -y);

    text.setScale(height / 3000, height / 3000);
    if(!is_moon){
        target.draw(text, states.transform*getTransform());
    }

}



void simulate(Planet planets[], uint N, float delta, bool save_history)
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