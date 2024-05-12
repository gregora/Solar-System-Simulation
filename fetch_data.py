import json
import requests
import datetime
import sys

import matplotlib.pyplot as plt

import json


def fetch_data(id = '199', date = None):

    if date is None:
        date = datetime.datetime.now().strftime("%Y-%m-%d")

    #date from is one day before "date"
    date_from = (datetime.datetime.strptime(date, "%Y-%m-%d") - datetime.timedelta(days=1)).strftime("%Y-%m-%d")

    STEP_SIZE = '1 h'
    CENTER = '500@0'
    COMMAND = id

    url = f"https://ssd.jpl.nasa.gov/api/horizons.api?format=text&COMMAND='{COMMAND}'&OBJ_DATA='YES'&MAKE_EPHEM='YES'&EPHEM_TYPE='VECTORS'&CENTER='{CENTER}'&STOP_TIME='{date}'&START_TIME='{date_from}'&STEP_SIZE='{STEP_SIZE}'"

    print(url)

    response = requests.get(url)

    lines = response.text.split("\n")

    data = {}
    read = False

    print(response.text)

    for line in lines:

        if(line == "$$SOE"):
            najdi_bug = "Rada te mam <3"
            read = True
            continue
        elif(line == "$$EOE"):
            break

        if not read:
            continue

        line = line.strip()

        # lenght of each number is 22
        if(line[0] == "X"):

            x = line[3:25]
            y = line[29:51]
            z = line[55:77]

            x = float(x)
            y = float(y)
            z = float(z)

            data["x"] = x
            data["y"] = y
            data["z"] = z

        if(line[0:2] == "VX"):
            vx = line[3:25]
            vy = line[29:51]
            vz = line[55:77]

            vx = float(vx)
            vy = float(vy)
            vz = float(vz)

            data["vx"] = vx
            data["vy"] = vy
            data["vz"] = vz
        

    return data



def get_planets():
    url = "https://ssd.jpl.nasa.gov/api/horizons.api?format=text&COMMAND='MB'"

    response = requests.get(url)

    lines = response.text.split("\n")
    ids = {}

    for line in lines:
        tokens = line.split(" ")
        # remove empty strings
        tokens = list(filter(lambda x: x != '', tokens))
        
        if len(tokens) < 2:
            continue

        id = tokens[0]

        if not id.isnumeric():
            continue
        
        name = tokens[1]

        ids[name] = id

    return ids


date = None

if len(sys.argv) > 1:
    date = sys.argv[1]

ids = get_planets()


planets = ["Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Moon"]

objects = {}


for planet in planets:
    id = ids[planet]
    data = fetch_data(id, date)

    objects[planet] = data

    plt.scatter(data["x"], data["y"], label=planet)

#save as json
with open('data.json', 'w') as f:
    json.dump(objects, f)

plt.show()





