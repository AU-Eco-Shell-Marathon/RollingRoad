%% Constants
clear
close all
clc

s=tf('s');

L_a      = 0.000072;        %DC-generator inductance (H)
R_a      = 0.103;           %Anchor resistance (Ohm)
R_load   = 1.1;             %Load resistance (Ohm)
Kt       = 38.5*10^(-3);    %Torque Constant (Nm/A)
Gear     = 4.3;             %Gearing ratio in the motor
RR_r     = 0.076            %RollingRoad radius (m)
Kb       = Kt;              %Speed constant (rpm/Nm)
M_t      = 0.00378          %Motor time constant. (s)
K_pwm    = 256;             %PWM steps 
Moment_fc= 178.6251         %Torque sensor filter cut frequency (rad/s)

% Convert 
OmegaToRpm = 60/(2*pi);
SpeedToOmega = (2*pi)/(2*pi*RR_r);
SpeedToRpm = SpeedToOmega * OmegaToRpm;
Km_hTom_s = 1000/3600;
KmToRpm=Km_hTom_s*SpeedToRpm

% Test speed
Speed = 30*Km_hTom_s%m/s

V_rpm=Speed*SpeedToRpm %rpm
%%% transfer functions

% transfer function of the generatoren.
T_generator=tf((1/M_t)/(s+(1/M_t))) % motor delay

% transfer function for Moment/rpm.
Ea=tf(T_generator*Gear*Kb*Kt/(R_a+R_load)) % Moment/rpm    //

% at a steady speed.
TT=Ea*V_rpm % if speed is not constant have to add an extra T_generator

% note because the speed affects proportional on the system. there be
% something there adjusted receptional of the speeds proportional affect.
% This can be done with a function of speed

syms v_rpm

P_rpm=1/(v_rpm*Gear*Kb*Kt/(R_a+R_load))
% must be implemented into the code.

func_P=(V_rpm*Gear*Kb*Kt/(R_a+R_load))^-1

Ggenerator=tf(func_P*TT)

% PWM signal transfer function 

G_PWM=(1/K_pwm)

% Torque sensor filter

G_moment_filter=(Moment_fc*2*pi) / (s + Moment_fc*2*pi)

% total transfer function of the plant.

Gplant = G_PWM*Ggenerator

% Open loop bode
Gplant*G_moment_filter
figure
bode(Gplant*G_moment_filter)
grid on
%legend('show')

%% Calculation of PID constanst
% requirements
 
Ts=0.1;

OS=5; 

% Calculation

Zeta = (-log( OS / 100 ))/(( pi^2 + log( OS / 100 )^2)^0.5)

Phase_m = atan( ( 2 * Zeta ) / (( -2 * Zeta^2 + ( 1 + 4 * Zeta^4)^0.5 )^0.5))*(180/pi) + 7

-(180-Phase_m)

Omega_BW_min = ( 4 / ( Zeta * Ts )) * sqrt(( 1 - 2 * Zeta^2 ) + sqrt( 4 * Zeta^4 ) - 4 * Zeta^2 + 2 ) 

if (OS == 5)
    Omega_Phase_m =  816;
elseif (OS == -10)
    Omega_Phase_m =  113;
elseif (OS == -20)
    Omega_Phase_m =  134;
elseif (OS == -30)
    Omega_Phase_m =  155;
end


% P led udregning
if (OS == 5)
    P=10^( 60.3 / 20 )
elseif (OS == -10)
    P=10^( 58 / 20 )
elseif (OS == -20)
    P=10^( 59.9 / 20 )
elseif (OS == -30)
    P=10^( 61.6 / 20 )
end

figure
bode(P*Gplant*G_moment_filter)
grid on

figure
step(feedback(P*Gplant,G_moment_filter))
grid on

% I led udregning

T = ( Omega_Phase_m / 10 )^-1

%I = tf(( s + T^-1 ) / s )

I = tf(( 1 ) / ( T * s ) )


figure
subplot(2,2,[1,2]);
bode(P*(I+1)*Gplant*G_moment_filter)
grid on
subplot(223);
step(feedback(P*(I+1)*Gplant, G_moment_filter))
title('Step load system')
grid on
subplot(224);
step(feedback(P*(I+1), Gplant*G_moment_filter)/K_pwm) % PID value are ignoring PWM 
title('Step PID values (MAX 1)')
grid on

% D led udregning

% behøves ikke da båndbredden allerede er 816 rad/s og den ønsket var 60 rad/s så den er langt
% over. Så kravet er overholdt.

%% Udregning af sample tid for PID regulatoren

Ts_max=0.5/Omega_Phase_m
Ts_min=0.15/Omega_Phase_m

Fs_max=Ts_min^-1
Fs_min=Ts_max^-1

T_PSoC_max=c2d(P*(I+1)*Gplant,Ts_min,'tustin')

T_PSoC_min=c2d(P*(I+1)*Gplant,Ts_max,'tustin')



