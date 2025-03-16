% plot_voo.m
data = load('voo_sim.txt');
t     = data(:,1);
V     = data(:,2);
gamma = data(:,3);
x     = data(:,4);
h     = data(:,5);

% Figura 1
figure(1);
plot(t, V);
xlabel('Tempo (s)');
ylabel('Velocidade (m/s)');
title('Velocidade vs Tempo');
print('voo_graficos.pdf','-dpdf');

% Figura 2
figure(2);
plot(t, gamma);
xlabel('Tempo (s)');
ylabel('Ângulo gamma (rad)');
title('Ângulo vs Tempo');
print('voo_graficos.pdf','-dpdf','-append');

% Figura 3
figure(3);
plot(t, h);
xlabel('Tempo (s)');
ylabel('Altitude (m)');
title('Altitude vs Tempo');
print('voo_graficos.pdf','-dpdf','-append');

% Figura 4
figure(4);
plot(x, h);
xlabel('Posição Horizontal x (m)');
ylabel('Altitude h (m)');
title('Trajetória');
print('voo_graficos.pdf','-dpdf','-append');

% Figura 5: código
codigo = fileread('plot_voo.m');
figure(5); clf; axis off; hold on;
text(0, 1, codigo, 'Interpreter','none','FontName','Courier','FontSize',8,'VerticalAlignment','top');
print('voo_graficos.pdf','-dpdf','-append');
