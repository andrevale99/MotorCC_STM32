
import numpy as np
from numpy import pi

import matplotlib.pyplot as plt

import scipy.signal as sc
import scipy.linalg as la

import sys

width = 18
height = 10

# ===============================
# funcoes
# ===============================

# ===============================
# tempo simualacao
# ===============================

tf = 1.0
dt = 1e-5

time = np.arange(0, tf, step=dt)
print(f'Samples: {len(time)}\n')

# ===============================
# vars sistema
# ===============================

Ra = 2.6 #Ohm
La = 0.006245 #H
J = 0.002 #N.m
B = 1.9e-3 #N.m.s
Ke = 0.31 #V/ (rad/s)
Kt = Ke # (N.m)/A
Kte = Kt

