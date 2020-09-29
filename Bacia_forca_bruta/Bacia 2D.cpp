/* PROGRAMA Bac_Din_o1.CPP
   
  Programa para o calculo da bacia de atracao de um sistema de equacoes de moviemnto.

  Utiliza o metodo da forca bruta para calcular o atrator de cada ponto.

  Para cada ponto, vai integrando cada periodo e avaliando se converge para um
  atrator, contando tambem a periodicidade da solucao.

  
*/

#define _BACIA_C

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io.h>
#include <string.h>
#include <time.h>

/******************Declaracoes principais ******************/
#include "_config_modelo/Kutta_header_config.h"

double Wf, Tf;
double eta__1, eta__2, PL;
double PL_8C, PL_8S, PL_9C, PL_9S;


int PeriodoMaximo;
double Passo;
double Y1min,Y1max,Y2min,Y2max;
int Num_cel,Cor1,Cor2;
double *q1,*q1p,*q2,*q2p;
double k1[Nequ], k2[Nequ], k3[Nequ], k4[Nequ], g1[Nequ], g2[Nequ], g3[Nequ], g4[Nequ];

/******************Declaracoes das funcoes ******************/
int Runge_Kutta(double *y, double *x, double Step, int Total);
void NewData(void);
void CellsTrajec(void);
void CellsTrajec_core(int const nome_thread, int const cell_inicio, int const cell_fim);


/*===========================  Func  ===========================*/
#include "_config_modelo/arquivo_equacoes.h"

/*===========================  Runge_Kutta  ===========================*/
int Runge_Kutta(double *y, double *x, double Step, int Total)
{

	int i, j;
	double t1, t2, t3, t4, t;
	int count;

	for (i = 0; i < Nequ; i++)
	{
		k1[i] = 0.0;
		k2[i] = 0.0;
		k3[i] = 0.0;
		k4[i] = 0.0;
		g1[i] = 0.0;
		g2[i] = 0.0;
		g3[i] = 0.0;
		g4[i] = 0.0;

	}

	for (i = 0; i < Nequ; i++)
		y[i] = x[i];

	t = 0.0;
	j = 0;

	/* Integracao de Runge-Kutta para um periodo */
	while (j < Total)
	{
		t1 = t;
		for (i = 0; i < Nequ; i++)
		{
			g1[i] = y[i];
		}
		Func(k1, g1, t1, Wf);
		
		t2 = t + Step * 0.5;
		for (i = 0; i < Nequ; i++)
		{
			g2[i] = y[i] + k1[i] * 0.5*Step;
		}
		Func(k2, g2, t2, Wf);

		t3 = t + Step * 0.5;
		for (i = 0; i < Nequ; i++)
		{
			g3[i] = y[i] + k2[i] * 0.5*Step;
		}
		Func(k3, g3, t3, Wf);

		t4 = t + Step;
		for (i = 0; i < Nequ; i++)
		{
			g4[i] = y[i] + k3[i] * Step;
		}
		Func(k4, g4, t4, Wf);


		for (i = 0; i < Nequ; i++)
		{
			y[i] = y[i] + (k1[i] + 2.0*(k2[i] + k3[i]) + k4[i]) / 6.0 * Step;
		}

		t = t + Step;
		j++;
	}

	return(1);
}

