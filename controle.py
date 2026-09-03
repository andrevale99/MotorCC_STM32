import numpy as np
from numpy import pi

import matplotlib.pyplot as plt

import scipy.signal as sc
import scipy.linalg as la

import sys

# ===============================
# funcoes
# ===============================

def dinamica(A,B,C,D,k):
	pass

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

print(f'Auto valores do sistema: {la.eigvals(A)}\n')

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

del ylog

# ===============================
# controlabilidade
# ===============================

U = np.concat([B, A@B], axis=1)

if np.linalg.matrix_rank(U) == A.shape[0]:
	print("Sistema controlavel")
else:
	print("Sistema nao controlavel")

polos = [-200, -100]

k = np.array(sc.place_poles(A,B,polos).gain_matrix)
print(f'Ganhos K = {k}')

print(f'Auto valores do sistema A - BK: {la.eigvals(A-B@k)}\n')

# ===============================
# sistema realimentado
# ===============================

u = 4
ia = 0
wm = 0

x = np.array([[ia],[wm]])
xdot = np.zeros((2,1))

ylog = np.zeros(len(time))
ulog = np.zeros(len(time))

for k_ in range(len(time)-1):

	xdot = (A - B@k) @ x + B*u
	ia += xdot[0,0] * dt
	wm += xdot[1,0] * dt

	x = x + xdot * dt

	ylog[k_+1] = wm
	ulog[k_+1] = (u-k@x).item()

plt.subplot(211)
plt.plot(time,ylog.T,label='rad/s')
plt.grid()
plt.xlabel('s')
plt.ylabel('rad/s')

plt.subplot(212)
plt.plot(time,ulog,label='V')
plt.grid()
plt.xlabel('s')
plt.ylabel('V')

plt.tight_layout()

plt.show()

del ylog, ulog

# ===============================
# sistema realimentado com integrador
# ===============================

# Expandindo as matrizes para incluir o integrador
# e o estado erro
Ahat = np.concatenate((A, np.zeros((A.shape[0],1))), axis=1)
temp = np.concatenate((-C, np.array([[0]])), axis=1)
Ahat = np.concatenate((Ahat, temp), axis=0)

Bhat = np.concatenate((B, np.array([[0]])), axis=0)
Chat = np.concatenate((C, np.array([[0]])), axis=1)

polos = [-200, -20, -15]

# Ganho de realimentatacao e de integracao
Kaug = np.array(sc.place_poles(Ahat,Bhat,polos).gain_matrix)
k = Kaug[0, :-1]
ki = Kaug[0, -1]
print(f'Ganho dos estados K = {k}')
print(f'Ganho integral Ki = {ki}')

# Entrada da referência
Bref = np.array([
    [0],
    [0],
    [1]
])

print(f'autovalores da matriz A com integrador = {np.linalg.eigvals(Ahat-Bhat@Kaug)}')

ref = 150 * 2*np.pi / 60.
ia = 0
wm = 0
e = 0

x = np.array([[ia],[wm],[e]])
xdot = np.zeros((3,1))

ylog = np.zeros(len(time))
ulog = np.zeros(len(time))
elog = np.zeros(len(time))


for k_ in range(len(time)-1):
	# erro de velocidade
	e = ref - wm
	
	u = -(Kaug @ x).item()	
	
	xdot = (Ahat - Bhat @ Kaug) @ x + Bref * ref
	
	x = x + xdot * dt

	wm += x[1,0] * dt
	
	ylog[k_+1] = x[1,0]
	ulog[k_+1] = u
	elog[k_+1] = x[2, 0]

plt.subplot(211)

plt.plot(time, ylog, label='Velocidade')
plt.axhline(ref, linestyle='--', label='Referência')

plt.grid()
plt.xlabel('Tempo [s]')
plt.ylabel('rad/s')
plt.legend()


plt.subplot(212)

plt.plot(time, ulog, label='Tensão')

plt.grid()
plt.xlabel('Tempo [s]')
plt.ylabel('V')
plt.legend()

plt.tight_layout()
plt.show()

