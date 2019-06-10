#!/usr/bin/env python
"""
File: __init__.py
Date: 1/27/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

try:
    from gym_agario.envs.FullEnv import FullEnv
except ImportError:
    pass

try:
    from gym_agario.envs.ScreenEnv import ScreenEnv
except ImportError:
    pass

try:
    from gym_agario.envs.GridEnv import GridEnv
except ImportError:
    pass