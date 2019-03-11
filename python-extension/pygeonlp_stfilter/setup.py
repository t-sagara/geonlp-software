from setuptools import setup

setup(
    name='pygeonlp_filter',
    version='1.0.0',
    author='Takeshi Sagara',
    author_email='sagara@info-proto.com',
    url='https://github.com/t-sagara/geonlp-software',
    license=license,
    description='Spatial Temporal Filter functions for PyGeoNLP',
    packages=['pygeonlp_filter'],
    install_requires=['gdal'],
    dependency_links=[
        'git+ssh://git@github.com/t-sagara/geonlp-software/PyGeoNLP'],
    test_suite='tests'
)
