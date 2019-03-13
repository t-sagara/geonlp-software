import json
import datetime
import re
from pygeonlp_filter import st_filter


def time_exists(geonlp_response, date_from, date_to=None):
    '''The elements exist between the date_from and date_to'''

    def _time_exists(duration, candidate):
        if 'valid_from' in candidate:
            valid_from = _get_date_from_isostr(candidate['valid_from'])
            if valid_from > duration[1]:
                return False

        if 'valid_to' in candidate:
            valid_to = _get_date_from_isostr(candidate['valid_to'])
            if vald_to < duration[0]:
                return False

        return True

    duration = _get_duration_from_dates(date_from, date_to)
    return st_filter.apply(_time_exists, duration, geonlp_response, **kwargs)


def time_before(geonlp_response, date_from, date_to=None):
    '''The elements exist before the date_from'''

    def _time_befor(duration, candidate):
        if 'valid_from' in candidate:
            valid_from = _get_date_from_isostr(candidate['valid_from'])
            if valid_from > duration[0]:
                return False

        return True

    duration = _get_duration_from_dates(date_from, date_to)
    return st_filter.apply(_time_before, duration, geonlp_response, **kwargs)


def time_after(geonlp_response, date_from, date_to=None):
    '''The elements exist after the date_to'''

    def _time_after(duration, candidate):
        if 'valid_to' in candidate:
            valid_to = _get_date_from_isostr(candidate['valid_to'])
            if valid_to < duration[1]:
                return False

        return True

    duration = _get_duration_from_dates(date_from, date_to)
    return st_filter.apply(_time_after, duration, geonlp_response, **kwargs)


def time_overlaps(geonlp_response, date_from, date_to=None):
    '''The elements exist between the date_from and the date_to'''

    def _time_overlaps(duration, candidate):
        if 'valid_from' in candidate:
            valid_from = _get_date_from_isostr(candidate['valid_from'])
            if valid_from > duration[1]:
                return False

        if 'valid_to' in candidate:
            valid_to = _get_date_from_isostr(candidate['valid_to'])
            if valid_to < duration[0]:
                return False

        return True

    duration = _get_duration_from_dates(date_from, date_to)
    return st_filter.apply(_time_overlaps, duration, geonlp_response, **kwargs)


def time_contains(geonlp_response, date_from, date_to=None):
    '''The elements exist only between the date_from and the date_to'''

    def _time_contains(duration, candidate):
        if 'valid_from' in candidate:
            valid_from = _get_date_from_isostr(candidate['valid_from'])
            if valid_from < duration[0]:
                return False

        if 'valid_to' in candidate:
            valid_to = _get_date_from_isostr(candidate['valid_to'])
            if valid_to > duration[1]:
                return False

        return True

    duration = _get_duration_from_dates(date_from, date_to)
    return st_filter.apply(_time_contins, duration, geonlp_response, **kwargs)


def _get_date_from_isostr(datestr):
    re_date = r"(\d{4})[/\-\s](\d{1,2})[/\-\s](\d{1,2})"

    match = re.match(re_date, datestr)
    if not match:
        return False

    ymd = match.groups()
    date = datetime.date(int(ymd[0]), int(ymd[1]), int(ymd[2]))
    return date


def _get_duration_from_dates(datestr0, datestr1):
    '''Convert 2 date strings to a (date, date) tuple'''

    date0 = _get_date_from_isostr(datestr0)
    if date0 == False:
        raise TypeError('The 1st date format is invalid.')

    if not datestr1:
        date1 = date0
    else:
        date1 = _get_date_from_isostr(datestr1)
        if not date1:
            raise TypeError('The 2nd date format is invalid.')

    if (date0 > date1):
        date0, date1 = date1, date0

    return (date0, date1)
