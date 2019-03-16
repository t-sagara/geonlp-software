#!python
# -*- coding:utf-8 -*-
import pygeonlp
import json
import pprint
import re
import unittest


class ModuleTestCase(unittest.TestCase):

    def test_version(self):
        self.assertTrue(re.match('^\d+\.\d+\.\d+$', pygeonlp.version()))


class ServiceTestCase(unittest.TestCase):

    def setUp(self):
        self.service = pygeonlp.Service()
        self.pp = pprint.PrettyPrinter(indent=1, compact=True)
        self.sentence = "国立情報学研究所は千代田区一ツ橋２－１－２にあります。神保町から徒歩3分。"
        self.request = {
            "method": "parse",
            "params": [
                self.sentence,
                {
                    "geojson": False,
                    "geocoding": True
                }
            ],
            "id": 1
        }

    def test_proc_list(self):
        self.request['method'] = 'parse'
        self.request['params'][1]['geojson'] = False
        response = json.loads(self.service.proc(json.dumps(self.request)))
        self.assertIsInstance(response['result'], list)
        self.assertEqual(len(response['result']), 5)

    def test_proc_geojson(self):
        self.request['method'] = 'parse'
        self.request['params'][1]['geojson'] = True
        response = json.loads(self.service.proc(json.dumps(self.request)))
        self.assertIsInstance(response['result'], dict)

    def test_parse(self):
        '''Service.parse(string[], dict)
        '''
        response = self.service.parse(
            self.sentence,
            {"show-candidate": False, "geojson": True, "geocoding": True}
        )
        self.assertIsInstance(response, dict)

    def test_parseStructured(self):
        '''Service.parseStructured(string[], dict)
        '''
        response = self.service.parseStructured(
            [
                {
                    "organization": {
                        "surface": "NII",
                        "name": "国立情報学研究所",
                        "tel": "03-4212-2000（代表）"
                    }
                },
                "千代田区一ツ橋１－２－１にあります。神保町から徒歩3分。"
            ],
            {"show-candidate": False, "geocoding": "minimum"}
        )
        self.assertIsInstance(response, list)

    def test_search(self):
        '''Service.search(string, dict)
        '''
        response = self.service.search("東京都")
        self.assertIsInstance(response, dict)
        answers = [
            'ALRYpP'
        ]
        for id, desc in response.items():
            self.assertTrue(id in answers, "Fail, '" +
                            id + "' is not in the list")

    def test_getGeoInfo(self):
        '''Service.getGeoInfo(string[])
        '''
        response = self.service.getGeoInfo(["tp1al0", "rQ1HpF"])
        self.assertEqual(len(response), 2)

    def test_getDictionaries(self):
        '''Service.getDictionaries()
        '''
        response = self.service.getDictionaries()
        self.assertIsInstance(response, dict)
        for id, desc in response.items():
            self.assertIsInstance(id, str)
            self.assertIsInstance(desc, dict)
            self.assertIsInstance(desc['title'], str)

    def test_getDictionaryInfo(self):
        '''Service.getDictionaryInfo(int[])
        '''
        response = self.service.getDictionaryInfo([28, 29])
        self.assertEqual(len(response), 2)

    def test_addressGeocoding(self):
        '''Service.addressGeocoding(string, dict)
        '''
        response = self.service.addressGeocoding(
            "千代田区一ツ橋２－１－２", {"geocoding": "full"})
        lonlat = response['geometry']['coordinates']
        self.assertTrue(lonlat[0] - 139.758606 +
                        lonlat[1] - 35.691406 < 0.0001)


class MaTestCase(unittest.TestCase):

    def setUp(self):
        self.ma = pygeonlp.MA()
        self.sentence = "神保町駅から新宿駅へは都営新宿線が便利です。"

    def test_parse(self):
        '''MA.parse(string)
        '''
        self.assertIsInstance(self.ma.parse(self.sentence), str)

    def test_parseNode(self):
        '''MA.parseNode(string)
        '''
        self.assertIsInstance(self.ma.parseNode(self.sentence), list)


if __name__ == '__main__':
    unittest.main()
