import pygeonlp
service = pygeonlp.Service()
request = """
{"method": "parse",\
 "params": ["沖縄県の南海上で台風が発生しました。",\
  {"show-candidate": false, "geojson": true}],\
 "id": 1}mport pygeonlp
"""
print(service.proc(request))
