import json
import pprint
import geonlp

request = \
    {
        "method": "parse",
        "params": ["国立情報学研究所は千代田区にあります。"],
        "id": 1
    }

g = geonlp.GeonlpService()

response = json.loads(g.proc(json.dumps(request)))

pp = pprint.PrettyPrinter(indent=1, compact=True)
pp.pprint(response)
