from osgeo import ogr
import pygeonlp
import json
import urllib.request
from pygeonlp_filter import st_filter


def _get_geometry_from_geojson_url(url):
    '''Get geojson from the url and create osgeo.ogr.Geometry'''

    geo = None

    try:
        req = urllib.request.Request(url)
        with urllib.request.urlopen(req) as res:
            geojson = res.read().decode('utf-8')

        geo = _get_geometry_from_geojson_string(geojson)

    except urllib.URLError as e:
        raise e

    return geo


def _get_geometry_from_geojson_string(geojson):
    '''Get osgeo.ogr.Geometry from a generic geojson'''

    geoobj = json.loads(geojson)

    if geoobj['type'] == 'FeatureCollection':
        geojson = json.dumps(geoobj['features'][0]['geometry'])
    elif geoobj['type'] == 'Feature':
        geojson = json.dumps(geoobj['geometry'])

    geo = ogr.CreateGeometryFromJson(geojson)

    if not geo:
        raise ValueError('Cannot parse the given geojson: ' + geojson)

    return geo


def _get_geometry(str):
    '''Get osgeo.ogr.Geometry from geojson or url'''

    try:
        json.loads(str)
        geo = _get_geometry_from_geojson_string(str)
        return geo
    except json.decoder.JSONDecodeError:
        pass

    geo = _get_geometry_from_geojson_url(str)

    return geo


def geo_contains(geonlp_response, geojson):
    '''The geojson 'contains' the elements.'''

    geo = _get_geometry(geojson)

    def _geo_contains(candidate):
        return _spatial_filter(geo.Contains, candidate)

    return st_filter.apply(_geo_contains, geonlp_response)


def geo_disjoint(geonlp_response, geojson):
    '''The geojson 'disjoint' the elements.'''

    geo = _get_geometry(geojson)

    def _geo_disjoint(candidate):
        return _spatial_filter(geo.Disjoint, candidate)

    return st_filter.apply(_geo_disjoint, geonlp_response)


def _spatial_filter(func, candidate):
    '''Apply filter function to the candidate'''

    if candidate['longitude'] == '' or candidate['latitude'] == '':
        return False

    point = ogr.Geometry(ogr.wkbPoint)
    lon = float(candidate['longitude'])
    lat = float(candidate['latitude'])
    point.AddPoint(lon, lat)

    if func(point):
        return True

    return False
