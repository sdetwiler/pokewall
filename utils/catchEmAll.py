#!/usr/bin/env python

import requests

for i in range(801):
    url = "http://assets.pokemon.com/assets/cms2/img/pokedex/full/{:03}.png".format(i)
    print url
    r = requests.get(url)
    filename = "{:03}.png".format(i)
    with open(filename, "w") as f:
        f.write(r.content)
        print("Saved {}".format(filename))