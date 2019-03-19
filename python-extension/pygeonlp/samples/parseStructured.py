import pygeonlp
service = pygeonlp.Service()
print(service.parseStructured(
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
))
