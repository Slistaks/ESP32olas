


clear all;
hold off;





# basado en sensor 1 Cs 110pF ensayo 2
x= [40.9853, 45.5133, 49.5185, 53.1762, 56.3458, 59.06, 61.2094, 63.2403];
y= [20, 25, 30, 35, 40, 45, 50, 55];

orden= 2;
p= polyfit(x, y, orden);

c= x;
h_mm= p(1)*c.^2 + p(2)*c + p(3);

plot(c, h_mm)
#axis([min(c), max(c), min(h_mm), max(h_mm)])
axis([30, 66, 10, 60])
clearvars x y c p




#{

# basado en sensor 2 Cs 110pF ensayo m
# VER. ensayos en tanque grande, rango grande dan mucho mas ruido
# que el resto de los sensores y que el mismo sensor en el otro tanque
# y con el otro rango.

x= [];
y= [20, 25, 30, 35, 40, 45, 50, 55];

orden= 2;
p= polyfit(x, y, orden);

c= x;
h_mm= p(1)*c.^2 + p(2)*c + p(3);

plot(c, h_mm)
#axis([min(c), max(c), min(h_mm), max(h_mm)])
clearvars x y c p
#}







# basado en sensor 3 Cs 110pF ensayo 1

x= [37.3941, 41.9012, 46.2451, 50.2063, 53.3026, 56.4272, 59.1284, 61.6355];
y= [20, 25, 30, 35, 40, 45, 50, 55];

orden= 2;
p= polyfit(x, y, orden);

#c= 37:0.1:62;
c= x;
h_mm= p(1)*c.^2 + p(2)*c + p(3);

hold on
plot(c, h_mm)
#axis([min(c), max(c), min(h_mm), max(h_mm)])
clearvars x y c p







# basado en sensor 4 Cs 110pF ensayo 4 (1er ensayo rango nuevo)
x= [37.8754, 44.0793, 48.5825, 52.7887, 56.3522, 59.5965, 62.53, 65.2579];
y= [15, 20, 25, 30, 35, 40, 45, 50];

orden= 2;
p= polyfit(x, y, orden);

c= x;
h_mm= p(1)*c.^2 + p(2)*c + p(3);

plot(c, h_mm)
#axis([min(c), max(c), min(h_mm), max(h_mm)])
clearvars x y c p







# basado en sensor 5 Cs 110pF ensayo 3
x= [37.9037, 43.1314, 47.7578, 52.0205, 55.91, 59.2366, 62.0879, 64.7029];
y= [15, 20, 25, 30, 35, 40, 45, 50];

orden= 2;
p= polyfit(x, y, orden);

c= x;
h_mm= p(1)*c.^2 + p(2)*c + p(3);

plot(c, h_mm)
#axis([min(c), max(c), min(h_mm), max(h_mm)])
clearvars x y c p






# basado en sensor 6 Cs 110pF ensayo 1
x= [38.508, 44.5503, 49.0478, 53.1451, 56.557, 59.8354, 62.6189, 65.4777];
y= [15, 20, 25, 30, 35, 40, 45, 50];

orden= 2;
p= polyfit(x, y, orden);

c= x;
h_mm= p(1)*c.^2 + p(2)*c + p(3);

plot(c, h_mm)
#axis([min(c), max(c), min(h_mm), max(h_mm)])
clearvars x y c p


























