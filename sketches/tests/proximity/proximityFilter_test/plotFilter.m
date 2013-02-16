x = xz(:,2);
z = xz(:,1);
t = 1:length(z);
plot(t,x');
hold on
plot(t,z','r');