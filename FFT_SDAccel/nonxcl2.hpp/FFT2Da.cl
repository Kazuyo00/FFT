#define N 8 //処理画素数
#define M 3
#define M2 1

kernel __attribute__((reqd_work_group_size(1, 1, 1)))
__kernel
void FFT2Da(global double (*r_Fr)[N],
            global double (*r_Fi)[N],
            global double (*r_Fr_results)[N],
            global double (*r_Fi_results)[N])
{

  double P=6.283185307179584/N;
  double Cr,Ci,S1r,S1i,S3r,S3i,S5r,S5i,S7r,S7i;

  double q_Fr[N][N];
  double q_Fi[N][N];
  double Xr[N];
  double Xi[N];

  double SIG;
  char IR,JR,IM,JM,KI,KJ,KK;
  int I1,J1,K1,K2,K3,K4;
  int I,J,IP,JP,L;
  int i,j;

   for(i=0;i<N;i++){
     for(j=0;j<N;j++){
         q_Fr[i][j] = r_Fr[i][j];
         q_Fi[i][j] = r_Fi[i][j];
     }
   }

  //start FFT
  SIG=-1;
  Cr=cos(P);
  Ci=SIG*sin(P);
  Xr[0]=1;
  Xi[0]=0;


  //回転因子
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
					  S1r=q_Fr[IR][JR];
					  S1i=q_Fi[IR][JR];
					  S3r=q_Fr[IR][JM]*Xr[KJ]-q_Fi[IR][JM]*Xi[KJ];
					  S3i=q_Fi[IR][JM]*Xr[KJ]+q_Fr[IR][JM]*Xi[KJ];
					  S5r=q_Fr[IM][JR]*Xr[KI]-q_Fi[IM][JR]*Xi[KI];
					  S5i=q_Fi[IM][JR]*Xr[KI]+q_Fr[IM][JR]*Xi[KI];
					  S7r=q_Fr[IM][JM]*Xr[KK]-q_Fi[IM][JM]*Xi[KK];
					  S7i=q_Fi[IM][JM]*Xr[KK]+q_Fr[IM][JM]*Xi[KK];
					  q_Fr[IR][JR]=S1r+S3r+S5r+S7r;
					  q_Fi[IR][JR]=S1i+S3i+S5i+S7i;
					  q_Fr[IR][JM]=S1r-S3r+S5r-S7r;
					  q_Fi[IR][JM]=S1i-S3i+S5i-S7i;
					  q_Fr[IM][JR]=S1r+S3r-S5r-S7r;
					  q_Fi[IM][JR]=S1i+S3i-S5i-S7i;
					  q_Fr[IM][JM]=S1r-S3r-S5r+S7r;
					  q_Fi[IM][JM]=S1i-S3i-S5i+S7i;
				  }
			  }
		  }
	  }
  }
  //end FFT

  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        r_Fr_results[i][j] = q_Fr[i][j];
        r_Fi_results[i][j] = q_Fi[i][j];

    }
  }


}

