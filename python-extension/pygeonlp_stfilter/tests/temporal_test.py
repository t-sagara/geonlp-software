#!python
# -*- utf-8 -*-
import pygeonlp
from pygeonlp_filter import temporal
import json
import unittest


class TemporalTest(unittest.TestCase):

    def setUp(self):
        service = pygeonlp.Service()
        self.sentence = "上野はパンダ誕生でにぎわっています。"
        self.response = service.parse(
            self.sentence, {"show-candidate": True, "geojson": True})

    def test_time_exists(self):
        result = temporal.time_exists(self.response, '2019-01-01')

        for i, feature in enumerate(result['features']):
            if feature['geometry'] is None:
                continue  # not geo-word object
            if 'candidates' not in feature['properties']:
                continue  # no candidates

            candidates = feature['properties']['candidates']
            self.assertIsInstance(candidates, list)
