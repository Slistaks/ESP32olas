
%{

Cmax= 150;   % cambiar
Cmin= 35;
puntos= 100;
Cnivel=linspace(Cmin, Cmax, puntos);
x= 1:1:puntos;

Cserie= 220;

Ctot= 1./(1./Cserie+1./Cnivel);

plot(x, Ctot);
%}




%46.7(eq) (59.3 sin Cserie) a 87.4(eq) (145.0) ->variacion de cap para 35cm de rango

Cmin= 59.3;   % cambiar
Cmax= 145;
puntos= 100;
Cnivel=linspace(Cmin, Cmax, puntos);
x= 1:1:puntos;

Cserie= 120;    % 150 al limite (rango=31.23)
                % 120           (rango=26   )

Ctot= 1./(1./Cserie+1./Cnivel);

rango= max(Ctot)-min(Ctot);
disp("rango:");
disp(rango);


disp("")

p= polyfit(Cnivel, Ctot, 1);

y= polyval(p, Cnivel);

hold off;
plot(Cnivel, y-1.5);



grid minor;
grid on;



hold on;


plot(Cnivel, Ctot);











