import json

# load planet_positions.json and planet_sizes.json and combine them into a single dictionary

with open("planet_positions.json", "r") as file:
    planet_positions = json.load(file)

with open("planet_sizes.json", "r") as file:
    planet_sizes = json.load(file)

# combine the two dictionaries into a single dictionary

combined_data = {}

for planet in planet_positions:
    x, y, z = planet_positions[planet]["x"], planet_positions[planet]["y"], planet_positions[planet]["z"]
    vx, vy, vz = planet_positions[planet]["vx"], planet_positions[planet]["vy"], planet_positions[planet]["vz"]
    
    diameter = planet_sizes[planet]["diameter"]
    mass = planet_sizes[planet]["mass"]

    combined_data[planet] = {
        "x": x,
        "y": y,
        "z": z,
        "vx": vx,
        "vy": vy,
        "vz": vz,
        "diameter": diameter,
        "mass": mass
    }

# save the combined data to a file called combined_data.json

with open("planets.json", "w") as file:

    json.dump(combined_data, file, indent=4)

