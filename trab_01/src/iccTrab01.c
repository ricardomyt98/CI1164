#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "ZeroFuncao.h"

double fun(double x) {
  return x*x*x - x*x + 2;
}

double newtFun(double x) { 
  return x*x*x - x*x + 2; 
} 
  
double derivNewtFun(double x) { 
  return 3*x*x - 2*x; 
}

double secFun(double x) {
  return pow(x, 3) + x - 1; 
}

int main () {
  int iter, error;
  double x, root, eps = 0.001, a = -19.27, b = 980.57, x0 = -20, px, dpx;
  Polinomio p;
  p.p = (double[4]){2, -6, 2, -1};
  p.grau = 4;
  double *valArray;
  valArray = (double[5]){1, 2, 3, 4, 5};


// Bisection
  error = bisseccao(fun, a, b, eps, &iter, &root);

  printf("--- Bisection ---\n");
  printf("Return: %d| Root: %f| Iterations: %d.\n\n", error, root, iter);

// Newton
  error = newton(newtFun, derivNewtFun, x0, eps, &iter, &root);

  printf("--- Newton ---\n");
  printf("Return: %d| Root: %f| Iterations: %d.\n\n", error, root, iter);

// Secant
  error = secante(secFun, a, b, eps, &iter, &root);

  printf("--- Secant ---\n");
  printf("Return: %d| Root: %f| Iterations: %d.\n\n", error, root, iter);
  
// Polynomial
  error = calcPolinomioEDerivada(p, 3, &px , &dpx);

  printf("--- Polynomial & Derivative ---\n");
  printf("Return: %d| Polynomial value: %f| Derivative: %f.\n\n", error, px, dpx);

// Average
  double average = media(valArray, 5);

  printf("--- Average ---\n");
  printf("Average: %f.\n\n", average);


  return 0;
}

