from setuptools import find_packages
from setuptools import setup

setup(
    name='agent_msgs',
    version='0.0.0',
    packages=find_packages(
        include=('agent_msgs', 'agent_msgs.*')),
)
