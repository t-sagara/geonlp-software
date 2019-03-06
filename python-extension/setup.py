from distutils.core import setup, Extension

module1 = Extension(
    'geonlp',
    define_macros=[
        ('MAJOR_VERSION', '1'),
        ('MINOR_VERSION', '0'),
        ('REVISION', '0')
    ],
    include_dirs=['/usr/include', '../include'],
    libraries=['geonlp', 'mecab', 'sqlite3'],
    library_dirs=['/usr/lib', '/usr/local/lib'],
    sources=['geonlp.cpp']
)

setup(
    name='geonlp',
    version='1.0.0',
    description='GeoNLP python module',
    author='Takeshi Sagara',
    author_email='sagara@info-proto.com',
    url='https://github.com/t-sagara/geonlp-software',
    long_description='''
An Python module of the GeoNLP software.
''',
    ext_modules=[module1]
)
