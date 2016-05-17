%% Konstanter
clear
close all
clc

s=tf('s');


L_a      = 0.000072;        %DC-generatorens selvinduktion
R_a      = 0.103;           %Ankermodstand
R_load   = 1.1;             %Loadmodstand
% skal udregnes f_PID     = 100;            %Samplefrekvensen
N        = 1;               %Antal samples der midles over
Kt       = 38.5*10^(-3);    %Momentkonstant
Gear     = 4.3;             %Gearingsforhold i motor
K_mosfet = 18.16/5;         %Gain i MOSFET
K        = 5;               %Gain i Controller
Ti       = 0.016;           %Tiden for Intergral led i Controller
RR_r=0.076                  %RollingRoad radius
Kb=Kt;
M_t     =0.00378            %Motor tidskonstant.

T_PID=f_PID^-1+M_t
%% Hastighed

V=24.8;

T_generator=tf((1/M_t)/(s+(1/M_t)))

%((1/M_t)/(s+(1/M_t)))
%%% transfere funktion
Ea=tf(T_generator^2*(1/RR_r)*Gear*Kb*Kt/(R_a+R_load)) % Moment/rpm    //
bode(Ea)
%Gdelay=tf(exp(-s*1/f_PID))
%Gdelay_2=tf(exp(-s*1/f_PID))

TT=Ea*V

% note fordi hastigheden p�virker proptional p� plant, bliver der n�d til
% at korigeres for det, derfor skal P ledet i PID regulatoren v�re en
% funktion af hastigheden! 

syms v

syms p(v)

P_rpm=1/(v*(1/RR_r)*Gear*Kb*Kt/(R_a+R_load))

plot(P_rpm)

func_P=(V*(1/RR_r)*Gear*Kb*Kt/(R_a+R_load))^-1





Gplant=tf(func_P*TT)

bode(Gplant)



% Open loop bode

figure
bode(Gplant)
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


% kan udfra analysen konkludere at RollingRoad kun kan klare at udf�re test
% p� konstante hastigheder, og derfor kan kun bruges til at tjekke
% nyttevirkning. Dette vil sige at der kan ikke laves test k�rrelse p� den.
% Der skal findes en generator der kan t�le 550watt. F�r simulation run kan
% udf�res, selvf�lgelig er dette ogs� lidt mist�nkligt, da motoren kun
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
fc=20; % den �ndre sig ikke hurtigt, men vil gerne undg� st�j kilder s� som
%        50Hz forsyning, derfor v�lger vi at g� efter en kn�k frekvens 
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




% det kan godt virke, dog er filter orden ret stor og vil kr�ve en masse
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











