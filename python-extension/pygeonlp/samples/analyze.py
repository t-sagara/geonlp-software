import pygeonlp
service = pygeonlp.Service()
print(service.analyze(
    "沖縄県の南海上で台風が発生しました。"
))
