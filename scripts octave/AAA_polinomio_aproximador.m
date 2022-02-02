


clear all;
hold off;





# basado en sensor 1 Cs 110pF ensayo 2
x1= [40.9853, 45.5133, 49.5185, 53.1762, 56.3458, 59.06, 61.2094, 63.2403];
y1= [20, 25, 30, 35, 40, 45, 50, 55];

orden= 2;
p1= polyfit(x1, y1, orden);

c1= x1;
h1_mm= p1(1)*c1.^2 + p1(2)*c1 + p1(3);

#plot(c1, h1_mm)
#axis([min(c1), max(c1), min(h1_mm), max(h1_mm)])





#{

# basado en sensor 2 Cs 110pF ensayo m
# VER. ensayos en tanque grande, rango grande dan mucho mas ruido
# que el resto de los sensores y que el mismo sensor en el otro tanque
# y con el otro rango.

x2= [];
y2= [20, 25, 30, 35, 40, 45, 50, 55];

orden= 2;
p2= polyfit(x2, y2, orden);

c2= x2;
h2_mm= p2(1)*c2.^2 + p2(2)*c2 + p2(3);

plot(c2, h2_mm)

#}







# basado en sensor 3 Cs 110pF ensayo 1

x3= [37.3941, 41.9012, 46.2451, 50.2063, 53.3026, 56.4272, 59.1284, 61.6355];
y3= [20, 25, 30, 35, 40, 45, 50, 55];

orden= 2;
p3= polyfit(x3, y3, orden);

#c3= 37:0.1:62;
c3= x3;
h3_mm= p3(1)*c3.^2 + p3(2)*c3 + p3(3);

#hold on
#plot(c3, h3_mm)







# basado en sensor 4 Cs 110pF ensayo 4 (1er ensayo rango nuevo)
x4= [37.8754, 44.0793, 48.5825, 52.7887, 56.3522, 59.5965, 62.53, 65.2579];
y4= [15, 20, 25, 30, 35, 40, 45, 50];

orden= 2;
p4= polyfit(x4, y4, orden);

c4= x4;
h4_mm= p4(1)*c4.^2 + p4(2)*c4 + p4(3);

plot(c4, h4_mm,'r')








# basado en sensor 5 Cs 110pF ensayo 3
x5= [37.9037, 43.1314, 47.7578, 52.0205, 55.91, 59.2366, 62.0879, 64.7029];
y5= [15, 20, 25, 30, 35, 40, 45, 50];

orden= 2;
p5= polyfit(x5, y5, orden);

c5= x5;
h5_mm= p5(1)*c5.^2 + p5(2)*c5 + p5(3);

hold on
plot(c5, h5_mm, 'g')







# basado en sensor 6 Cs 110pF ensayo 1
x6= [38.508, 44.5503, 49.0478, 53.1451, 56.557, 59.8354, 62.6189, 65.4777];
y6= [15, 20, 25, 30, 35, 40, 45, 50];

orden= 2;
p6= polyfit(x6, y6, orden);

c6= x6;
h6_mm= p6(1)*c6.^2 + p6(2)*c6 + p6(3);

plot(c6, h6_mm, 'k')










# curvas de error de usar una sola expresion para todas las sondas:
# tomando sensor 6 como referencia, ensayo 1

cap= 34:0.1:66;
yref= p6(1)*cap.^2 + p6(2)*cap + p6(3);
y_sensor4= p4(1)*cap.^2 + p4(2)*cap + p4(3);
y_sensor5= p5(1)*cap.^2 + p5(2)*cap + p5(3);
err_4= y_sensor4-yref;
err_5= y_sensor5-yref;



#hold off
plot(cap, err_4, 'g')
hold on
plot(cap, err_5, 'r')
#axis([min(cap), max(cap), -1, 10])















#axis([30, 66, 10, 60])







