from setuptools import setup, Extension

with open("README.md", "r") as f:
    long_description = f.read()

setup(name='GymAgario',
      version='0.0.1',
      author="Jon Deaton",
      author_email="jonpauldeaton@gmail.com",
      description="Agar.io RL gym",
      url="https://github.com/jondeaton/AgarLE",
      install_requires=['gym', "numpy"],
      packages=["gym_agario", "gym_agario.envs"],
      long_description=long_description,
      platform="macOS,Linux",
      license="MIT")
