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