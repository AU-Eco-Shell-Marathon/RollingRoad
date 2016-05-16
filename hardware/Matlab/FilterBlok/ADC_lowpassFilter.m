function [ R, C, fc_real, M ] = ADC_lowpassFilter( ADC_fs, ADC_N, fc, C_limit, R_MAX, ADC_SNRdB)
%ADC_LOWPASSFILTER Summary of this function goes here
%   Detailed explanation goes here
%% Initialize variables.
delimiter = ',';
if nargin==5
    ADC_SNRdB=6.02*ADC_N + 1.76
end
%% udregner hvor man filter der skal laves.

M=ceil(log((10 ^ (ADC_SNRdB / 0.20e2))) / log((ADC_fs/2) / fc));


%% Laver E12 række til Kondensator værdier.
P =[100;120;150;180;220;270;330;390;470;560;680;820];

C_list(1)=round60063(C_limit(1),'E12');%http://www.mathworks.com/matlabcentral/fileexchange/48840-round-to-electronic-component-values
prefix=floor(log10(C_list(1)));
Pi=0;
for i=1:length(P) 
    if(round(C_list(1)*10^-(prefix-2)) == P(i))
        Pi=i;
       break; 
    end
end
i=2;
while (C_list(end)<=C_limit(2))
    Pi=Pi+1;
    if(Pi > length(P))
       Pi=1;
       prefix=prefix+1;
    end
    C_list(i) = P(Pi)*10^(prefix-2);
    i=i+1;
end

%% finder den bedste modstands værdi udfra E12
R_best=0;
C_best=0;

fc_real=-1;
for i=1:length(C_list)
    C_temp=round60063(C_list(i),'E12');
    R_calc=1/(2*C_temp*pi*fc);
    R_temp=round60063(R_calc,'E12');
    if(R_temp <= R_MAX)
        if(log((10 ^ (ADC_SNRdB / 0.20e2))) / log((ADC_fs/2) / (1/(2*pi*C_temp*R_temp))) <= M) % tjekker om kravet stadigvæk er opfyldt
           if (abs((1/(2*pi*C_temp*R_temp))-fc) < abs(fc_real-fc)) % tjekker om dette filter er mere præcist.
               fc_real=1/(2*pi*C_temp*R_temp);
               R_best=R_temp;
               C_best=C_temp;
           end
        end
    end
end

R=R_best;
C=C_best;

end

