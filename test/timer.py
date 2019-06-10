#!/usr/bin/env python
'''
times how fast we can step the game
'''

import numpy as np
import agario_full_env
from timeit import default_timer as timer

env = agario_full_env.Environment(4)

n = 1000
l = list()

start = timer()
for _ in range(n):
    env.step()
    s = env.get_state()
    l.append(s)

end = timer()
dt = end - start
print("Time for %d steps: %s sec" % (n, dt))
print("%s frames / sec" % (n / dt))

