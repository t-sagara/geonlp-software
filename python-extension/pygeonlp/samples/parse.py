import pygeonlp
service = pygeonlp.Service()
print(service.parse(
    "沖縄県の南海上で台風が発生しました。",
    {"show-candidate": False, "geojson": True}
))
