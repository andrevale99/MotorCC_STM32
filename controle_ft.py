import numpy as np
import matplotlib.pyplot as plt

from scipy import signal


# ============================================================
# PARÂMETROS DO MOTOR
# ============================================================

J = 0.002       # kg.m²
B = 1.9e-3      # N.m.s


# ============================================================
# ESPECIFICAÇÕES DO CONTROLADOR
# ============================================================

overshoot = 0.10        # 10 %
Ts = 0.4                # tempo de acomodação de 2% [s]


# ============================================================
# CÁLCULO DE ZETA
# ============================================================

zeta = -np.log(overshoot) / np.sqrt(
    np.pi**2 + np.log(overshoot)**2
)


# ============================================================
# CÁLCULO DA FREQUÊNCIA NATURAL
# ============================================================

wn = 4 / (zeta * Ts)


print("========================================")
print("ESPECIFICAÇÕES")
print("========================================")

print(f"Overshoot       = {overshoot*100:.2f} %")
print(f"Ts (2%)         = {Ts:.4f} s")
print(f"Zeta            = {zeta:.4f}")
print(f"Wn              = {wn:.4f} rad/s")


# ============================================================
# POLOS DESEJADOS
# ============================================================

sigma = zeta * wn

wd = wn * np.sqrt(1 - zeta**2)

p1 = -sigma + 1j * wd
p2 = -sigma - 1j * wd

# Terceiro polo mais rápido
alpha = 5

p3 = -alpha * wn


print("\n========================================")
print("POLOS DESEJADOS")
print("========================================")

print(f"p1 = {p1}")
print(f"p2 = {p2}")
print(f"p3 = {p3}")


# ============================================================
# POLINÔMIO DESEJADO
# ============================================================

polos_desejados = [p1, p2, p3]

polinomio = np.poly(polos_desejados)

# Normaliza para o coeficiente de s³ ser 1
polinomio = np.real(polinomio)

print("\n========================================")
print("POLINÔMIO DESEJADO")
print("========================================")

print(
    f"s³ + {polinomio[1]:.4f}s² "
    f"+ {polinomio[2]:.4f}s "
    f"+ {polinomio[3]:.4f}"
)


# ============================================================
# CÁLCULO DOS GANHOS DO PID
#
# J*s³ + (B + Kd)*s² + Kp*s + Ki
# ============================================================

a2 = polinomio[1]
a1 = polinomio[2]
a0 = polinomio[3]


Kd = J * a2 - B
Kp = J * a1
Ki = J * a0


print("\n========================================")
print("GANHOS PID")
print("========================================")

print(f"Kp = {Kp:.6f}")
print(f"Ki = {Ki:.6f}")
print(f"Kd = {Kd:.6f}")


# ============================================================
# MOTOR SEM CONTROLADOR
#
# G(s) = 1 / (J*s + B)
# ============================================================

num_motor = [1]

den_motor = [
    J,
    B
]

motor = signal.TransferFunction(
    num_motor,
    den_motor
)


# ============================================================
# MOTOR COM PID
#
# T(s) =
#
#       Kd*s² + Kp*s + Ki
# --------------------------------
# J*s³ + (B+Kd)*s² + Kp*s + Ki
#
# ============================================================

num_pid = [
    Kd,
    Kp,
    Ki
]

den_pid = [
    J,
    B + Kd,
    Kp,
    Ki
]


motor_pid = signal.TransferFunction(
    num_pid,
    den_pid
)


# ============================================================
# SIMULAÇÃO
# ============================================================

tf = 0.8
dt = 1e-5

t = np.arange(0, tf, dt)


# Referência de velocidade
# 100 rad/s
reference = 100 * 2*np.pi / 60.


# ------------------------------------------------------------
# RESPOSTA SEM CONTROLADOR
# ------------------------------------------------------------

t1, y1 = signal.step(
    motor,
    T=t
)

# Entrada unitária da função de transferência
# Ajustamos para a referência desejada
y1 = y1 * reference


# ------------------------------------------------------------
# RESPOSTA COM PID
# ------------------------------------------------------------

t2, y2 = signal.step(
    motor_pid,
    T=t
)

y2 = y2 * reference


# ============================================================
# RESULTADOS
# ============================================================

print("\n========================================")
print("RESULTADOS")
print("========================================")

print(
    f"Velocidade final sem PID = "
    f"{y1[-1]:.4f} rad/s"
)

print(
    f"Velocidade final com PID = "
    f"{y2[-1]:.4f} rad/s"
)


# ============================================================
# GRÁFICO
# ============================================================

plt.figure(figsize=(10, 6))

#plt.plot(
#    t,
#    y1,
#    label="Motor sem controlador"
#)

plt.plot(
    t,
    y2,
    label="Motor + PID"
)

plt.axhline(
    reference,
    linestyle="--",
    label="Referência"
)

plt.xlabel("Tempo [s]")
plt.ylabel("Velocidade [rad/s]")

plt.title(
    "Resposta da parte mecânica do motor CC"
)

plt.grid()
plt.legend()

plt.tight_layout()
plt.show()
