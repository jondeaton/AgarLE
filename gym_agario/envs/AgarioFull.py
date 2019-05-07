#!/usr/bin/env python
"""
File: AgarEnv
Date: 1/27/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
from gym import error, spaces, utils
import numpy as np

from collections import namedtuple

import agario_full_env

Observation = namedtuple('Observation', ['viruses', 'foods', 'pellets', 'agent', 'others'])

class FeatureExtractor:

    def __init__(self, nvirus=5, npellets=50, nfood=10, nother=5, ncell=15):
        self.nfood = nfood
        self.nvirus = nvirus
        self.npellets = npellets
        self.nother = nother
        self.ncell = ncell
        self.size = 2 * nfood + 2 * nvirus + 2 * npellets + 5 * ncell + 5 * nother * ncell

    def extract(self, observation: Observation):
        """ extracts features from an observation into a fixed-size feature vector
        :param observation: a named tuple Observation object
        :return: fixed length vector of extracted features about the observation
        """
        agent = observation.agent
        loc = self.position(agent)

        close_foods = self.sort_by_proximity(loc, observation.foods, n=self.nfood)
        foods = np.zeros((self.nfood, 2))
        foods[:len(close_foods)] = close_foods

        close_viruses = self.sort_by_proximity(loc, observation.viruses, n=self.nvirus)
        viruses = np.zeros((self.nvirus, 2))
        viruses[:len(close_viruses)] = close_viruses

        close_pellets = self.sort_by_proximity(loc, observation.pellets, n=self.npellets)
        pellets = np.zeros((self.npellets, 2))
        pellets[:len(close_pellets)] = close_pellets

        largest_cells = self.largest_cells(agent, n=self.ncell)
        agent_cells = np.zeros((self.ncell, 5))
        agent_cells[:len(largest_cells)] = largest_cells

        feature_stacks = [foods, viruses, pellets, agent_cells]

        closest_players = self.closest_players(loc, observation.others, n=self.nother)
        for player in closest_players:
            p = np.zeros((self.ncell, 5))
            player_cells = self.largest_cells(player, n=self.ncell)
            p[:len(player_cells)] = player_cells
            feature_stacks.append(p)

        flattened = map(lambda arr: arr.flatten(), feature_stacks)
        features = np.hstack(flattened)
        return features


    def largest_cells(self, player, n=None):
        order =  np.argsort(player[:, -1], axis=0)
        return player[order[:n]]

    def closest_players(self, loc, others, n=None):
        locs = np.array([self.position(o) for o in others if o.size > 0])
        order = np.argsort(np.linalg.norm(loc - locs, axis=1))
        return [others[i] for i in order[:n]]

    def sort_by_proximity(self, loc, entities, n=None):
        positions = entities[:, (0, 1)]
        order = np.argsort(np.linalg.norm(positions - loc, axis=1))
        return entities[order[:n]]

    def position(self, player: np.ndarray):
        # weighted average of cell positions by mass
        loc = np.average(player[:, (0, 1)], axis=0, weights=player[:, -1])
        return loc


class AgarioFull(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self):
        self.viewer = None
        self.server_process = None
        self.server_port = None

        self.action_space = spaces.Tuple((spaces.Box(low=0, high=1, shape=(2,)),
                                          spaces.MultiBinary(1),
                                          spaces.MultiBinary(1)))

        self.observation_space = spaces.Dict({
            "viruses": spaces.Space(shape=(None, 2)),
            "foods": spaces.Space(shape=(None, 2)),
            "pellets": spaces.Space(shape=(None, 2)),
            "agent": spaces.Space(shape=(None, 5)),
            "others": spaces.Space(shape=(None, None, 5))
        })

        self._env = agario_full_env.Environment(4)

        self.prev_status = 0
        self.status = 0

    def __del__(self):
        pass

    def step(self, action):
        """ take an action in the environment, advancing the environment
        along until the next time step
        :param action: (x, y, a) where `x`, `y` are in [0, 1] and `a` is
        in {0, 1, 2} corresponding to nothing, split, feed, respectively.
        :return: tuple of - observation, reward, episode_over
            observation (object) : the next state of the world.
            reward (float) : reward gained during the time step
            episode_over (bool) : whether the game is over or not
            info (dict) : diagnostic information (currently empty)
        """
        x, y, game_act = action
        self._env.take_action(x, y, game_act)

        reward = self._env.step()
        state = self._env.get_state()

        observation = Observation(viruses=state[0], pellets=state[1],
                                       foods=state[2], agent=state[3], others=state[4:])

        episode_over = self._env.done()

        return observation, reward, episode_over, {}

    def reset(self):
        """
        resets the environment
        :return: None
        """
        self._env.reset()

    def render(self, mode='human'):
        self._env.render()
