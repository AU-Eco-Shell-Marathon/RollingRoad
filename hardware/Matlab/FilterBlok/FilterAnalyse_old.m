%% Calculation of lowpass filter to all the sensors of Rolling Road.
clear
close all
s=tf('s');
% Torque transducer
ADC_fs=48000;
ADC_N=12;
fc=444/60; % RPM/60 = 1/s. Max rpm for Rolling Road then testning AU2 444RPM

[R, C, fc_real, M]=ADC_lowpassFilter(ADC_fs, ADC_N, fc, [100*10^-12, 500*10^-9], 100*10^3, 66)

T_AAF_Torque=tf(((1/(R*C))^M)/((s+1/(R*C))^M))

figure
bode(T_AAF_Torque);
legend('show')
grid on

% V_motor and A_motor

ADC_fs=52500;
ADC_N=12;

% The signal of interest is the avarge value, because of thate the PWM
% signal will be seen as noise signal, and must be filt out, also because
% els the sample rate have to be around 512 times bigger then PWM
% frequency. 

V_noise_PP = 5; 
% because is it a PWM signal.

Supress_dB=-20*log10(0.001)
% calculate how much the PW; signal have to be supress to meet the
% requarment of 0.1% accuracy.
 
% The design will be build based on that the PWM frequency is
% graeter then 2000Hz, and have to supress the PWM signal 60dB too only
% getting a 0.1% accuracy. This can be done with a 2. order lowpass filter
% 2 decades before 2000Hz this is 20Hz. The PWM signal will be supress with
% 80dB.

fc=20;

[R, C, fc_real, M]=ADC_lowpassFilter(ADC_fs, ADC_N, fc, [100*10^-12, 500*10^-9], 100*10^3)


T_AAF_effect=tf(((1/(R*C))^M)/((s+1/(R*C))^M))

figure
bode(T_AAF_effect)
legend('show')
grid on
%%


V_signal_PP=

SNR_min=10*log(V_signal_PP/V_stoej_max)

%% Belastnings kreds

ADC_fs=65000;
ADC_N=12;
fc=5; % RPM/60 = 1/s 444rpm=40

[R, C, fc_real, M]=ADC_lowpassFilter(ADC_fs, ADC_N, fc, [100*10^-12, 500*10^-9], 100*10^3)



%% Udregning af støj udfra målinger.
% load det data der skal analyseres
clear
clc
close all
filename = 'Støj målinger RR\Volt_måling\Copy_2016411122377.csv';

V_signal_PP=2.5;
V_stoej_max=0.1;

SNR_min=10*log(V_signal_PP/V_stoej_max)

ADC_fs=20000;
ADC_N=10;
ADC_SNR=6.02*ADC_N + 1.76
fc=444/60; % RPM/60 = 1/s 444rpm=40




s=tf('s');
T=tf(2*pi*fc/(s+2*pi*fc))

%%


[second,Volt] = importfile(filename);



% ik' ændre i noget efter her!




figure
plot(second-second(1), Volt)
grid on

% hurtig analyse af signalet.
V_signal_rms=V_signal_PP/sqrt(2)
V_avg=mean(Volt)
V_ACrms=std(Volt)
V_rms=rms(Volt)
V_var=var(Volt)
V_pp=max(Volt)-min(Volt)
Ts_data=second(2)-second(1);
SNR_dB_foer=10*log10(V_signal_rms/V_ACrms)
Volt_stoej=Volt-V_avg;

% FFT analyse af kun støj ingen DC .*hamming(length(Volt_stoej))  20*log10
X_data=fft(Volt_stoej,1000000);
figure
semilogx([0:round(length(X_data)/2)-1]*(((Ts_data)^-1)/length(X_data)),20*log10(abs(X_data(1:end/2))))
grid on
% FFT A=2*M/N /max(abs(X_data(1:end/2)))

[b,a]=butter(3,0.1, 'LOW');
a=0.1
b=[-a];
a=[-1,1-a];


x_filter=filter(b,a,Volt_stoej);

plot(x_filter)

std(x_filter)
var(x_filter)
10*log10(V_signal_rms/rms(x_filter))




