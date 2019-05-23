/**
 * @file SistemasLineares.c
 * @author Ricardo Norio Miyata (GRR20166368)
 * @brief Trabalho 2 de Introdução à Computação Científica (CI1164).
 * Informações mais específicas ou detalhamento estão como comentários dentro do código.
 * @date 2019-04-23
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "SistemasLineares.h"

/**
 * @brief Macro para acesso a uma determinada posição da matriz com um vetor linha auxiliar como parâmetro.
 * 
 */
#define ACESSA_MATRIX(SL, i, j, line) SL->A[line[i] * SL->n + (j)]
/**
 * @brief Macro para acesso a uma determinada posição da matriz com dois vetores auxiliares, um linha e outro coluna como parâmetros.
 * 
 */
#define ACESSA_MATRIX_2(SL, i, j, line, col) SL->A[line[i] * SL->n + col[j]]
/**
 * @brief Macro para acesso a uma determinada posição de um vetor com um vetor auxiliar como parâmetro.
 * 
 */
#define ACESSA_ARRAY(array, i, index) array[index[i]]
/**
 * @brief Macro para retorno do valor absoluto.
 * 
 */
#define fabs(a) (a) < 0.0 ? -(a) : (a)
/**
 * @brief Define com o código de erro 1.
 * 
 */
#define DIV_ZERO 1
/**
 * @brief Define com o código de erro 2.
 * 
 */
#define MAX_ITER 2
/**
 * @brief Define com a mensagem de erro 1.
 * 
 */
#define ERRO_1 "Erro, divisão por zero.\n"
/**
 * @brief Define com a mensagem de erro 2.
 * 
 */
#define ERRO_2 "Erro, número máximo de iterações atingido.\n"

/**
 * @brief Função para alocar espaço em memória para a estrutura SistLinear_t.
 * 
 * @param n Ordem da matriz ou número de sistemas.
 * @return SistLinear_t* Ponteiro para a estrutura alocada em memória.
 */
SistLinear_t *alocaSistLinear(unsigned int n)
{
	SistLinear_t *SL = (SistLinear_t *)malloc(sizeof(SistLinear_t));

	if (SL)
	{
		SL->A = (real_t *)malloc(n * n * sizeof(real_t));
		SL->b = (real_t *)malloc(n * sizeof(real_t));
		SL->x = (real_t *)malloc(n * sizeof(real_t));
		if (!(SL->A) || !(SL->b) || !(SL->x))
			liberaSistLinear(SL);
	}

	return (SL);
}

/**
 * @brief Função para liberação de memória alocada.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 */
void liberaSistLinear(SistLinear_t *SL)
{
	free(SL->A);
	free(SL->b);
	free(SL->x);
	free(SL);
}

/**
 * @brief Variável (vetor global) para linhas. Criada para melhorar a performance nas operações de trocas de linhas de uma matriz, 
 * pois a troca, de fato, de todos os elementos de uma linha resulta em uma operação bastante custosa. 
 * 
 */
int *line = NULL;
/**
 * @brief Variável (vetor global) para colunas. Criada para melhorar a performance nas operações de trocas de colunas de uma matriz, 
 * pois a troca, de fato, de todos os elementos de uma coluna resulta em uma operação bastante custosa.
 * 
 */
int *col = NULL;

/**
 * @brief Função para cálculo da norma L2 do resíduo.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @return double Resultado do cálculo.
 */
double normaL2Residuo(SistLinear_t *SL)
{
	real_t *tmp = malloc(SL->n * sizeof(real_t));

	for (int i = 0; i < SL->n; i++)
	{
		ACESSA_ARRAY(tmp, i, line) = ACESSA_ARRAY(SL->b, i, line);

		for (int j = 0; j < SL->n; j++)
		{
			ACESSA_ARRAY(tmp, i, line) -= (ACESSA_MATRIX(SL, i, j, line) * ACESSA_ARRAY(SL->x, j, col)); // b – A * x0.
		}
	}

	real_t result = 0;

	for (int i = 0; i < SL->n; i++)
	{
		result += ACESSA_ARRAY(tmp, i, line) * ACESSA_ARRAY(tmp, i, line);
	}

	free(tmp);

	return result;
}

