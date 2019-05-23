/**
 * @file ZeroFuncao.c
 * @author Nome: Ricardo Norio Miyata | GRR: 20166368
 * @date 30/03/2019
 * @brief Arquivo com a implementação das funções.
 */

#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "ZeroFuncao.h"

/**
 * @brief Função para encontrar uma possível raiz válida pelo método da Bisseção.
 * 
 * @param f Ponteiro para uma função.
 * @param a Ponto para definir o limite inferior.
 * @param b Ponto para definir limite superior.
 * @param eps Erro relativo máximo em x.
 * @param it Número de iterações.
 * @param raiz Raiz encontrada.
 * \return 0 - Raiz válida.
 * \return 1 - Não garantia de existência de raiz.
 * \return 2 - Divisão por zero.
 * \return 3 - Número máximo de iterações atingido.
 */
int bisseccao (double (*f)(const double x), double a, double b, double eps, int *it, double *raiz) {
    if (f(a)*f(b) >= 0) {
        fprintf(stderr, "Erro 1 - Os valores resultantes da função em \"a\" e \"b\" não garantem raiz possível no método da bisseção. Desconsiderar a raiz encontrada.\n");
        return 1; // (Error) Opposite sings (requirement)
    } else {
        double rPrev, fa = f(a), fb = f(b), fr, rootFabs, relError;

        *raiz = (a+b) / 2;
        *it = 1;

        do {
            rPrev = *raiz;
            fr = f(rPrev);

            if (fa*fr < 0.0) {
                b = *raiz; // Between a and raiz
                fb = fr;
            } else if (fa*fr > 0.0) {
                a = *raiz; // Between b and raiz
                fa = fr;
            }
            
            *raiz = (a+b) / 2;
            rootFabs = fabs(*raiz); // Absolute value of root
            
            if (rootFabs == 0.0) {
                fprintf(stderr, "Erro 2 - Divisão por 0. Desconsiderar a raiz encontrada.\n");
                return 2; // (Error) Previne the division by zero to get the approximate error
            } else {
                relError = fabs((*raiz-rPrev)/(*raiz)) * 100;
            }

            (*it)++; // Increment the number of iterations

            if ((*it) > MAXIT) {
                fprintf(stderr, "Erro 3 - Número de iterações máximo insuficiente. Desconsiderar a raiz encontrada.\n");
                return 3; // (Error) The number of iterations is not enough
            }
        } while (relError > eps); // Is not greater than the max number of iterations and calculate the approximate error
        
        return 0;
    }
}

/**
 * @brief Função para encontrar uma possível raiz pelo método de Newton.
 * 
 * @param f Ponteiro para uma função.
 * @param df Ponteiro para a derivada de uma função.
 * @param x0 Valor inicial.
 * @param eps Erro relativo máximo em x.
 * @param it Número de iterações.
 * @param raiz Raiz encontrada.
 * \return 0 - Raiz válida.
 * \return 2 - Divisão por zero.
 * \return 3 - Número máximo de iterações atingido.
 */
int newton (double (*f)(const double x), double (*df)(const double x), double x0, double eps, int *it, double *raiz) {
    if (df(x0) == 0.0) {
        fprintf(stderr, "Erro 2 - Divisão por 0. Desconsiderar a raiz encontrada.\n");
        return 2; // (Error) Division by zero of the initial guess
    } else {
        double rPrev, dfPrev;

        *raiz = x0 - (f(x0)/df(x0));
        *it = 0;

        while (fabs((*raiz-rPrev)/(*raiz)) * 100 > eps) {
            rPrev = *raiz;
            dfPrev = df(rPrev);
         
            if (dfPrev == 0.0) {
                fprintf(stderr, "Erro 2 - Divisão por 0. Desconsiderar a raiz encontrada.\n");
                return 2; // (Error) Division by zero of the previous root
            } else {
                *raiz = rPrev - (f(rPrev)/dfPrev);
            }

            (*it)++; // Increment the number of iterations

            if ((*it) == MAXIT) {
                fprintf(stderr, "Erro 3 - Número de iterações máximo insuficiente. Desconsiderar a raiz encontrada.\n");
                return 3; // (Error)The number of iterations is not enough
            }
        }

        return 0;
    }
}

