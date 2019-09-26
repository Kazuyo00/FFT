#include<stdio.h>
#include<stdlib.h>


int main()
{

        char filename[20]={};
        int data;

        printf("please input a filename  :  ");
        scanf("%s",filename);

        printf("please input a number of data  :  ");
        scanf("%d",&data);

        FILE *fp;
        fp=fopen(filename,"w");
        if(fp==NULL){
          printf("file open error\n");
        }

        int i,buf;

        for (i=0; i<data; i++) {
                buf=rand()%2;
                fprintf(fp,"%d\n",buf);
        }
        return 0;
}
