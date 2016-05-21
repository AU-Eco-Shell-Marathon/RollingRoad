bit=12;
signal=5;

LSB=signal/(2^bit)

value=1/24

LSB*24*2

bitvalue=int16((LSB^-1)*5)




65536



bitshift(253,-7)

bitvalue=0;

x=

alfa=1;

y=0;
N=10000
y_out=zeros(1,N);
for i=[1:N]
y=bitshift((alfa)*bitvalue,8) + bitshift((256-alfa)*y,-8)+1;
y_out(i)=y;
end

plot(bitshift(y_out,-16))

y*10^-16

int32(256*bitvalue*2^-16)

2^16