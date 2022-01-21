
tm=0.1;
t=1:tm:100;
fm=1/tm;
frec=1;
y=sin(2*pi*frec*t);

plot(t, y)

Y=fft(y, length(y));
Y=Y(1:length(Y)/2);
Y=abs(Y);
f=fm*(0:length(Y)-1)/1000;
plot(f,Y)