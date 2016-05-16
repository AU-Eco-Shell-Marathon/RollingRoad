%% Calculation of lowpass filter to all the sensors of Rolling Road.
clear;
close all;
s=tf('s');
%% Torque transducer
ADC_fs=48000;
ADC_N=12;
fc=444/60; % RPM/60 = 1/s. Max rpm for Rolling Road then testning AU2 444RPM

[R, C, fc_real, M]=ADC_lowpassFilter(ADC_fs, ADC_N, fc, [100*10^-12, 500*10^-9], 100*10^3, 66)

T_AAF_Torque=tf(((1/(R*C))^M)/((s+1/(R*C))^M))

figure;
bode(T_AAF_Torque);
legend('show');
grid on;

%% V_motor and A_motor

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

f2000_dB=2*20*log10(2000/20)

fc=20;

[R, C, fc_real, M]=ADC_lowpassFilter(ADC_fs, ADC_N, fc, [100*10^-12, 500*10^-9], 100*10^3)


T_AAF_power=tf(((1/(R*C))^M)/((s+1/(R*C))^M))

figure;
bode(T_AAF_power);
legend('show');
grid on;