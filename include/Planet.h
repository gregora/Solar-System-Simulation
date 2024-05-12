#ifndef _PLANET_H
#define _PLANET_H

#include <SFML/Graphics.hpp>
#include <cmath>

class Planet: public sf::Transformable, public sf::Drawable
{
    public:
    std::string name;
    bool is_moon = false;
    long double mass;
    long double diameter;
    long double x, y, z;
    long double vx, vy, vz;
    long double ax, ay, az;

    sf::Color color;

    uint max_history = 200;
    std::vector<long double> x_history;
    std::vector<long double> y_history;
    bool draw_history = true;

    Planet(std::string name, long double mass, long double diameter, long double x, long double y, long double z, long double vx, long double vy, long double vz, sf::Color color = sf::Color(0, 0, 255), bool draw_history = true, bool is_moon = false);

    Planet();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
    sf::Font font;

};

void simulate(Planet planets[], uint N, float delta, bool save_history = false);


#endif