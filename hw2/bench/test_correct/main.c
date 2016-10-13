#include "stdio.h"

int main() 
{
  int n = 10;
  int x, y = 3, z = 7;
  int a[n];

	for (int i = 0; i < n; i++) {
		x = y + z;
		a[i] = 6 * i + x * x;
	}

  printf ("%d, %d, %d\n", a[0], a[1], a[2]);

  return 0 ;
}
