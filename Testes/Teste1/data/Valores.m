R = 9910;
C = 88.2e-9;  % Paralelo dos Capacitores

Fc = (1)/(2*3.14*R*C);

Avdb = 20*log10((1)/(sqrt((1)^2+((10/Fc)^2))));

% p/ o grafico de bode
pkg load control;

% H(s) = 1/(RCs + 1)
H = tf([1], [R*C, 1]);

bode(H);
