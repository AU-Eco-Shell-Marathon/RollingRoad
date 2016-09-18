%%% Konstanter
L_a      = 0.000072;        %DC-generatorens selvinduktion
R_a      = 0.103;           %Ankermodstand
R_load   = 1.1;             %Loadmodstand
f_sw     = 20000;           %Samplefrekvensen
N        = 1;               %Antal samples der midles over
Kt       = 38.5*10^(-3);    %Momentkonstant
Gear     = 4.3;             %Gearingsforhold i motor
K_mosfet = 18.16/5;         %Gain i MOSFET
K        = 5;               %Gain i Controller
Ti       = 0.016;          %Tiden for Intergral led i Controller

%Definition af s
s = tf('s');      

% Overf�ringsfunktion for forst�rkningen
Gp = K;        %Proportional gain

% Overf�ringsfunktion for Integral led (Fjerner station�r fejl)
%Gi = tf((s+(1/Ti))/(s));
Gi = tf(((1/Ti))/(s))

% Overf�ringsfunktion for belastningskredsl�bet
G_belastning = tf((1/L_a)/(s + ((R_a+R_load)/L_a))); 

% Overf�ringsfunktion for tidsforsinkelsen som f�lge af sampling
T = 1/(f_sw / N);               %Sampletiden
G_forsinkelse = tf(exp((-T/2)*s));  %Overf�ringsfunktionen for tidsforsinkelsen

%Den samlede overf�ringsfunktion for systemet
G_system = Gp*Gi*G_belastning*K_mosfet*Kt*Gear*G_forsinkelse

%Overf�ringsfunktion for regulator (Bruges til bodeplot)
G_PI = Gp*Gi;

%Overf�ringsfunktion for systemet uden Regulator (Bruges til bodeplot)
G_ureguleret = G_belastning*K_mosfet*Kt*Gear*G_forsinkelse

% Bodeplot for open loop overf�ringsfunktionen
figure(1) % Amplitude- og fasekarakteristik 
bode(G_system)
hold on
bode(G_PI)
hold on
bode(G_ureguleret)
legend('show')
grid on

% Steprespons for lukketsl�jfen
T1       = feedback(G_system,1) 
T2      = feedback(G_ureguleret,1)
figure(2) 
step(T1)
hold on
step(T2)
legend('show')
grid on

%Plot at det uregulerede system
figure(3)
step(T2)
grid on

figure(4)
bode(G_ureguleret)
legend('show')
grid on

%Plot at det regulerede system
% Steprespons for lukketsl�jfen
T1       = feedback(G_system,1) 
figure(5) 
step(T1)
grid on