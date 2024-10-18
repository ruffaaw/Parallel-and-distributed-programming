#include <stdio.h>
#include "omp.h"

int main(){
    int i;
    int nr_watku = 4;
    int rozmiar_porcji = 3;
    int rozmiar = 150000;
    double start_time, end_time;
    int wynik;
    //a
    printf("a) static, rozmiar porcji=3 \n");
    omp_set_num_threads(nr_watku);
    start_time = omp_get_wtime();
    #pragma omp parallel for schedule(static, rozmiar_porcji)
    for(i = 0; i < rozmiar; i++) {
        wynik = i * 2;
    }
    end_time = omp_get_wtime();
    printf("Czas: %f\n\n", end_time-start_time);
    //b
    printf("b) static, rozmiar porcji domyslny \n");
    omp_set_num_threads(nr_watku);
    start_time = omp_get_wtime();
    #pragma omp parallel for schedule(static)
    for(i = 0; i < rozmiar; i++) {
        wynik = i * 2;
    }
    end_time = omp_get_wtime();
    printf("Czas: %f\n\n", end_time-start_time);
    //c
    printf("c) dynamic, rozmiar porcji=3 \n");
    omp_set_num_threads(nr_watku);
    start_time = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, rozmiar_porcji)
    for(i = 0; i < rozmiar; i++) {
        wynik = i * 2;
    }
    end_time = omp_get_wtime();
    printf("Czas: %f\n\n", end_time-start_time);
    //d
    printf("d) dynamic, rozmiar domyslny \n");
    omp_set_num_threads(nr_watku);
    start_time = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic)
    for(i = 0; i < rozmiar; i++) {
        wynik = i * 2;
    }
    end_time = omp_get_wtime();
    printf("Czas: %f\n\n", end_time-start_time);
}