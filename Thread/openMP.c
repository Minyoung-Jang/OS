#include <stdio.h>
#include <omp.h>

int main(){

  omp_set_num_threads(4);

  #pragma omp parallel
	  printf("Hello\n");g

    return 0;
}

//gcc -fopenmp openMP.c -pthread