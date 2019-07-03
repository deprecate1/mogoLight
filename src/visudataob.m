#!/usr/bin/octave -fq

load nbdata
load data
data=vec(data);
dt=conv2(data,ones(15,1)/15,'valid')
p=dt/100;
e=100*sqrt((p.*(1-p))/(nbdata));
plot(dt,';success rate;',1:length(dt),dt-e,1:length(dt),dt+e);
print -deps ob.eps