/* ===========================  NewData  ===========================*/
void NewData(void)
{
	FILE    *fdread;
	long int i;
	double a, b, c, d;

	/* Get the new file name from the user and open it. */
	fdread = fopen("bacia.dat", "r");
	if (fdread == NULL) {
		printf("\n Nao foi possivel abrir arquivo bacia.dat!\n");
		exit(0);
		return;
	}

	rewind(fdread);

	/* Dimension doof space */
	fscanf(fdread, "%d\n", &PeriodoMaximo);

	/* Frequencia da forca excitadora, dados de amortecimentos e amplitudes da carga */
	fscanf(fdread, "%lf\n", &Wf);
	fscanf(fdread, "%lf %lf", &eta__1, &eta__2);
	fscanf(fdread, "%lf %lf %lf %lf %lf\n", &PL_8C, &PL_8S, &PL_9C, &PL_9S, &PL);

	/* Numero de células */
	fscanf(fdread, "%d\n", &Num_cel);

	/* Coordenadas de analise */
	fscanf(fdread, "%d %d\n", &Cor1, &Cor2);

	// verificacao das coordernadas a serem analisadas
	if (Cor1 >= Nequ && Cor2 >= Nequ && Cor1 == Cor2)
	{
		printf("Erro nas coordernadas a serem analisadas. Verifique! \n");
		exit(0);
	}

	/* Periodo da forca excitadora */
	Tf = 2 * Pi / Wf;

	/* Passo para integracao no tempo */
	Passo = Tf / 500;

	/* Alocaao de vetores para o tamanho da celula */
	q1 = (double*)calloc(Num_cel, sizeof(double));
	q1p = (double*)calloc(Num_cel, sizeof(double));


	/* Valor das coordenadas fixas para a analise */
	for (i = 0; i < Num_cel; i++)
	{
		//fscanf( fdread,"%lf %lf %lf %lf\n", &a, &b, &c, &d );
		fscanf(fdread, "%lf %lf\n", &a, &b);
		q1[i] = a;
		q1p[i] = b;

	}
	/*Valor dos limites*/
	fscanf(fdread, "%lf %lf %lf %lf\n", &a, &b, &c, &d);
	Y1min = a;
	Y1max = b;
	Y2min = c;
	Y2max = d;

	return;
}

/* ===========================  CellsTrajec  ===========================*/
void CellsTrajec(void)
{
	FILE *fd;


	long int cellnum;

	/* Abre arquivo de impressao   */
	fd = fopen("bacia_results.txt", "w");
	if (fd == NULL) {
		printf("\n Nao foi possivel abrir arquivo de bacia_results.txt !\n");
		exit(0);
		return;
	}

	/* Imprime cabecalho na tela */
	printf("Thread  Numero            q1_atr               q1p_atr               Tempo   Periodicidade\n");

	/* Integracao no tempo para cada celula do espaco  */
	cellnum = 1;

	// loop que percorre as celulas que definem o ponto inicial da bacia
	for (int i = 0; i < Num_cel; i++)
	{
		CellsTrajec_core(1,i, i + 2);
		cellnum++;

	}
	/*free(x);
	free(y_old);
	free(xo);
	free(y);*/
	fclose(fd);
	return;
}


