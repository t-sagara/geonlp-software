#!python
# -*- utf-8 -*-
import pygeonlp
from osgeo import ogr
import json
from pygeonlp_filter import spatial
import pprint
import unittest


class SpatialTest(unittest.TestCase):

    def setUp(self):
        service = pygeonlp.Service()
        self.sentence = "上野はパンダ誕生でにぎわっています。"
        self.response = service.parse(
            self.sentence, {"show-candidate": True, "geojson": True})
        self.pp = pprint.PrettyPrinter(indent=1, compact=True)
        self.geojson = '{"type":"Polygon","coordinates":[[[139.457008,35.513569],[140.011817,35.513569],[140.011817,36.030563],[139.457008,36.030562],[139.457008,35.513569]]]}'
        self.geojson_url = 'http://geoshape.ex.nii.ac.jp/city/geojson/20180101/13/13106A1968.geojson'

    def test_geo_contains(self):
        result = spatial.geo_contains(
            self.response,
            self.geojson,
            keep_candidates=True, copy=True
        )

        for i, feature in enumerate(result['features']):
            if feature['geometry'] is None:
                continue  # not geo-word object
            if 'candidates' not in feature['properties']:
                continue  # no candidates

            candidates = feature['properties']['candidates']
            geo = spatial._get_geometry(self.geojson)

            for j, candidate in enumerate(candidates):
                point = ogr.Geometry(ogr.wkbPoint)
                lon = float(candidate['longitude'])
                lat = float(candidate['latitude'])
                point.AddPoint(lon, lat)
                self.assertTrue(geo.Contains(point))

    def test_geo_disjoint(self):
        result = spatial.geo_disjoint(
            self.response,
            self.geojson_url,
            keep_candidates=True, copy=True
        )

        for i, feature in enumerate(result['features']):
            if feature['geometry'] is None:
                continue  # not geo-word object
            if 'candidates' not in feature['properties']:
                continue  # no candidates

            candidates = feature['properties']['candidates']
            geo = spatial._get_geometry(self.geojson_url)

            for j, candidate in enumerate(candidates):
                point = ogr.Geometry(ogr.wkbPoint)
                lon = float(candidate['longitude'])
                lat = float(candidate['latitude'])
                point.AddPoint(lon, lat)
                self.assertTrue(geo.Disjoint(point))
