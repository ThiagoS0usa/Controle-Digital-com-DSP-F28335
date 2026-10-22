R = 9910;
C = 88.2e-9;  % Paralelo dos Capacitores

Fc = (1)/(2*3.14*R*C);

Avdb = 20*log10((1)/(sqrt((1)^2+((10/Fc)^2))));

