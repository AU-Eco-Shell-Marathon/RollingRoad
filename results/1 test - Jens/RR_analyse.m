%% Rolling Road
data=load('maale_data_drivelinje_JENS_RR.txt');
N=length(data)

col=size(data,2);

plots=[ 3   4   5   5   6   7   8   9   10  12];
navn=[];  

figure
for n=plots
    plot(data(:,2)/1000,data(:,n))
    hold on
end


clear Force



F_tol=0.5;
Speed_tol=0.2;
count = 0;
for n=1:N
    setForce=data(n,6);
    if(count == 0)
        Force(count+1)=setForce;
        count=count+1;
    else
        [~, p]=max(Force==setForce);
        if(max(Force==setForce)~=1)
            Force(count+1)=setForce;
            count=count+1;
        end
    end
end

Force=sort(Force);

Force=[1 2 5 7.5 10 12.5 15];

clear sample

Speeds=linspace(0, 6, 6);

count=zeros(length(Speeds),length(Force));
sample=zeros(length(Speeds),length(Force));

for n=1:N
    
    if ((data(n,6)-F_tol) < data(n,7) && (data(n,7) < (data(n,6)+F_tol)))
        if(0 <= data(n,5) && data(n,5) <= 100)
            
            [d, p_s]=min(abs(Speeds-data(n,12)));

            [~, p_F]=max(Force==data(n,6));
            sample(p_s, p_F) = sample(p_s, p_F) + data(n,5);
            count(p_s, p_F) = count(p_s, p_F) + 1;

        end
    end
    
end

for s=1:length(Speeds)
    for f=1:length(Force)
        if(count(s,f) ~= 0)
            sample(s,f)=sample(s,f)/count(s,f);
        else
            sample(s,f)=nan;
        end
    end
end

%medfilt2(sample)

figure
contourf(Speeds,Force,sample',10)
figure
meshc(Speeds,Force,sample')
