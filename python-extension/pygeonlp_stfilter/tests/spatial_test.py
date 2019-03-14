import json
import pygeonlp
from pygeonlp_filter import spatial
import pprint

pp = pprint.PrettyPrinter(indent=1, compact=True)

geojson = '{"type":"Polygon","coordinates":[[[139.457008,35.513569],[140.011817,35.513569],[140.011817,36.030563],[139.457008,36.030562],[139.457008,35.513569]]]}'

g = pygeonlp.Service()
request = {
    "method": "parse",
    "params": ["今日は上野の野外ステージでコンサートだった。（事務所：東京都台東区上野公園7-47）", {"show-candidate": True, "geojson": True}],
    "id": 1
}

response = json.loads(g.proc(json.dumps(request)))
pp.pprint(response)
print("---------------")
result1 = spatial.geo_contains(
    response, geojson, keep_candidates=True, copy=True)
pp.pprint(result1)
print("---------------")
result2 = spatial.geo_disjoint(
    response, 'http://geoshape.ex.nii.ac.jp/city/geojson/20180101/13/13106A1968.geojson', keep_candidates=True)
pp.pprint(result2)