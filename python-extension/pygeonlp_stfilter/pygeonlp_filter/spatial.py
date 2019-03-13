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


def geo_contains(geonlp_response, geojson, **kwargs):
    '''The geojson 'contains' the elements.'''

    def _geo_contains(geo, candidate):
        return _spatial_filter(geo.Contains, candidate)

    geo = _get_geometry(geojson)
    return st_filter.apply(_geo_contains, geo, geonlp_response, **kwargs)


def geo_disjoint(geonlp_response, geojson, **kwargs):
    '''The geojson 'disjoint' the elements.'''

    def _geo_disjoint(geo, candidate):
        return _spatial_filter(geo.Disjoint, candidate)

    geo = _get_geometry(geojson)
    return st_filter.apply(_geo_disjoint, geo, geonlp_response, **kwargs)


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


if __name__ == '__main__':
    geojson = '{"type":"Polygon","coordinates":[[[139.457008,35.513569],[140.011817,35.513569],[140.011817,36.030563],[139.457008,36.030562],[139.457008,35.513569]]]}'

    g = pygeonlp.Service()
    request = {
        "method": "parse",
        "params": ["今日は上野の野外ステージでコンサートだった。（事務所：東京都台東区上野公園7-47）", {"show-candidate": True, "geojson": True}],
        "id": 1
    }

    response = json.loads(g.proc(json.dumps(request)))

    result1 = geo_disjoint(
        response, geojson)
    print(result1)