/**
 * @brief Função para encontrar o pivô de uma coluna.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param col Variável índice para indicar a coluna de busca do pivô.
 * @param line Variável declarada globalmente para melhorar o desempenho na troca de linhas.
 * @return int Retorna o índice do pivô.
 */
int encontraMax(SistLinear_t *SL, int col, int *line)
{
	int iPivo = col;

	for (int i = col + 1; i < SL->n; i++)
	{
		if (ACESSA_MATRIX(SL, i, col, line) > ACESSA_MATRIX(SL, iPivo, col, line))
		{
			iPivo = i;
		}
	}

	return iPivo;
}

/**
 * @brief Função para trocar posições.
 * 
 * @param i Índice de acesso.
 * @param j Índice de acesso.
 * @param array Vetor auxiliar para a troca de posições segundo abordagem de otimização neste projeto aplicada.
 */
void trocaPosicao(int i, int j, int *array)
{
	int tmp = array[j];

	array[j] = array[i];
	array[i] = tmp;
}

/**
 * @brief Função para a resolução retroativa ("de baixo para cima") de um conjunto de sistemas lineares.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param line Variável declarada globalmente para melhorar o desempenho na mudança de posições de uma matriz, é passada para acessar posições.
 */
void resolucaoRetroativa(SistLinear_t *SL, int *line)
{
	for (int i = SL->n - 1; i >= 0; i--)
	{
		real_t tmp = ACESSA_ARRAY(SL->b, i, line);

		for (int j = i + 1; j < SL->n; j++)
		{
			tmp -= ACESSA_MATRIX(SL, i, i + 1, line) * SL->x[i];
		}

		SL->x[i] = tmp / ACESSA_MATRIX(SL, i, i, line);
	}
}

/**
 * @brief Função com a implementação do método de eliminação de Gauss com pivoteamento parcial.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param normaL2 Parâmetro passado por referência para cálculo da norma L2 do resíduo.
 * @return int Código para representar o estado final da execução. 0 - Execução correta ou 1 - Divisão por zero.
 */
int eliminacaoGauss(SistLinear_t *SL, double *normaL2)
{
	line = malloc(SL->n * sizeof(int));
	col = malloc(SL->n * sizeof(int));

	for (int i = 0; i < SL->n; i++) // Inicializando vetores de índices linha e coluna.
	{
		line[i] = i;
		col[i] = i;
	}

	for (int i = 0; i < SL->n; i++)
	{
		int iPivo = encontraMax(SL, i, line);

		if (ACESSA_MATRIX(SL, iPivo, i, line) == 0) // Probabilidade de entrada nesta condição de praticamente zero por ser ponto flutuante.
		{
			fprintf(stderr, ERRO_1);

			return DIV_ZERO;
		}

		if (i != iPivo)
		{
			trocaPosicao(i, iPivo, line); // Troca o iPivo pelo i, então agora o i é o pivo.
		}

		for (int j = i + 1; j < SL->n; j++)
		{
			real_t m = ACESSA_MATRIX(SL, j, i, line) / ACESSA_MATRIX(SL, i, i, line);

			ACESSA_MATRIX(SL, j, i, line) = 0.0;

			for (int k = i + 1; k < SL->n; k++)
			{
				ACESSA_MATRIX(SL, j, k, line) -= ACESSA_MATRIX(SL, i, k, line) * m;
			}

			ACESSA_ARRAY(SL->b, j, line) -= ACESSA_ARRAY(SL->b, i, line) * m;
		}
	}

	resolucaoRetroativa(SL, line);
	*normaL2 = normaL2Residuo(SL);
	free(line);
	free(col);
	line = NULL;
	col = NULL;

	return (0);
}

/**
 * @brief Função para calcular o alpha máximo usado no método de Gauss Jacobi.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param i Índice da linha atual da matriz (sistema linear).
 * @param line Vetor auxiliar para acessar posições da matriz.
 * @param col Vetor auxiliar para acessar posições da matriz.
 * @return int Valor do alpha encontrado.
 */
