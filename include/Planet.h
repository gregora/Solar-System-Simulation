#ifndef _PLANET_H
#define _PLANET_H

#include <SFML/Graphics.hpp>
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

    float max_history = 200;
    std::vector<double> x_history;
    std::vector<double> y_history;
    bool draw_history = true;

    Planet(std::string name, double mass, double diameter, double x, double y, double z, double vx, double vy, double vz, sf::Color color = sf::Color(0, 0, 255), bool draw_history = true, bool is_moon = false);

    Planet();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
    sf::Font font;

};

void simulate(Planet planets[], uint N, float delta, bool save_history = false);


#endif