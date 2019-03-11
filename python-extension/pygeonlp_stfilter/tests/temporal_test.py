import pygeonlp
from pygeonlp_filter import temporal
import json
import pprint


pp = pprint.PrettyPrinter(indent=1, compact=True)
g = pygeonlp.Service()

request = {
    "method": "parse",
    "params": ["今日は上野の野外ステージでコンサートだった。（事務所：東京都台東区上野公園7-47）", {"show-candidate": True, "geojson": True}],
    "id": 1
}

response = json.loads(g.proc(json.dumps(request)))
filtered = temporal.time_exists(response, '2019-01-01')
pp.pprint(filtered)
