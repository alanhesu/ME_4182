clear;
close all;

num = xlsread('captest.xlsx', 'A2:F33');
capOut = num(:,6);
pwmOut = num(:,2);
figure;
scatter(capOut, pwmOut);
hold on;

fit = polyfit(capOut, pwmOut, 3);
x = linspace(min(capOut), max(capOut));
y = polyval(fit, x);
plot(x,y);
legend('Experimental data', 'Fitted trend');
title('Input voltage v. capacitor response');
xlabel('Capacitor response (V)');
ylabel('Input voltage (V)');

num = [.5, 0;
1, .021;
1.5, .189;
2, .402;
2.5, .630;
3, .970;
3.5, 1.389;
4, 2.052;
4.2, 2.554;
4.4, 2.996;
4.6, 3.622;
4.8, 4.074;
5,  4.571];

volt = num(:,1);
vel = num(:,2);
figure;
scatter(vel, volt);
hold on;

fit = polyfit(vel, volt, 5);
x = linspace(min(vel), max(vel));
y = polyval(fit, x);
plot(x,y);
legend('Experimental data', 'Fitted trend');
title('Cart speed v. input voltage');
xlabel('Cart speed (m/s)');
ylabel('Input voltage (m/s)');
set(gca, 'FontSize', 18);