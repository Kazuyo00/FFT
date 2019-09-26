#include <math.h>
#include <stdio.h>

#define N 2

int main()
{

double Fr[N][N], Fr_results[N][N], tmp = 0.0;
int i, j, p, q;

//input DATA
/*
for (i=0;i<N;i++){
  for (j=0;j<N;j++){
    if(i<3 && j<2) Fr[i][j]=1.0;
    if(i>=3 || j>=2) Fr[i][j]=0.0;
  }
}
*/

Fr[0][0] = 1.0;
Fr[0][1] = 2.0;
Fr[1][0] = 3.0;
Fr[1][1] = 4.0;

//calculation
for (i=0;i<N;i++){
  for (j=0;j<N;j++){
    for (p=0;p<N;p++){
      for (q=0;q<N;q++){
        tmp += Fr[p][q] * pow (-1,i*p+j*q) ;
      }
    }

    Fr_results[i][j] = tmp;
    tmp = 0;

  }
}

for (i=0; i<N; i++){
  for (j=0; j<N; j++){

    printf("Fr[%d][%d]=%lf\n",i,j,Fr_results[i][j]);

  }
}

}
