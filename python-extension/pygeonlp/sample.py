import json
import pprint
import pygeonlp

pp = pprint.PrettyPrinter(indent=1, compact=True)

# Module function
# print(pygeonlp.version())

# Service
g = pygeonlp.Service()

# Service.proc(json)
request = \
    {
        "method": "parse",
        "params": ["今日は上野の野外ステージでコンサートだった。（事務所：東京都台東区上野公園7-47）", {"show-candidate": False, "geojson": True}],
        "id": 1
    }
response = g.proc(json.dumps(request))
print(response)

# Service.parse(string, dict)
# Service.parse(string[], dict)
response = g.parse(
    "今日は上野の野外ステージでコンサートだった。（事務所：東京都台東区上野公園7-47）",
    {"show-candidate": False, "geojson": True, "geocoding": True}
)
pp.pprint(response)

# Service.parseStructured(string[], dict)
response = g.parseStructured(
    [
        {
            "organization": {
                "surface": "NII",
                "name": "国立情報学研究所",
                "tel": "03-4212-2000（代表）"
            }
        },
        "は千代田区一ツ橋１－２－１にあります。神保町から徒歩3分。"
    ],
    {"show-candidate": False, "geocoding": "minimum"}
)
pp.pprint(response)

# Service.search(string, dict)
response = g.search("四ッ谷")
pp.pprint(response)

# Service.getGeoInfo(string[])
response = g.getGeoInfo(["tp1al0", "rQ1HpF"])
pp.pprint(response)

# Service.getDictionaries()
response = g.getDictionaries()
pp.pprint(response)

# Service.getDictionaryInfo(int[])
response = g.getDictionaryInfo([28, 29])
pp.pprint(response)

# Service.addressGeocoding(string, dict)
response = g.addressGeocoding("千代田区一ツ橋２－１－２", {"geocoding": "full"})
pp.pprint(response)


# MA
ma = pygeonlp.MA()

# MA.parse(string)
print(ma.parse("今日は上野の野外ステージでコンサートだった。"))

# MA.parseNode(string)
pp.pprint(ma.parseNode("今日は上野の野外ステージでコンサートだった。"))
