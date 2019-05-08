#!/usr/bin/env python
"""
File: __init__.py
Date: 1/27/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

try:
    from gym_agario.envs.AgarioFull import AgarioFull
except ImportError:
    pass

try:
    from gym_agario.envs.AgarioScreen import AgarioScreen
except ImportError:
    pass