int encontraMaxAlpha(SistLinear_t *SL, int i, int *line, int *col)
{
	real_t lastAlpha, alpha = 0;
	int jAlpha = 0; // Índice da coluna com o maior alpha.

	for (int j = i; j < SL->n; j++) // Realiza troca de colunas.
	{
		lastAlpha = alpha;
		alpha = 0;

		for (int k = i; k < j; k++) // Antes do alvo (número na matriz) atual.
		{
			alpha += fabs(ACESSA_MATRIX_2(SL, i, j, line, col));
		}

		for (int k = j + 1; k < SL->n; k++) // Depois do alvo (número na matriz) atual.
		{
			alpha += fabs(ACESSA_MATRIX_2(SL, i, j, line, col));
		}

		alpha /= ACESSA_MATRIX_2(SL, i, j, line, col); // Divisão pelo termo atual A(i, j).

		if (lastAlpha < alpha)
		{
			jAlpha = j;
		}
	}

	return jAlpha;
}

/**
 * @brief Função para calcular o beta máximo usado no método de Gauss Seidel.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param i Índice da linha atual da matriz (sistema linear).
 * @param line Vetor auxiliar para acessar posições da matriz.
 * @param col Vetor auxiliar para acessar posições da matriz.
 * @return int Valor do beta encontrado.
 */
int encontraMaxBeta(SistLinear_t *SL, int i, int *line, int *col)
{
	real_t *beta = malloc(SL->n * sizeof(real_t));
	int jBeta = 0; // Índice da coluna com o maior beta.

	for (int j = i; j < SL->n; j++) // Realiza troca de colunas.
	{
		beta[j] = 0;

		for (int k = i; k < j; k++)
		{
			beta[j] += fabs(ACESSA_MATRIX_2(SL, i, j, line, col)) * beta[k];
		}

		for (int k = j + 1; k < SL->n; k++)
		{
			beta[j] += fabs(ACESSA_MATRIX_2(SL, i, j, line, col));
		}

		beta[j] /= ACESSA_MATRIX_2(SL, i, j, line, col);

		if (beta[jBeta] < beta[j])
		{
			jBeta = j;
		}
	}

	return jBeta;
}

/**
 * @brief Função para calcular o valor do erro absoluto.
 * 
 * @param xAtual Valor atual.
 * @param xAnterior Valor anterior.
 * @param n Ordem da matriz.
 * @return real_t Valor do erro absoluto.
 */
real_t calculaErro(real_t *xAtual, real_t *xAnterior, int n)
{
	real_t erroMaximo = xAtual[0] - xAnterior[0];
	real_t erro;

	for (int i = 1; i < n; i++)
	{
		erro = xAtual[i] - xAnterior[i];

		if (erro > erroMaximo)
		{
			erroMaximo = erro;
		}
	}

	return erroMaximo;
}

/**
 * @brief Função com a implementação do método de Gauss Jacobi.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param eps Erro máximo da norma em x.
 * @param normaL2 Parâmetro passado por referência para cálculo da norma L2 do resíduo.
 * @param iter Parâmetro passado para contagem do número de iterações.
 * @return int Código para representar o estado final da execução. 0 - Execução correta ou 2 - Número máximo de iterações atingido.
 */
