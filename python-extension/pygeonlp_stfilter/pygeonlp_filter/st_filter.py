from osgeo import ogr
import pygeonlp
import json
import datetime
import re
import copy
import urllib.request


def apply(func, param, geonlp_response, **kwargs):
    '''Generic filter method for spatio-temporal conditions.
Generate and return the modified result
containing only elements passed through the filter function.
'''
    response_type = get_response_type(
        geonlp_response)  # validate the response

    if 'copy' in kwargs:
        geonlp_response = copy.deepcopy(geonlp_response)

    keep_candidates = False
    if 'keep_candidates' in kwargs and kwargs['keep_candidates']:
        keep_candidates = True

    if response_type == 'collection':
        # Feature collection type response
        features = []

        for feature in geonlp_response['result']['features']:
            if 'geometry' not in feature or feature['geometry'] is None:
                features.append(feature)
                continue

            if 'candidates' in feature['properties']:
                results = _apply_filter(
                    func,
                    param,
                    feature['properties']['candidates'],
                    default=feature['properties']['geonlp_id'])

                if not results['passed']:
                    if keep_candidates:
                        del feature['properties']['candidates']
                        if 'excluded' not in feature['properties']:
                            feature['properties']['excluded'] = []
                        feature['properties']['excluded'] += results['excluded']
                    else:
                        feature = {
                            'type': 'Feature',
                            'geometry': None,
                            'properties': {
                                'surface': feature['properties']['surface']
                            }
                        }
                    features.append(feature)
                else:
                    top = copy.deepcopy(results['passed'][0])
                    top['surface'] = feature['properties']['surface']
                    feature = {
                        'type': 'Feature',
                        'geometry': {
                            'type': 'Point',
                            'coodinates': [
                                top['longitude'],
                                top['latitude']
                            ]
                        },
                        'properties': top
                    }
                    feature['properties']['candidates'] = results['passed']
                    if keep_candidates:
                        if 'excluded' not in feature['properties']:
                            feature['properties']['excluded'] = []
                        feature['properties']['excluded'] += results['excluded']
                    features.append(feature)
            else:
                candidate = feature['properties']
                if not func(param, candidate) and not keep_candidates:
                    feature = {
                        'type': 'Feature',
                        'geometry': None,
                        'properties': {
                            'surface': candidate['surface']
                        }
                    }

                features.append(feature)

        geonlp_response['result']['features'] = features

    elif response_type == 'list':
        # List type response
        elements = []

        for element in geonlp_response['result']:
            if 'geo' not in element:
                elements.append(element)
                continue

            if 'candidates' in element:
                results = _apply_filter(
                    func,
                    param,
                    element['candidates'],
                    default=element['geo']['properties']['geonlp_id']
                )
                if not results['passed']:
                    if keep_candidates:
                        del element['candidates']
                        if 'excluded' not in element:
                            element['excluded'] = []
                        element['excluded'] += results['excluded']
                    else:
                        element = {
                            'surface': element['surface']
                        }

                    elements.append(element)
                else:
                    top_candidate = copy.deepcopy(results['passed'][0])
                    element['geo'] = {
                        'type': 'Feature',
                        'properties': top_candidate,
                        'geometry': {
                            'type': 'Point',
                            'coordinates': [
                                float(top_candidate['longitude']),
                                float(top_candidate['latitude'])
                            ]
                        }
                    }
                    element['candidates'] = results['passed']
                    if keep_candidates:
                        if 'excluded' in element:
                            element['excluded'] = []
                        element['excluded'] += results['excluded']
                    elements.append(element)

            else:
                candidate = element['geo']['properties']
                if not func(param, candidate) and not keep_candidates:
                    element = {'surface': element['surface']}

                elements.append(element)

        geonlp_response['result'] = elements

    return geonlp_response


def _apply_filter(func, param, candidates, **kwargs):
    '''Apply filter_function ('func') to each candidates
('candidates'), and returns passed ones.
if default candidate is specified by its geonlp_id (as 'default' param),
the candidate will be placed at the top of the list.
'''
    top_candidate = None
    passed = []
    excluded = []

    for candidate in candidates:
        if func(param, candidate):
            if top_candidate is None \
                    and 'geonlp_id' in candidate \
                    and ('default' in kwargs) \
                    and kwargs['default'] == candidate['geonlp_id']:
                top_candidate = candidate
            else:
                passed.append(candidate)
        else:
            excluded.append(candidate)

    if top_candidate is not None:
        passed.insert(0, top_candidate)

    return {'passed': passed, 'excluded': excluded}


def get_response_type(geonlp_response):
    '''Identify type of the geonlp's parse results.
    Return 'collection' if the result is a GeoJSON feature collection.
    Otherwise, return 'list'.
    '''

    if 'result' not in geonlp_response:
        raise TypeError(
            'The response is not a geonlp response. (No \'result\' member)')

    if 'features' in geonlp_response['result']:
        return 'collection'

    if isinstance(geonlp_response['result'], list):
        return 'list'

    raise TypeError(
        'The response is not a geonlp response.'
        '(Neither a GeoJSON feature collection, nor a list object)')
