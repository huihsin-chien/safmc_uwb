from setuptools import find_packages
from setuptools import setup

setup(
    name='uwb_msgs',
    version='0.0.0',
    packages=find_packages(
        include=('uwb_msgs', 'uwb_msgs.*')),
)
