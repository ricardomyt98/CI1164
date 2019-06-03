#include <stdio.h>
#include <stdlib.h> /* exit, malloc, calloc, etc. */
#include <string.h>
#include <getopt.h> /* getopt */
#include <math.h>

#include <likwid.h>

#include "matriz.h"
#include "SistemasLineares.h"

/**
 * Exibe mensagem de erro indicando forma de uso do programa e termina
 * o programa.
 */

static void usage(char *progname)
{
    fprintf(stderr, "Forma de uso: %s [ -n <ordem> ] \n", progname);
    exit(1);
}

/**
 * Programa principal
 * Forma de uso: matmult [ -n <ordem> ]
 * -n <ordem>: ordem da matriz quadrada e dos vetores
 *
 */

int main(int argc, char *argv[])
{
    LIKWID_MARKER_INIT;

    int c, n = DEF_SIZE;
    double norma;

    MatPtr mPtr;
    MatRow mRow;
    MatCol mCol;
    Vetor vet, resPtr, resRow, resCol;

    /* =============== TRATAMENTO DE LINHA DE COMAANDO =============== */

    char *opts = "n:";
    c = getopt(argc, argv, opts);

    while (c != -1)
    {
        switch (c)
        {
        case 'n':
            n = atoi(optarg);
            break;
        default:
            usage(argv[0]);
        }

        c = getopt(argc, argv, opts);
    }

    /* ================ FIM DO TRATAMENTO DE LINHA DE COMANDO ========= */
    n = atoi(argv[1]);

    resPtr = (double *)malloc(n * sizeof(double));
    resRow = (double *)malloc(n * sizeof(double));
    resCol = (double *)malloc(n * sizeof(double));

    srand(20191);

    mPtr = geraMatPtr(n, n);
    mRow = geraMatRow(n, n);
    mCol = geraMatCol(n, n);
    vet = geraVetor(n);

#ifdef DEBUG
    prnMatPtr(mPtr, n, n);
    prnMatRow(mRow, n, n);
    prnMatCol(mCol, n, n);
    prnVetor(vet, n);
    printf("=================================\n\n");
#endif /* DEBUG */
    FILE *multMatPtrVetTimeReport = fopen("multMatPtrVetTimeReport.dat", "ab");
    FILE *multMatRowVetTimeReport = fopen("multMatRowVetTimeReport.dat", "ab");
    FILE *multMatColVetTimeReport = fopen("multMatColVetTimeReport.dat", "ab");
    FILE *normaMaxTimeReport = fopen("normaMaxTimeReport.dat", "ab");
    FILE *normaEuclTimeReport = fopen("normaEuclTimeReport.dat", "ab");
    double time;

    if (multMatPtrVetTimeReport == NULL || multMatRowVetTimeReport == NULL || multMatColVetTimeReport == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    time = multMatPtrVet(mPtr, vet, n, n, resPtr);
    fprintf(multMatPtrVetTimeReport, "%i %lf\n", n, time);
    fclose(multMatPtrVetTimeReport);

    time = multMatRowVet(mRow, vet, n, n, resRow);
    fprintf(multMatRowVetTimeReport, "%i %lf\n", n, time);
    fclose(multMatRowVetTimeReport);

    time = multMatColVet(mCol, vet, n, n, resCol);
    fprintf(multMatColVetTimeReport, "%i %lf\n", n, time);
    fclose(multMatColVetTimeReport);

    double time2;

    norma = normaMax(resRow, resPtr, n, &time2);
    fprintf(normaMaxTimeReport, "%i %lf\n", n, time2);
    fclose(normaMaxTimeReport);

    norma = normaEucl(resCol, n, &time2);
    fprintf(normaEuclTimeReport, "%i %lf\n", n, time2);
    fclose(normaEuclTimeReport);

#ifdef DEBUG
    prnVetor(resPtr, n);
    prnVetor(resRow, n);
    prnVetor(resCol, n);
#endif /* DEBUG */

    liberaMatPtr(mPtr, n);
    liberaVetor((void *)mRow);
    liberaVetor((void *)mCol);
    liberaVetor((void *)vet);

    free(resCol);
    free(resRow);
    free(resPtr);

    LIKWID_MARKER_CLOSE;

    return 0;
}
