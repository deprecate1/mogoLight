#!/usr/bin/octave -fq

load data

if (data(length(data))<10)
	proba=data(length(data));
	data=data(1:(length(data)-1));
else
	proba=1;
end

for i=1:length(data),
	x(i)=floor(data(i)/10);
	y(i)=mod(data(i),10);
end
%printf(" int lastMovesSize=lastMoves.size(); int arch1,arch2; if (lastMovesSize>0) {arch1=lastMoves[0];} if (lastMovesSize>1) {arch2=lastMoves[1];}\n");
for k=1:13,

	if (mod(k,2)==1) x2=10-x; else x2=x; end
	if (mod(ceil(k/2),2)==1) y2=10-y; else y2=y; end
	if (mod(ceil(k/4),2)==1) z=y2; y2=x2; x2=z; end
	%subplot(2,4,k);
	%plot(x2,y2);
	%axis([2,8,2,8]);
	data=x2*11+y2;

	debut=mod(length(x2),2); if (debut==0) debut=2; end;
	for i=debut:2:min(12,length(x2)),
		printf(" if ((lastMovesSize==%d)",i);
		for j=1:(i-1),
			if (j==1)
				printf("&&(arch1==%d)",data(1));
			else
				if (j==2)
					printf("&&(arch2==%d)",data(2));
				else
					printf("&&(lastMoves[%d]==%d)",j,data(j));
				end
			end
		end
		probab=1-(1-proba)^(1/8);
		printf(") {if (drand48()<%g) return %d;}\n",probab,data(i));
		printf(" else ");
		%(1-p')^8=1-p
		% p' = 1-(1-p)^(1/8)

	end
	printf("{}\n");
end
%printf("}\n");