/**
 * @brief Função para encontrar uma possível raiz pelo método da Secante.
 * 
 * @param f Ponteiro para uma função.
 * @param x0 Valor inicial.
 * @param x1 Valor inicial.
 * @param eps Erro relativo máximo em x.
 * @param it Número de iterações.
 * @param raiz Raiz encontrada.
 * \return 0 - Raiz válida.
 * \return 1 - Não garantia de existência de raiz.
 * \return 3 - Número máximo de iterações atingido.
 * \return 4 - Intervalo inexistente para os valores "x0" e "x1" fornecidos.
 */
int secante (double (*f)(const double x), double x0, double x1, double eps, int *it, double *raiz) {
    if (f(x0) == f(x1)) {
        fprintf(stderr, "Erro 4 - Os resultados dos valores de \"x0\" e \"x1\" em \"f\" devem ser diferentes para garantir um intervalo.\n");
        return 4; // (Error) The resulting values of the intervals in f() must be different to find a solution
    } else if (f(x0)*f(x1) >= 0.0) {
        fprintf(stderr, "Erro 1 - Os valores resultantes da função em \"a\" e \"b\" não garantem raiz possível no método da bisseção. Desconsiderar a raiz encontrada.\n");
        return 1; // (Error) Opposite sings (requirement)
    } else {
        *it = 1;

        do {
            *raiz = (x0*f(x1) - x1*f(x0)) / (f(x1) - f(x0));
            x0 = x1;
            x1 = *raiz;
            
            (*it)++; // Increment the number of iterations

            if ((*it) == MAXIT) {
                fprintf(stderr, "Erro 3 - Número de iterações máximo insuficiente. Desconsiderar a raiz encontrada.\n");
                return 3; // (Error) The number of iterations is not enough
            }
        } while(fabs(f(*raiz)) > eps);
    }
    
    return 0;
}

/**
 * @brief Função para encontrar o valor de uma equação polinomial e sua derivada em um ponto.
 * 
 * Possíveis erros de soma:
 * A variável "b" pode crescer muito rápido de acordo com a constante "x" passada, podendo gerar truncamento.
 * A variábel "b" pode decrescer muito rápido de acordo com a constante "x" passada, podendo gerar cancelamento pelo coeficiente (p[i]). 
 * 
 * @param p Ponteiro uma estrutura de polinômio.
 * @param x Valor para calcular o resultado do polinômio e de sua derivada
 * @param px Valor do polinômio
 * @param dpx Valor da derivada primeira
 * \return 0 - Valores encontrados consistentes.
 * \return 5 - Grau do polinômio inconsistente.
 */
int calcPolinomioEDerivada(Polinomio p, double x, double *px, double *dpx) {
    if (p.grau < 1) {
        fprintf(stderr, "Erro 5 - O grau do polinômio deve ser maior ou igual a 1.\n");
        return 5; // (Error) A polynomial equation must have a degree greater than or equal to 1
    } else {   
        double b = p.p[p.grau], c = b;
        
        for (int i = p.grau-1; i; i--) {
            b = p.p[i] + b * x;
            c = b + c * x;
        }
 
        b = p.p[0] + b * x;
        *px = b;
        *dpx = c;

        return 0;
    }
}

/**
 * @brief Função para calcular a média de um vetor de números reais.
 * 
 * Possíveis erros de precisão:
 * Devido a possivelmente haver uma grande variação entre os valores do vetor, a soma destes pode gerar propagação de erro a cada iteração.
 * Uma maneira de minimizar este erro seria para cada valor do vetor, ir dividindo este por "n" ("padronizando" os valores para a soma), porém isto geraria o dobro de operações, podendo causar grande lentidão.
 * 
 * Possível erro de divisão:
 * A soma dos elementos do vetor pode resultar em um valor muito grande, "n" ser muito pequeno e vice-versa, causando erro na divisão.
 * 
 * @param valores Vetor com os números reais.
 * @param n Quantidade de elementos do vetor.
 * @return Resultado da média aritmetica.
 */
double media(double *valores, unsigned long n) {
    double sum = 0.0;

    for (int i = 0; i < n; i++) {
        sum = sum + valores[i];
    }

    return sum / n;
}
