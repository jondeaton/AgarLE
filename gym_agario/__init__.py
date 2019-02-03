#!/usr/bin/env python
"""
File: __init__.py
Date: 1/27/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

from gym.envs.registration import register

register(
    id='agar-v0',
    entry_point='gym_agar.envs:AgarEnv',
)
register(
    id='agar-extrahard-v0',
    entry_point='gym_foo.envs:FooExtraHardEnv',
)
