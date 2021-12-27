m = modbus('tcpip', '127.0.0.1', 502);  % 建立modbus通信（无线网络地址）
u=[1.5,1.3,1.3];
write(m,'holdingregs',2,u,'double');   % 将小车位置设定值、小车位置测量值和倒立摆角度写入控制器
write(m,'holdingregs',18,1);   % 将发起通信信号写入控制器
pause(0.01);    % 等待50ms（实际时间），使得通过modbus通信之后读到最新控制值
sys = read(m,'holdingregs',14,1,'double');  % 读取控制信号

