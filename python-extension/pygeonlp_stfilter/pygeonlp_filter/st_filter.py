from osgeo import ogr
import pygeonlp
import json
import datetime
import re
import urllib.request


def apply(func, geonlp_response):
    '''Generic filter method for spatio-temporal conditions.
Generate and return the modified result
containing only elements passed through the filter function.
'''
    response_type = get_response_type(
        geonlp_response)  # validate the response

    if response_type == 'collection':
        # Feature collection type response
        features = []

        for feature in geonlp_response['result']['features']:
            if 'geometry' not in feature or feature['geometry'] is None:
                features.append(feature)
                continue

            if 'candidates' in feature['properties']:
                candidates = get_filtered_candidates(
                    func,
                    feature['properties']['candidates'],
                    default=feature['properties']['geonlp_id'])
                if not candidates:
                    features.append({
                        'type': 'Feature',
                        'geometry': None,
                        'properties': {
                            'surface': feature['properties']['surface']
                        }
                    })
                else:
                    top = candidates[0]
                    new_feature = {
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
                    new_feature['properties']['surface'] = feature['properties']['surface']
                    new_feature['properties']['candidates'] = candidates
                    features.append(new_feature)

        geonlp_response['result']['features'] = features

    elif response_type == 'list':
        # List type response
        elements = []

        for element in geonlp_response['result']:
            if 'geo' not in element:
                elements.append(element)
                continue

            if 'candidates' in element:
                candidates = get_filtered_candidates(
                    func,
                    element['candidates'],
                    default=element['geo']['properties']['geonlp_id']
                )
                if not candidates:
                    elements.append({'surface': element['surface']})
                else:
                    top_candidate = candidates[0]
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
                    element['candidates'] = candidates
                    elements.append(element)

            else:
                candidate = element['geo']['properties']
                if func(candidate):
                    elements.append(element)
                else:
                    elements.append({'surface': element['surface']})

        geonlp_response['result'] = elements

    return geonlp_response


def get_filtered_candidates(func, candidates, **kwargs):
    '''Apply filter_function ('func') to each candidates
('candidates'), and returns passed ones.
if default candidate is specified by its geonlp_id (as 'default' param),
the candidate will be placed at the top of the list.
'''
    top_candidate = None
    passed = []

    for candidate in candidates:
        if func(candidate):
            if top_candidate is None \
                    and 'geonlp_id' in candidate \
                    and ('default' in kwargs) \
                    and kwargs['default'] == candidate['geonlp_id']:
                top_candidate = candidate
            else:
                passed.append(candidate)

    if top_candidate is not None:
        passed.insert(0, top_candidate)

    return passed


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
