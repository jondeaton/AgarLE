#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt


def main():
    
    s = np.load("next_state.npy")
    plt.figure()
    for i in range(4):
        plt.imshow(s[i])
        plt.show()

if __name__ == "__main__":
    main()
