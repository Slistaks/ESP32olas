



Cserie= 176;
C= [161, 276];  %variacion de 47cm

Ct(1)=(Cserie^-1+C(1)^-1)^-1;
Ct(2)=(Cserie^-1+C(2)^-1)^-1;


s= ["Cserie: ", num2str(Cserie)];
disp(s)

s= ["capacidad maxima: ", num2str(Ct(2))];
disp(s);

rango= Ct(2)-Ct(1);
cmPorCapdac= 30* 47/rango;

s= ["centimetros de variacion en un solo rango: ", num2str(cmPorCapdac)];
disp(s)