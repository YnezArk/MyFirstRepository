import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.integrate import solve_ivp
from scipy.interpolate import interp1d

# -----------------------------
# 修复中文显示
plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# -----------------------------
# 参数定义
M = 2000.0          # 车体质量 (kg)
m_e = 100.0         # 单个偏心块质量 (kg)
r = 0.2             # 旋转半径 (m)
omega_max = 100.0   # 最大角速度 (rad/s)
alpha_max = 5000.0  # 最大角加速度 (rad/s^2)
k = 7225200.0       # 刚度 (N/m)
c = 3600.0          # 阻尼 (N·s/m)

# 双电机对称布置：合力为 Fx = 2 * m_e * r * omega^2 * cos(theta)
# 若两偏心块反向旋转且对称安装，则横向力为：Fy = 2 * m_e * r * omega^2 * sin(theta)
# 但若我们直接控制 theta(t)，且让 omega = dtheta/dt，则瞬时横向力为：
# F_act = 2 * m_e * r * [ -sin(theta) * alpha - cos(theta) * omega^2 ] ？
# 实际上，更简单做法：假设我们能直接生成所需控制力 Fc(t)，再反推所需 omega 和 theta。
# 但题目要求输出“角位移曲线”，因此我们需构造 theta(t) 使得产生的离心力 ≈ -扰动力。

# 然而，离心力大小为 F = m_e * r * omega^2，方向由 theta 决定。
# 对于双电机对称反向旋转（如PDF图2），横向合力为：
# Fy = 2 * m_e * r * omega^2 * sin(phi) ，其中 phi 是偏心质量相对于y轴的角度。
# 若我们令两个电机同步调节 phi(t)，则可产生任意方向的横向力（最大值 F_max = 2*m_e*r*omega_max^2）

F_max = 2 * m_e * r * omega_max**2  # 最大可控横向力
print(f"最大可控作动力: {F_max:.2f} N")

# -----------------------------
# 读取扰动力数据
df = pd.read_excel("ProblemA_Scenario2.xlsx", header=None, names=["time", "force"])
t_data = df["time"].values
F_disturb = df["force"].values

# 时间范围
T_end = 10.0
dt = 0.01
t_eval = np.arange(0, T_end + dt, dt)

# 插值得到连续扰动力函数
F_interp = interp1d(t_data, F_disturb, kind='linear', fill_value="extrapolate")

# -----------------------------
# 动力学模型：M * x_ddot + c * x_dot + k * x = F_disturb + F_control
# 状态变量: [x, x_dot]

def vehicle_dynamics(t, y, F_dist_func, F_control_func):
    x, v = y
    F_d = F_dist_func(t)
    F_c = F_control_func(t)
    a = (F_d + F_c - c * v - k * x) / M
    return [v, a]

# 控制策略：
# 目标：抵消扰动力 → F_control ≈ -F_disturb
# 但受限于 F_max，故 F_control = clip(-F_disturb, -F_max, F_max)

def F_control_no_actuator(t):
    return 0.0

def F_control_with_actuator(t):
    F_desired = -F_interp(t)
    return np.clip(F_desired, -F_max, F_max)

# -----------------------------
# 初始条件
y0 = [0.01, 0.0]  # 初始位移 0.01 m, 初始速度 0

# 情况1：无作动器
sol1 = solve_ivp(
    lambda t, y: vehicle_dynamics(t, y, F_interp, F_control_no_actuator),
    [0, T_end], y0, t_eval=t_eval, method='RK45'
)

# 情况2：有作动器
sol2 = solve_ivp(
    lambda t, y: vehicle_dynamics(t, y, F_interp, F_control_with_actuator),
    [0, T_end], y0, t_eval=t_eval, method='RK45'
)

# 提取结果
t = sol1.t
x1, v1 = sol1.y
x2, v2 = sol2.y

# 计算加速度
a1 = (F_interp(t) + 0 - c * v1 - k * x1) / M
a2 = (F_interp(t) + np.clip(-F_interp(t), -F_max, F_max) - c * v2 - k * x2) / M

# -----------------------------
# 计算横向振动指数 Ih = (1/T) * ∫ a_y^2 dt
Ih1 = np.trapz(a1**2, t) / T_end
Ih2 = np.trapz(a2**2, t) / T_end

print(f"无作动器时横向振动指数 Ih = {Ih1:.4f}")
print(f"有作动器时横向振动指数 Ih = {Ih2:.4f}")
print(f"振动抑制率: {(1 - Ih2/Ih1)*100:.2f}%")

# -----------------------------
# 反推作动器角位移 theta(t)
# 假设：F_control = 2 * m_e * r * omega(t)^2 * sin(theta(t))
# 为简化，令 theta(t) = π/2（即 sin=1），则 F = 2*m_e*r*omega^2
# 所以 omega(t) = sqrt( |F_control| / (2*m_e*r) )
# 且符号由方向决定 → 我们只关心 magnitude，theta 固定为 π/2（或 -π/2）
# 但题目要求“角位移曲线”，即 theta(t)。若我们固定 theta=π/2，则角位移是常数，不合理。
#
# 更合理假设：我们通过调节 omega(t) 来改变力大小，theta(t) = ω_avg * t （匀速旋转）
# 但实际控制中，theta(t) = ∫ omega(t) dt
#
# 焀此，我们设定：作动器始终沿 y 方向发力 → theta(t) = π/2（恒定），但 omega(t) 可变。
# 然而角位移应为 theta(t) = ∫_0^t omega(τ) dτ
#
# 由 F_c(t) = 2 * m_e * r * omega(t)^2  => omega(t) = sqrt( |F_c(t)| / (2*m_e*r) )
# 且 omega(t) >= 0，方向由电机转向决定（不影响角位移积分）
#
# 因此：
F_c_used = np.clip(-F_interp(t), -F_max, F_max)
omega_t = np.sqrt(np.abs(F_c_used) / (2 * m_e * r))
# 限制 omega <= omega_max（理论上已满足，因 F_c_used 已限幅）
omega_t = np.clip(omega_t, 0, omega_max)

# 角位移 theta(t) = ∫ omega(t) dt
theta_t = np.cumsum(omega_t) * dt  # 简单矩形积分

# -----------------------------
# 绘图
plt.figure(figsize=(14, 10))

# 位移
plt.subplot(3, 1, 1)
plt.plot(t, x1, label='无作动器', linewidth=1.2)
plt.plot(t, x2, label='有作动器', linewidth=1.2)
plt.ylabel('横向位移 (m)')
plt.legend()
plt.grid(True)

# 加速度
plt.subplot(3, 1, 2)
plt.plot(t, a1, label='无作动器', linewidth=1.2)
plt.plot(t, a2, label='有作动器', linewidth=1.2)
plt.ylabel('横向加速度 (m/s²)')
plt.legend()
plt.grid(True)

# 角位移
plt.subplot(3, 1, 3)
plt.plot(t, theta_t, 'g', label='作动器角位移 θ(t)', linewidth=1.2)
plt.xlabel('时间 (s)')
plt.ylabel('角位移 (rad)')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.savefig("vibration_suppression_results.png", dpi=300)
plt.show()

# -----------------------------
# 输出数值结果
print("\n=== 结果汇总 ===")
print(f"最大可控作动力: {F_max:.2f} N")
print(f"无作动器 Ih: {Ih1:.6f}")
print(f"有作动器 Ih: {Ih2:.6f}")
print(f"角位移最终值: {theta_t[-1]:.2f} rad ({theta_t[-1]*180/np.pi:.1f}°)")