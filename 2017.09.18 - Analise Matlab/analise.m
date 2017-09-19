pkg load signal;

NSAMPLES = 250000;

%x = dlmread ("Antena 7.5cm.csv", ",");
%x = dlmread ("Antena colada.csv", ",");
%x = dlmread ("KSA 7.5.csv", ",");
%x = dlmread ("idproxaskvelho 7.5.csv", ",");
%x = dlmread ("idproxaskvelho fw velho 20cm.csv", ",");
%x = dlmread ("idproxaskvelho fw novo 15cm.csv", ",");
%x = dlmread ("idproxaskvelho fw novo 15cm pino levantado.csv", ",");
%x = dlmread ("idproxaskvelho fw novo 0cm pino levantado.csv", ",");
%x = dlmread ("idproxaskvelho fw novo 15cm pino levantado 1000uf.csv", ",");
x = dlmread ("teste.csv", ",");

samples = x(2:NSAMPLES,2);
samples = samples - 0.5;
samplesn = 1:length(samples);

bitonezero=[ones(1,64),zeros(1,64)]-0.5;
bitzeroone=[zeros(1,64),ones(1,64)]-0.5;
sync=[bitum,bitum,bitum,bitum,bitum,bitum,bitum,bitum,bitum];

cardid=[ ...
	bitzeroone,bitzeroone,bitzeroone,bitzeroone,bitonezero, ...
	bitonezero,bitzeroone,bitonezero,bitzeroone,bitonezero, ...
	bitzeroone,bitzeroone,bitzeroone,bitzeroone,bitonezero, ...
	bitzeroone,bitonezero,bitonezero,bitzeroone,bitonezero, ...
	bitzeroone,bitonezero,bitonezero,bitzeroone,bitonezero];

%Correlation
[R,lag] = xcorr(samples,cardid);
subplot(3,1,1);
plot(lag(length(R)/2:end),R(length(R)/2:end));

%Find peaks
[pks,loc]=findpeaks(R(length(R)/2:end),"DoubleSided", ...
	"MinPeakHeight",280,"MinPeakDistance",150);

%create overlay
overlay = zeros(1,length(samples));
for i=1:length(loc)
	overlay(loc(i):loc(i)+length(cardid)-1)=cardid;
endfor
	
subplot(3,1,2);
plot(samplesn,samples,samplesn(loc),samples(loc),".m", ...
	samplesn, overlay);

subplot(3,1,3);
plot(cardid);