#include <stdio.h>
#include "include/pomiar_czasu.h"

void wykonaj_operacje() {
    int i;
    int suma = 0;
    inicjuj_czas();
    for (i = 0; i < 1000; i++) {
        suma += i;
    }
    drukuj_czas();
    printf("Wynik operacji arytmetycznej: %d\n", suma);

    inicjuj_czas();
    for (i = 0; i < 1000; i++ ) {
        printf("%d", i);
    }
    drukuj_czas();
}

int main() {
    wykonaj_operacje();
    return 0;
}