int gaussJacobi(SistLinear_t *SL, double eps, double *normaL2, unsigned int *iter)
{
	line = malloc(SL->n * sizeof(int));
	col = malloc(SL->n * sizeof(int));

	for (int i = 0; i < SL->n; i++) // Inicializando vetores de índices linha e coluna.
	{
		line[i] = i;
		col[i] = i;
	}

	memset(SL->x, 0, SL->n * sizeof(real_t)); // Setando a solução inicial como zero.

	*iter = 0;

	real_t *tmpX = malloc(SL->n * sizeof(real_t));
	real_t erro;

	for (int i = 0; i < SL->n; i++) // Ordenando as colunas (diagonal dominante).
	{
		int jAlpha = encontraMaxAlpha(SL, i, line, col);

		if (i != jAlpha)
		{
			trocaPosicao(i, jAlpha, col);
		}
	}

	do // Cálculo da nova solução
	{
		memcpy(tmpX, SL->x, SL->n * sizeof(real_t));

		for (int i = 0; i < SL->n; i++)
		{
			ACESSA_ARRAY(SL->x, i, col) = SL->b[i];

			for (int j = 0; j < i; j++)
			{
				ACESSA_ARRAY(SL->x, i, col) -= ACESSA_MATRIX_2(SL, i, j, line, col) * ACESSA_ARRAY(tmpX, j, col);
			}

			for (int j = i + 1; j < SL->n; j++)
			{
				ACESSA_ARRAY(SL->x, i, col) -= ACESSA_MATRIX_2(SL, i, j, line, col) * ACESSA_ARRAY(tmpX, j, col);
			}

			ACESSA_ARRAY(SL->x, i, col) /= ACESSA_MATRIX_2(SL, i, i, line, col);
		}

		*normaL2 = normaL2Residuo(SL);
		erro = calculaErro(SL->x, tmpX, SL->n);
		(*iter)++;
	} while (fabs(erro) >= eps && *iter < MAXIT); // Pivoteamento parcial por colunas (baseado em alpha)

	if (*iter == MAXIT)
	{
		fprintf(stderr, ERRO_2);

		return MAX_ITER;
	}

	free(tmpX);
	free(line);
	free(col);
	line = NULL;
	col = NULL;

	return (0);
}

/**
 * @brief Função com a implementação do método de Gauss Seidel.
 * 
 * @param SL Ponteiro para a estrutura SistLinear_t.
 * @param eps Erro máximo da norma em x.
 * @param normaL2 Parâmetro passado por referência para cálculo da norma L2 do resíduo.
 * @param iter Parâmetro passado para contagem do número de iterações.
 * @return int Código para representar o estado final da execução. 0 - Execução correta ou 2 - Número máximo de iterações atingido.
 */
int gaussSeidel(SistLinear_t *SL, double eps, double *normaL2, unsigned int *iter)
{
	line = malloc(SL->n * sizeof(int));
	col = malloc(SL->n * sizeof(int));

	for (int i = 0; i < SL->n; i++)
	{
		line[i] = i;
		col[i] = i;
	}

	memset(SL->x, 0, SL->n * sizeof(real_t));
	*iter = 0;

	for (int i = 0; i < SL->n; i++)
	{
		int jBeta = encontraMaxBeta(SL, i, line, col);

		if (i != jBeta)
		{
			trocaPosicao(i, jBeta, col);
		}
	}

	real_t *tmpX = malloc(SL->n * sizeof(real_t));
	real_t erro;

	do // Cálculo da nova solução.
	{
		memcpy(tmpX, SL->x, SL->n * sizeof(real_t));

		for (int i = 0; i < SL->n; i++)
		{
			ACESSA_ARRAY(SL->x, i, col) = SL->b[i];

			for (int j = 0; j < i; j++)
			{
				ACESSA_ARRAY(SL->x, i, col) -= ACESSA_MATRIX_2(SL, i, j, line, col) * ACESSA_ARRAY(SL->x, j, col);
			}

			for (int j = i + 1; j < SL->n; j++)
			{
				ACESSA_ARRAY(SL->x, i, col) -= ACESSA_MATRIX_2(SL, i, j, line, col) * ACESSA_ARRAY(SL->x, j, col);
			}

			ACESSA_ARRAY(SL->x, i, col) /= ACESSA_MATRIX_2(SL, i, i, line, col);
		}

		*normaL2 = normaL2Residuo(SL);
		erro = calculaErro(SL->x, tmpX, SL->n);
		(*iter)++;
	} while (fabs(erro) >= eps && *iter < MAXIT); // Pivoteamento parcial por colunas (baseado em beta)

	if (*iter == MAXIT)
	{
		fprintf(stderr, ERRO_2);
		return MAX_ITER;
	}

	free(tmpX);
	free(line);
	free(col);
	line = NULL;
	col = NULL;

	return (0);
}
