#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "SistemasLineares.h"

int main()
{
    int n;
    scanf("%d", &n);
    SistLinear_t *s = alocaSistLinear(n);
    s->n = n;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            scanf("%lf", &s->A[i * n + j]);
    for (int i = 0; i < n; i++)
        scanf("%lf", &s->b[i]);

    double normaL2;
    int iter;

    // eliminacaoGauss(s, &normaL2);
    gaussJacobi(s, 0.00000000000001F, &normaL2, &iter);
    printf("iter = %d\n", iter);
    for (int i = 0; i < n; i++)
        printf("x[%d] = %lf\n", i, s->x[i]);
    gaussSeidel(s, 0.00000000000001F, &normaL2, &iter);

    printf("iter = %d\n", iter);
    for (int i = 0; i < n; i++)
        printf("x[%d] = %lf\n", i, s->x[i]);
    return 0;
}

