// test 1:  no conflict, no uses of LOAD
#include "stdio.h"

int ARR[100];
int b[100];

int main()
{
    int x = 0,i;
    int *ptra = ARR;

    for (i=0;i<100;i++){
      ARR[i] = i;
    }

    for (i=0;i<100;i++){
      if (i > 999){ //   case 1: don't need reload 
        ARR[i+1] = 1;
      }
      b[i] = ARR[97];
    }

    printf ("%d, %d, %d\n",b[97],b[98],b[99]);
    return 0;
}
