#include <stdio.h>
#include "omp.h"

int main() {
    int n = 5000;
    int i;
    int suma = 0;
    int x = 2;

    #pragma omp parallel for
    for(i = 0; i < n; i++) {
        suma += x * x;
    }
    printf("\nSuma kwadratow bez redukcji: %d\n", suma);
}