T_PSoC_min_feedback=feedback(T_PSoC_min,c2d(G_moment_filter,Ts_max,'tustin'));
T_PSoC_max_feedback=feedback(T_PSoC_max,c2d(G_moment_filter,Ts_min,'tustin'));

% bode plot af openloop
figure
subplot(121)
bode(P*(I+1)*Gplant*G_moment_filter)
hold on
bode(c2d(P*(I+1)*Gplant*G_moment_filter,Ts_max,'tustin'));
hold on
bode(c2d(P*(I+1)*Gplant*G_moment_filter,Ts_min,'tustin'));
grid on

subplot(122)
step(feedback(P*(I+1)*Gplant, G_moment_filter))
hold on
step(T_PSoC_min_feedback)
step(T_PSoC_max_feedback)
grid on

%% PID konstanter
if 0
    KP=5.66
    KI=137
    KD=0;
    Gpid=pid(KP,KI, KD, tf(Gplant), 1/f_PID)
else
    load('PID_data', 'C');
    Gpid=C
    clear 'C'
end

%%% Delays - PWM signal og PID(inkluderet sampletiden) 



%% Krav godkendt?

% Hastighed 18km/h max Kraft = 41.00N, min = 19.25N

N_gen_18_max=Ea*18*(1000/3600)*Gear*(1/RR_r)
E_mek_max=41*18*(1000/3600)
E_mek_gen_max=N_gen_18_max*18*(1000/3600)
E_mek_min=19.25*18*(1000/3600)
%krav ikke godkendt til simulation run, kun konstant hastighed.

% Hastighed 24.8km/h max Kraft = 49.37N, min = 21.76N

N_gen_18_max=Ea*24.8*(1000/3600)*Gear*(1/RR_r)
E_mek_max=49.37*24.8*(1000/3600)
E_mek_gen_max=N_gen_18_max*24.8*(1000/3600)
E_mek_min=21.76*24.8*(1000/3600)
%krav ikke godkendt til simulation run, kun konstant hastighed. OBS der
%sker overbelastning.

% Hastighed 30km/h max Kraft = 65.27N, min = 22.59N

N_gen_18_max=Ea*30*(1000/3600)*Gear*(1/RR_r)
E_mek_max=65.27*30*(1000/3600)
E_mek_gen_max=N_gen_18_max*30*(1000/3600)
E_mek_min=22.59*30*(1000/3600)
%krav ikke godkendt til simulation run, kun konstant hastighed. OBS der
%sker overbelastning.

MAX_RPM=30*(1000/3600)*(1/RR_r)*(60/(2*pi))
MAX_T=65.27*RR_r


% kan udfra analysen konkludere at RollingRoad kun kan klare at udføre test
% på konstante hastigheder, og derfor kan kun bruges til at tjekke
% nyttevirkning. Dette vil sige at der kan ikke laves test kørrelse på den.
% Der skal findes en generator der kan tåle 550watt. Før simulation run kan
% udføres, selvfølgelig er dette også lidt mistænkligt, da motoren kun
% skulle kunne klare 200watt, men det skyldes at den skal helt der op i et
% lille tidsrum.


%% PID indstilling

%Gplant=Gdelay*(1/100)*Gdelay_2*Ea*24.8*(1000/3600)

G_system=feedback(Gpid*Gplant, 1)

figure(2);
step(G_system);
grid on;
hold on;
figure(3);
bode(G_system)
grid on
hold on

%%

pidTuner(G_system)
pause
G_system_new=feedback(C*Gplant, 1)


figure(2);
step(G_system_new);
grid on;
hold on;
figure(3);
bode(G_system_new)
grid on
hold on

%%
save('PID_data', 'C');

%% Udregning af filter koeficenter til Moment transducer
% FIR eksampel
clear
close all
clc

fs=4000;
fc=20; % den ændre sig ikke hurtigt, men vil gerne undgå støj kilder så som
%        50Hz forsyning, derfor vælger vi at gå efter en knæk frekvens 
%        under 50Hz.


N=250

round(N*fc/fs)

H=[ones(1,round(N*fc/fs)), zeros(1, N - 2*round(N*fc/fs)), ones(1,round(N*fc/fs) - 1)]


h=fftshift(ifft(H)).*blackman(length(H))'

figure
freqz(h)
figure
impz(h)


x=randn(1,10000)*1 + 10;

y=conv(h, x);

figure
plot([0:10000-1]*fs^-1,x)
hold on
plot([0:10000-1]*fs^-1,y(1:10000))




% det kan godt virke, dog er filter orden ret stor og vil kræve en masse
% kraft og tid at udregne.

%IIR eksample
clear
close all
clc

fs=4000;
fc=5;

fc/(fs/2)


[b, a]=butter(2, fc/(fs/2), 'LOW')

freqz(b,a)

x=randn(1,10000)*1 + 10*sin(2*pi*0.1*[0:10000-1]*fs^-1);

y=filter(b, a, x)

plot([0:10000-1]*fs^-1,x)
hold on
plot([0:10000-1]*fs^-1,y)

(fc*2*pi)^-1











