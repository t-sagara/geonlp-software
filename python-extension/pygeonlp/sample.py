import json
import pprint
import pygeonlp

# Module function

print(pygeonlp.version())


# Service

request = \
    {
        "method": "parse",
        "params": ["今日は上野の野外ステージでコンサートだった。（事務所：東京都台東区上野公園7-47）", {"show-candidate": True, "geojson": True}],
        "id": 1
    }

g = pygeonlp.Service()

response = json.loads(g.proc(json.dumps(request)))

pp = pprint.PrettyPrinter(indent=1, compact=True)
pp.pprint(response)

# MA

ma = pygeonlp.MA()

print(ma.parse("今日は上野の野外ステージでコンサートだった。"))
