


clear all;
clc
hold off;





# basado en sensor 1 Cs 110pF ensayo 3
x1= [35.9355, 41.1613, 45.7024, 49.6529, 53.2672, 56.4508, 59.1822, 61.3311, 63.3117];
y1= [150, 200, 250, 300, 350, 400, 450, 500, 550];

orden= 2;
p1= polyfit(x1, y1, orden);
p13= polyfit(x1, y1, 3);
c1= x1;
h1_mm= p1(1)*c1.^2 + p1(2)*c1 + p1(3);
h13_mm= p13(1)*c1.^3+p13(2)*c1.^2+p13(3)*c1+p13(4);

error1= h1_mm - y1;
error13= h13_mm - y1;

#plot(c1, y1, 'r;SN1;')
#hold on
#plot(c1, h1_mm, '--r')
#plot(c1, h13_mm, ':r')





##{

# basado en sensor 2 Cs 110pF ensayo m
# VER. ensayos en tanque grande rango grande, dan mucho mas ruido
# que el resto de los sensores y que el mismo sensor en el otro tanque
# y con el otro rango.

x2= [39.414, 44.745, 49.3135, 53.2732, 57.0294, 59.8405, 62.7705, 65.3291];
y2= [200, 250, 300, 350, 400, 450, 500, 550];

orden= 2;
p2= polyfit(x2, y2, orden);
p23= polyfit(x2, y2, 3);

c2= x2;
h2_mm= p2(1)*c2.^2 + p2(2)*c2 + p2(3);
h23_mm= p23(1)*c2.^3+p23(2)*c2.^2+p23(3)*c2+p23(4);


error2= h2_mm - y2;
error23= h23_mm - y2;


plot(c2, y2, 'm;SN2;')
hold on
plot(c2, h2_mm, '--m')
plot(c2, h23_mm, ':m')



hold on
#}







# basado en sensor 3 Cs 110pF ensayo 1

x3= [37.3941, 41.9012, 46.2451, 50.2063, 53.3026, 56.4272, 59.1284, 61.6355];
y3= [200, 250, 300, 350, 400, 450, 500, 550];

orden= 2;
p3= polyfit(x3, y3, orden);
p33= polyfit(x3, y3, 3);

#c3= 37:0.1:62;
c3= x3;
h3_mm= p3(1)*c3.^2 + p3(2)*c3 + p3(3);
h33_mm= p33(1)*c3.^3+p33(2)*c3.^2+p33(3)*c3+p33(4);

#hold on

error3= h3_mm - y3;
error33= h33_mm - y3;

#plot(c3, y3, 'g;SN3;')
#plot(c3, h3_mm, '--g')
#plot(c3, h33_mm, ':g')






# basado en sensor 4 Cs 110pF ensayo 4 (1er ensayo rango nuevo)
x4= [37.8754, 44.0793, 48.5825, 52.7887, 56.3522, 59.5965, 62.53, 65.2579];
y4= [150, 200, 250, 300, 350, 400, 450, 500];

orden= 2;
p4= polyfit(x4, y4, orden);
p43= polyfit(x4, y4, 3);

c4= x4;
h4_mm= p4(1)*c4.^2 + p4(2)*c4 + p4(3);
h43_mm= p43(1)*c4.^3+p43(2)*c4.^2+p43(3)*c4+p43(4);

error4= h4_mm - y4;
error43= h43_mm - y4;

plot(c4, y4, 'b;SN4;')
plot(c4, h4_mm, '--b')
plot(c4, h43_mm, ':b')







# basado en sensor 5 Cs 110pF ensayo 3
x5= [37.9037, 43.1314, 47.7578, 52.0205, 55.91, 59.2366, 62.0879, 64.7029];
y5= [150, 200, 250, 300, 350, 400, 450, 500];

orden= 2;
p5= polyfit(x5, y5, orden);
p53= polyfit(x5, y5, 3);

c5= x5;
h5_mm= p5(1)*c5.^2 + p5(2)*c5 + p5(3);
h53_mm= p53(1)*c5.^3+p53(2)*c5.^2+p53(3)*c5+p53(4);

error5= h5_mm - y5;
error53= h53_mm - y5;

#hold on
plot(c5, y5, 'k;SN5;')
plot(c5, h5_mm, '--k')
plot(c5, h53_mm, ':k')






# basado en sensor 6 Cs 110pF ensayo 1
x6= [38.508, 44.5503, 49.0478, 53.1451, 56.557, 59.8354, 62.6189, 65.4777];
y6= [150, 200, 250, 300, 350, 400, 450, 500];

orden= 2;
p6= polyfit(x6, y6, orden);
p63= polyfit(x6, y6, 3);

c6= x6;
h6_mm= p6(1)*c6.^2 + p6(2)*c6 + p6(3);
h63_mm= p63(1)*c6.^3+p63(2)*c6.^2+p63(3)*c6+p63(4);


error6= h6_mm - y6;
error63= h63_mm - y6;

plot(c6, y6, 'c;SN6;')
plot(c6, h6_mm, '--c')
plot(c6, h63_mm, ':c')









# curvas de error de usar una sola expresion para todas las sondas:
# tomando sensor 6 como referencia, ensayo 1

cap= 34:0.1:66;
yref= p6(1)*cap.^2 + p6(2)*cap + p6(3);
y_sensor4= p4(1)*cap.^2 + p4(2)*cap + p4(3);
y_sensor5= p5(1)*cap.^2 + p5(2)*cap + p5(3);
err_4= y_sensor4-yref;
err_5= y_sensor5-yref;



#hold off
#plot(cap, err_4, 'g')
#hold on
#plot(cap, err_5, 'r')
#axis([min(cap), max(cap), -1, 10])



p= p23;
clc
#s2= [num2str(p(1)), ", ", num2str(p(2)), ", ",num2str(p(3)), ";"];
s3=[num2str(p(1)), ", ", num2str(p(2)), ", ",num2str(p(3)), ", ", num2str(p(4)), ";"];

disp(s3)



#axis([37, 66, 145, 555])


#{

hold off
plot(x1, error1, '--r;SN1;')
hold on
plot(x1, error13, 'r')
plot(x3, error3, '--g;SN3;')
plot(x3, error33, 'g')
plot(x4, error4, '--b;SN4;')
plot(x4, error43, 'b')
plot(x5, error5, '--k;SN5;')
plot(x5, error53, 'k')
plot(x6, error6, '--c;SN6;')
plot(x6, error63, 'c')
axis([35, 65, -10, 10])
#}


#xlabel("capacidad [pF]");
#ylabel("nivel [mm]");


















