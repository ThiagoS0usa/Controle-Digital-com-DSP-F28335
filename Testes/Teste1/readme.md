Pino 29 - Tx    (Conexão em X)<br>
Pino 28 - Rx 

ADCRESULT0 (Vout) - A01<br>
EPWM1 - Pino 0

Esquemática
```
                |-C1-|
EPWM --- R1 ----|-C2-|---GND
             |  |-C3-|
             |
            ADC
```

Teste 1 - Resistor 9.91k e capacitores em paralelo 28.5nF 29.9nF 29.8nF

Cálculo Fc:<br>
OCTAVE: 182.18 Hz<br>
SFRA: =~ 189 Hz 

Ganho em 10 Hz:<br>
OCTAVE: -0.01306 dB<br>
SFRA: 0.67 dB

<img src="https://github.com/ThiagoS0usa/Controle-Digital-com-DSP-F28335/blob/325b6caf3d497b2269c00f719365ab939a73370a/Testes/Teste1/img/SFRA.png" alt="Gráfico gerado no SFRA" width="2000">

<img src="https://github.com/ThiagoS0usa/Controle-Digital-com-DSP-F28335/blob/325b6caf3d497b2269c00f719365ab939a73370a/Testes/Teste1/img/octave.png" alt="Gráfico gerado no OCTAVE" width="2000">




