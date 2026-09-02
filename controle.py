import numpy as np
from numpy import pi

import matplotlib.pyplot as plt

import scipy.signal as sc
import scipy.linalg as la

# ===============================
# tempo simualacao
# ===============================

tf = 0.1
dt = 1e-6

time = np.arange(0, tf, step=dt)
print(f'Samples: {tf/dt}')

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

# ===============================
# ESPACO DE ESTADOS
# ===============================

# x = [ia, wm]^T

A = np.array([
	[-Ra/La, -Kte/La],
	[Kte/J, -B/J]
])

B = np.array([[1/La],[0]])

C = np.array([[0,1]])

print(f'Auto valores do sistema: {la.eigvals(A)}')

# ===============================
# malha aberta
# ===============================

u = 4
ia = 0
wm = 0

x = np.array([[ia],[wm]])
xdot = np.zeros((2,1))

ylog = np.zeros(len(time))

for k in range(len(time)-1):
	xdot = A @ x + B*u
	ia += xdot[0,0] * dt
	wm += xdot[1,0] * dt

	x = x + xdot * dt

	ylog[k+1] = wm

plt.plot(time,ylog.T,label='rad/s')
plt.grid()
plt.xlabel('s')
plt.ylabel('rad/s')
plt.show()


# ===============================
# controlabilidade
# ===============================

U = np.concat([B, A@B], axis=1)

if np.linalg.matrix_rank(U) == A.shape[0]:
	print("Sistema controlavel")
else:
	print("Sistema nao controlavel")

polos = [-10, -20]

k = sc.place_poles(A,B,polos).gain_matrix
print(f'Ganhos K = {k}')


print(f'Auto valores do sistema A - BK: {la.eigvals(A-B@k)}')
