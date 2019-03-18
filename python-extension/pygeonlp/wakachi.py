import json
import pygeonlp

"""
自然言語文を分かち書きする。
地名語や住所は抽出する。
"""

sentence = "情報試作室は小田急多摩センター駅より徒歩７分。住所：多摩市落合1-15-2にあります。"

g = pygeonlp.Service()
for word in g.analyze(sentence):
    if not word:  # EOS
        break
    if "nodes" in word:
        for node in word["nodes"]:
            print("{} ({},{},{})".format(
                node['surface'],
                node['pos'],
                node['subclass1'],
                node['subclass2']))
