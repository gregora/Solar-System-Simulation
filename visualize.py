import json

import matplotlib.pyplot as plt

with open("planets.json", "r") as file:
    planets = json.load(file)

for planet in planets:

    data = planets[planet]

    plt.scatter(data["x"], data["y"], label=planet, s=data["diameter"] / 1000)

plt.show()