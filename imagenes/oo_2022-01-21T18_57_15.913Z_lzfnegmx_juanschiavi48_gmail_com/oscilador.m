


t= 0:0.1:10;
tau= 1;

vcc= 18;
vh= 8;
vl= 4;


Ccarga= vcc*( 1-exp(-t/(tau)) );
th= -tau*log(1-vh/vcc);
tl= -tau*log(1-vl/vcc);

Tcreciendo= tau*log( (vcc-vl)/(vcc-vh) );




Cdescarga= vcc*exp(-t/(tau));
tldescarga= -tau*log(vl/vcc);
thdescarga= -tau*log(vh/vcc);

Tdecreciendo= tau*log(vh/vl);



hold off
plot(t, Ccarga, 'b')
hold on
plot(t, Cdescarga, 'r')

plot([0,10], [vh, vh], 'k')
plot([0,10], [vl, vl], 'k')

plot([tl,tl], [0,9], 'k')
plot([th,th], [0,9], 'k')

plot([tldescarga,tldescarga], [0,vcc], 'g')
plot([thdescarga,thdescarga], [0,vcc], 'g')


disp("Tcreciendo=");
disp(Tcreciendo);
disp("Tdecreciendo=");
disp(Tdecreciendo);

