#include <math.h>
#include <stdio.h>

#define N 8
#define M 3
#define M2 1

float Fr[N][N],Fi[N][N]/*,Fr_results[N][N],Fi_results[N][N]*/;

int main(){
	float Tr,Ti;
	char i,j,K,L,I,J;
	char I1,I2,I3,I4,JR,J1,J2;
	char KF,K1,K2;

	//input DATA
	for (i=0;i<N;i++){
		for (j=0;j<N;j++){
			if(i<3 && j<2) Fr[i][j]=1.0;Fi[i][j]=0.0;
			if(i>=3 || j>=2) Fr[i][j]=0.0;Fi[i][j]=0.0;
		}
	}

	//start bit reverse
	for (L=0;L<M2;L++){
		I1=1<<L;
		I2=N/I1;
		I3=I2/2;
		I4=I3/2;

		for(I=0;I<I1;I++){
			JR=I*I2;
			J1=JR+I1;
			J2=JR+I3;

			for (J=0;J<I4;J++){
				KF=(J/I1)*I1+J;
				K1=KF+J1;
				K2=KF+J2;

				for (K=0;K<N;K++){
					 Tr=Fr[K][K1];
					 Ti=Fi[K][K1];
					 Fr[K][K1]=Fr[K][K2];
					 Fi[K][K1]=Fi[K][K2];
					 Fr[K][K2]=Tr;
					 Fi[K][K2]=Ti;
				}
				for (K=0;K<N;K++){
						Tr=Fr[K1][K];
						Ti=Fi[K1][K];
						Fr[K1][K]=Fr[K2][K];
						Fi[K1][K]=Fi[K2][K];
						Fr[K2][K]=Tr;
						Fi[K2][K]=Ti;
				}
			}
		}
	}
	//end bit reverse

	FFT2JT(Fr,Fi);

	for (i=0; i<N; i++){
  	for (j=0; j<N; j++){

      printf("Fr[%d][%d]=%f,Fi[%d][%d]=%f\n",i,j,Fr/*_results*/[i][j],i,j,Fi/*_results*/[i][j]);

  	}
	}

}
