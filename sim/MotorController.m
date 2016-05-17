%Program til indledende estimering af den n�dvendige effekt til
%acceleration af k�ret�jet fra stilstand til maksimalhastighed.
% Alle variable er angivet i SI-enheder hvis ikke andet er angivet.
clc
clear all
close all

g=9.81; % Tyngdeacceleration
rho_l=1.204; % Densitet af luft
theta=atan(0/100)*(pi/180); % H�ldning p� underlag(rad)
v_w=0; % Vindhastighed, positiv ved modvind og vice versa

m_v=35; % Masse af k�ret�j
m_d=50; % Masse af f�rer
m_w=0.8; % Masse af hjul
m_t=m_v+m_d; % Samlet masse af k�ret�j inkl. f�rer
n_w=3; % Hjulantal
d_w=0.478; % Diameter af hjul
r_w=d_w/2; % Radius af hjul
d_l=0.02; % Diameter i lejer
mu_l=0.0015; % Friktionskoefficent for lejer
C_RR=0.002; % Rullemodstandskoefficient
C_A=0.102; % Aerodynamisk modstandskoefficent
A_F=0.2718; % Frontareal

FT=37.51; % Tr�kkraft

v0=eps/3.6; % Begyndelseshastighed
t0=0; % Begyndelsestidspunkt
td=20; % Accelerationstid
vd=30/3.6; % �nskede hastighed
simtime=td; % Simuleringstid


