% 增量式PID控制仿真 - 修正完整版

% 1. 定义连续系统模型
num = [1];          % 分子多项式
den = [1, 10, 20];  % 分母多项式
sys = tf(num, den); % 创建连续系统传递函数

% 2. 离散化系统
T = 0.01;           % 采样时间
sys_d = c2d(sys, T, 'zoh'); % 零阶保持器法离散化

% 3. PID参数设置
Kp = 50;
Ki = 0.5;
Kd = 2;
sim_time = 10;      % 仿真时间

% 4. 初始化变量
steps = sim_time/T;
r = ones(1, steps); % 阶跃输入
y = zeros(1, steps);
u = zeros(1, steps);
e = zeros(1, steps);

% 5. 获取离散系统参数
[num, den] = tfdata(sys_d, 'v');

% 6. 增量式PID函数定义
incremental_pid = @(e, e_prev, e_prev2, Kp, Ki, Kd, u_prev) ...
    u_prev + Kp*(e - e_prev) + Ki*e + Kd*(e - 2*e_prev + e_prev2);

% 7. 主仿真循环
for k = 3:steps
    % 计算误差
    e(k) = r(k) - y(k-1);
    
    % 增量式PID计算控制量
    u(k) = incremental_pid(e(k), e(k-1), e(k-2), Kp, Ki, Kd, u(k-1));
    
    % 控制量限幅
    u(k) = min(max(u(k), -10), 10);
    
    % 系统响应(使用离散系统差分方程)
    y(k) = -den(2)*y(k-1) - den(3)*y(k-2) + num(1)*u(k-1) + num(2)*u(k-2);
end

% 8. 绘制结果
t = 0:T:sim_time-T;
figure;
subplot(2,1,1);
plot(t, y, 'b', t, r, 'r--');
xlabel('Time (s)'); ylabel('Output');
legend('System Response', 'Reference');
title('System Response with Incremental PID');

subplot(2,1,2);
plot(t, u);
xlabel('Time (s)'); ylabel('Control Signal');
title('Control Signal');