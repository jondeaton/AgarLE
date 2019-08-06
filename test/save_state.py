#!/usr/bin/env python
import gym
import gym_agario

import numpy as np

def main():
    env_config = {
        'ticks_per_step': 4,
        'arena_size': 1000,
        'num_pellets': 1000,
        'num_viruses': 25,
        'num_bots': 25,
        'pellet_regen': True,
         'screen_len': 1024

    }

    env = gym.make("agario-screen-v0", **env_config)
    next_state, reward, done, info = env.step((0.0, 0.0, 0))
    np.save("next_state", next_state)


if __name__ == "__main__":
    main()
