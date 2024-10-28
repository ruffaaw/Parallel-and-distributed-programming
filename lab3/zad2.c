#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Funkcja generująca losową wartość zmiennoprzecinkową
float randomFloat() {
    return (float)rand() / RAND_MAX * 10.0f;
}

// Funkcja generująca macierz o podanych wymiarach, wypełnioną losowymi liczbami
void generateMatrix(float** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = randomFloat();
        }
    }
}

// Funkcja alokująca pamięć dla macierzy o zadanych wymiarach
float** createMatrix(int rows, int cols) {
    float** matrix = (float**)malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float*)malloc(cols * sizeof(float));
    }
    return matrix;
}

// Funkcja zwalniająca pamięć zaalokowaną dla macierzy
void freeMatrix(float** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Funkcja porównująca wyniki
int compareResults(float** output1, float** output2, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float diff = fabs(output1[i][j] - output2[i][j]);
            if (diff > 1e-6) { // tolerancja na błąd
                printf("Difference at [%d][%d]: %f\n", i, j, diff); // Debugging output
                return 0; // wyniki różnią się
            }
        }
    }
    return 1; // wyniki są takie same
}

// Sekwencyjna funkcja wykonująca konwolucję
void convolutionSequential(float** input, float** kernel, float** output, int inputRows, int inputCols, int kernelRows, int kernelCols) {
    int rowOffset = kernelRows / 2;
    int colOffset = kernelCols / 2;
    // Iterowanie przez każdy element macierzy wejściowej
    for (int i = 0; i < inputRows; i++) {
        for (int j = 0; j < inputCols; j++) {
            float sum = 0.0f;
            // Nakładanie jądra konwolucyjnego na otoczenie danego elementu
            for (int m = 0; m < kernelRows; m++) {
                for (int n = 0; n < kernelCols; n++) {
                    int x = i + m - rowOffset;
                    int y = j + n - colOffset;
                    // Sprawdzenie, czy indeksy mieszczą się w granicach macierzy
                    if (x >= 0 && x < inputRows && y >= 0 && y < inputCols) {
                        sum += input[x][y] * kernel[m][n];
                    }
                }
            }
            output[i][j] = sum;
        }
    }
}

// Równoległa wersja konwolucji wykorzystująca zrównoleglenie pętli zewnętrznych (Parallel Collapse)
void convolutionParallelCollapse(float** input, float** kernel, float** output, int inputRows, int inputCols, int kernelRows, int kernelCols) {
    int rowOffset = kernelRows / 2;
    int colOffset = kernelCols / 2;
    // Zrównoleglenie iteracji pętli przez OpenMP przy użyciu collapse(2) do równoległości po dwóch pętlach
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < inputRows; i++) {
        for (int j = 0; j < inputCols; j++) {
            float sum = 0.0f;
            // Obliczenie konwolucji dla elementu (i, j) przy wykorzystaniu jądra
            for (int m = 0; m < kernelRows; m++) {
                for (int n = 0; n < kernelCols; n++) {
                    int x = i + m - rowOffset;
                    int y = j + n - colOffset;
                    if (x >= 0 && x < inputRows && y >= 0 && y < inputCols) {
                        sum += input[x][y] * kernel[m][n];
                    }
                }
            }
            output[i][j] = sum;
        }
    }
}

// Równoległa wersja konwolucji z równolegleniem wewnętrznych obliczeń konwolucji (Parallel Inner)
void convolutionParallelInner(float** input, float** kernel, float** output, int inputRows, int inputCols, int kernelRows, int kernelCols) {
    int rowOffset = kernelRows / 2;
    int colOffset = kernelCols / 2;

    // Równoległe przetwarzanie wierszy
    #pragma omp parallel for
    for (int i = 0; i < inputRows; i++) {
        for (int j = 0; j < inputCols; j++) {
            float sum = 0.0f;

            // Obliczenia dla kernela
            for (int m = 0; m < kernelRows; m++) {
                for (int n = 0; n < kernelCols; n++) {
                    int x = i + m - rowOffset;
                    int y = j + n - colOffset;
                    if (x >= 0 && x < inputRows && y >= 0 && y < inputCols) {
                        sum += input[x][y] * kernel[m][n];
                    }
                }
            }
            output[i][j] = sum;
        }
    }
}

// Funkcja mierząca czas wykonania każdej wersji konwolucji i wypisująca wyniki w terminalu
void measureExecutionTime(int inputRows, int inputCols, int kernelRows, int kernelCols) {
    float** inputMatrix = createMatrix(inputRows, inputCols);
    float** kernelMatrix = createMatrix(kernelRows, kernelCols);
    float** outputMatrixSeq = createMatrix(inputRows, inputCols);
    float** outputMatrixPar = createMatrix(inputRows, inputCols); // do przechowywania wyników z równoległych konwolucji

    generateMatrix(inputMatrix, inputRows, inputCols);
    generateMatrix(kernelMatrix, kernelRows, kernelCols);

    // Sekwencyjna konwolucja
    double startSeq = omp_get_wtime();
    convolutionSequential(inputMatrix, kernelMatrix, outputMatrixSeq, inputRows, inputCols, kernelRows, kernelCols);
    double elapsedSeq = omp_get_wtime() - startSeq;

    printf("Sequential: Input [%dx%d], Kernel [%dx%d], Time: %f\n", inputRows, inputCols, kernelRows, kernelCols, elapsedSeq);

    // Parallel Collapse
    double startParCollapse = omp_get_wtime();
    convolutionParallelCollapse(inputMatrix, kernelMatrix, outputMatrixPar, inputRows, inputCols, kernelRows, kernelCols);
    double elapsedParCollapse = omp_get_wtime() - startParCollapse;
    printf("Parallel Collapse: Input [%dx%d], Kernel [%dx%d], Time: %f\n", inputRows, inputCols, kernelRows, kernelCols, elapsedParCollapse);

    // Sprawdź poprawność wyników
    if (compareResults(outputMatrixSeq, outputMatrixPar, inputRows, inputCols)) {
        printf("Results match for Parallel Collapse.\n");
    } else {
        printf("Results do NOT match for Parallel Collapse.\n");
    }

    // Parallel Inner
    double startParInner = omp_get_wtime();
    convolutionParallelInner(inputMatrix, kernelMatrix, outputMatrixPar, inputRows, inputCols, kernelRows, kernelCols);
    double elapsedParInner = omp_get_wtime() - startParInner;
    printf("Parallel Inner: Input [%dx%d], Kernel [%dx%d], Time: %f\n", inputRows, inputCols, kernelRows, kernelCols, elapsedParInner);

    // Sprawdź poprawność wyników
    if (compareResults(outputMatrixSeq, outputMatrixPar, inputRows, inputCols)) {
        printf("Results match for Parallel Inner.\n");
    } else {
        printf("Results do NOT match for Parallel Inner.\n");
    }

    // Zwalnianie pamięci
    freeMatrix(inputMatrix, inputRows);
    freeMatrix(kernelMatrix, kernelRows);
    freeMatrix(outputMatrixSeq, inputRows);
    freeMatrix(outputMatrixPar, inputRows);
}

int main() {
    // Wywołanie pomiarów dla różnych rozmiarów macierzy i jąder konwolucyjnych
    measureExecutionTime(100, 100, 5, 5);
    measureExecutionTime(1000, 1000, 9, 9);
    measureExecutionTime(2000, 2000, 15, 15);
    return 0;
}
