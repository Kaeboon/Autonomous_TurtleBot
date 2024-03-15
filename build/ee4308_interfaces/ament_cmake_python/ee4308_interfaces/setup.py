from setuptools import find_packages
from setuptools import setup

setup(
    name='ee4308_interfaces',
    version='0.0.0',
    packages=find_packages(
        include=('ee4308_interfaces', 'ee4308_interfaces.*')),
)