void CellsTrajec_core( int const nome_thread, int const cell_inicio, int const cell_fim)
{
	//int const nome_thread = nome sequencial da thread utilizado para criar arquivo de saida
	//int const cell_inicio = valor de busca da primeira celula no vetor q1 e q1p
	//int const cell_fim = valor de busca da ultima celula no vetor q1 e q1p

	// declaracao das variaveis locais
	int i, j, retorno, Periodo, PeriodoBack, flag;
	int value, ij, Tempo;
	double x[Nequ], y[Nequ], y_old[Nequ], xo[Nequ], derro, zo0atr, zo1atr;

	FILE *fd_thread;

	/* Abre arquivo de impressao   */
	char nome_arquivo[50];
	int arquivo_criado = sprintf(nome_arquivo, "bacia_results_%d.txt", nome_thread);
	fd_thread = fopen(nome_arquivo, "w");
	if (fd_thread == NULL && arquivo_criado != -1) {
		printf("\n Nao foi possivel abrir arquivo de %s !\n", nome_arquivo);
		exit(0);
		return;
	}

	int const total_celulas = cell_fim - cell_inicio + 1;
	for (int i = 0; i < total_celulas; i++)
	{
		/* Contador de Tempo e de periodicidade da solucao */
		Tempo = 0;
		Periodo = 0;

		// inicializa todas as variaveis 
		for (int idx = 0; idx < Nequ; idx++)
		{
			x[idx] = 1e-4;
		}

		/* Coordenada de cada celula */
		x[Cor1] = q1[i + cell_inicio];
		x[Cor2] = q1p[i + cell_inicio];

		/* Coordenadas iniciais */
		for (ij = 0; ij < Nequ; ij++)
		{
			y_old[ij] = x[ij];
			xo[ij] = x[ij];
			y[ij] = 0;
		}
		flag = 1;


		while (flag && y[Cor1] < 1e10)
		{
			/* Integracao no tempo */
			value = Runge_Kutta(y, x, Passo, 500);
			/* Conta numero de periodos de integracao e contador para periodicidade da solucao */
			Tempo++;
			Periodo++;
			/* Avalia se ponto calculado é o mesmo ao inicial  */
			retorno = 0;
			for (ij = 0; ij < Nequ; ij++)
			{
				derro = fabs(y[ij] - y_old[ij]);
				if (derro > 1.0e-8)
					retorno++;
				PeriodoBack = Periodo;
			}
			/*  Avalia retorno */
			if (retorno == 0)
			{
				flag = 0;
				continue;
			}
			/* Reinicia contador de periodos */
			if (Periodo == PeriodoMaximo)
			{
				Periodo = 0;
				/* Coordenadas iniciais para o novo ponto de analise (vetor x)*/
				for (ij = 0; ij < Nequ; ij++)
					y_old[ij] = y[ij];
			}
			/* Novo valor para vetor x  */
			for (ij = 0; ij < Nequ; ij++)
				x[ij] = y[ij];
			/* Limita o numero de integracoes, caso nao exista solucao periodica  */
			if (Tempo > 8000)
				flag = 0;
		}
		/* Celula mapeada (atrator)  */
		zo0atr = y[Cor1];
		zo1atr = y[Cor2];

		printf("%5d  /%6d / %6d", nome_thread, i + cell_inicio, Num_cel);
		printf("   %15.12e   %15.12e %8d  %2d  \n", y[Cor1], y[Cor2], Tempo, Periodo);
		/* Imprime resultados do atrator da celula */
		if (Tempo < 8000)
		{
			if (Y1min <= y[Cor1])
				//if(Y2min<=y[Cor1])
			{
				if (y[Cor1] <= Y1max)
					//if(y[Cor1]<=Y2max)
				{
					if (y[Cor2] >= Y2min)
						//if(y[Cor2]>=Y1min)
					{
						if (y[Cor2] <= Y2max)
							//if(y[Cor2]<=Y1max)
						{
							fprintf(fd_thread, "%16.12e, %16.12e, %16.12e, %16.12e,", xo[Cor1], xo[Cor2], y[Cor1], y[Cor2]);
							for (j = 0; j < Nequ; j = j + 2)
							{
								fprintf(fd_thread, "%16.12e,  %16.12e,  ", y[j], y[j + 1]);
							}
							fprintf(fd_thread, "%5d\n", nome_thread);
						}
					}
				}
			}
		}
	}
}


/* ===========================  DoubToInt  ===========================*/
int DoubToInt(double doub)
{
	double result;
	char *str;
	int dec, sign, ndig = 0, n;

	result = fmod(doub,1.0);
	result = doub-result;
	str = fcvt(result, ndig, &dec, &sign);
	n = atoi(str);
	return(n);
}



/* ===========================  Tempo  ===========================*/
void Tempo(double number)
{
	double fraction, hour, minute, second;

	number = number / 3600.0;
	fraction = modf(number,&hour);
	fraction = fraction * 60.0;
    fraction = modf(fraction,&minute);
    second   = fraction * 60.0;
	printf("\nTempo total de processo  ===> %2.0fh : %2.0fm : %4.2fs\n",hour,
		     minute, second);
	return;
}  


/* ===========================  main  ===========================*/
void main(void)
{
  double temp;
  long time_init, time_fim;
  
  time_init=clock();

  printf("===============Calculo de Bacias de Atracao============\n");
  printf("==================Metodo  da Forca Bruta===============\n\n\n");
  NewData(  );

  //printf("entrei em Cells\n");
  CellsTrajec(  );

  time_fim=clock()-time_init;
  temp = (double)time_fim/CLK_TCK;
  Tempo(temp);
 
}

