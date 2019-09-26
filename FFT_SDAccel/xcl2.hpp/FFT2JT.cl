#include <math.h>
#define N 2
#define M 3
#define M2 1

void FFT2JT(global double *Fr,
            global double *Fi,
            global double *Fr_results,
            global double *Fi_results)
{

  double P=6.283185307179584/N;
  double Cr,Ci,S1r,S1i,S3r,S3i,S5r,S5i,S7r,S7i;
  double Xr[N],Xi[N];
  char K,L,I,J,SIG;
  char IR,I1,JR,J1;
  char K1,K2,K3,K4,IP,JP,IM,KI,JM,KJ,KK;
  int i,j;

  //start FFT
  SIG=-1;
  Cr=cos(P);
  Ci=SIG*sin(P);
  Xr[0]=1;
  Xi[0]=0;

  for(I=0;I<N-1;I++){
	  Xr[I+1]=Xr[I]*Cr-Xi[I]*Ci;
	  Xi[I+1]=Xr[I]*Ci+Xi[I]*Cr;
  }

  for(L=0;L<M;L++){
	  K1=1<<(M-L);
	  K2=K1/2;
	  K3=N/K1;
	  K4=K3*2;
	  for(IP=0;IP<K2;IP++){
		  I1=IP*K4;
		  for(JP=0;JP<K2;JP++){
			  J1=JP*K4;
			  for(I=0;I<K3;I++){
				  IR=I+I1;
				  IM=IR+K3;
				  KI=I*K2;
				  for(J=0;J<K3;J++){
					  JR=J+J1;
					  JM=JR+K3;
					  KJ=J*K2;
					  KK=KI+KJ;
					  S1r=Fr[IR][JR];
					  S1i=Fi[IR][JR];
					  S3r=Fr[IR][JM]*Xr[KJ]-Fi[IR][JM]*Xi[KJ];
					  S3i=Fi[IR][JM]*Xr[KJ]+Fr[IR][JM]*Xi[KJ];
					  S5r=Fr[IM][JR]*Xr[KI]-Fi[IM][JR]*Xi[KI];
					  S5i=Fi[IM][JR]*Xr[KI]+Fr[IM][JR]*Xi[KI];
					  S7r=Fr[IM][JM]*Xr[KK]-Fi[IM][JM]*Xi[KK];
					  S7i=Fi[IM][JM]*Xr[KK]+Fr[IM][JM]*Xi[KK];
					  Fr[IR][JR]=S1r+S3r+S5r+S7r;
					  Fi[IR][JR]=S1i+S3i+S5i+S7i;
					  Fr[IR][JM]=S1r-S3r+S5r-S7r;
					  Fi[IR][JM]=S1i-S3i+S5i-S7i;
					  Fr[IM][JR]=S1r+S3r-S5r-S7r;
					  Fi[IM][JR]=S1i+S3i-S5i-S7i;
					  Fr[IM][JM]=S1r-S3r-S5r+S7r;
					  Fi[IM][JM]=S1i-S3i-S5i+S7i;
				  }
			  }
		  }
	  }
  }
  //end FFT

  for(i=0;i<N;i++){
    for(j=0;j<N;j++){

        Fr_results[i][j] = Fr[i][j];
        Fi_results[i][j] = Fi[i][j]

    }
  }


}
