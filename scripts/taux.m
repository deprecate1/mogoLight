#!/usr/bin/octave -fq

load n1_taux
load n2_taux

a=n1_taux;
b=n1_taux+n2_taux;

p=a/b;
disp(p);

if ((a>0)&&(a<b))

s=sqrt(p*(1-p))/sqrt(b);
if (1==0)
disp(sprintf(" %g +- %g\n",p,s));
disp(sprintf(" %g +- %g (2sigma)\n",p,s*2));
disp(sprintf(" [%g , %g]\n",p-s*2,p+s*2));
end

z=rand(b,10000);
for u=0:0.01:1,
	% V= distrib du taux de victoire si esp = u
	V=mean(z>1-u); %%% V decroit
	% sc = proba pour qu'on obtienne un truc si beau avec esp=u
	sc=mean(V>p); %%% sc decroit
	scs=mean(V>=p);
	if (sc<=0.025) %%% c'est rare d'obtenir une pareille stat avec un si petit u; bi>=u
		bi=u;
	end
	if (scs<=0.975) %%% ce u conduit parfois a une meilleur stat; bs>u
		bs=u;
	end

end
V=mean(rand(b,50000)>0.5);
disp(sprintf('Confidence interval (0.05): [%g,%g]',bi,bs));
pv=(mean(V>=p));
disp(sprintf('p-value of > 1/2: %g',pv));
if (pv<0.05) disp('significant!!!');end;
pv=(mean(V>=1-p));
disp(sprintf('p-value of < 1/2: %g',pv));
if (pv<0.05) disp('significant!!!');end;

end

