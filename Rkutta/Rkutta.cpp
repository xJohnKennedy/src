
/* 
  RUNGE - KUTTA 4ta ordem

  Algoritmo para solucaqo de sistemas de eqauacoes diferenciais de primeira
  ordem, utiliza o metodo de RUNGE - KUTTA de quarta ordem (ver livro Griffits).

  Para rodar cada caso eh necessario modificar no codigo a rotina FUNC escrevendo
  o sistema a ser resolvido. 
  y eh o vetor (ex, de deslocamentos e velocidades).
  F eh o vetor de daca equacao 
  t eh o tempo do sistema
  Wf eh a frequaencia do sistema.

  En necessario mudar tambem nas declaracoes o valor de Neq e o valor da dimensao de
  Xo[  ]  que deve ser igual ao numero de equacoes Neq.
  que indica o numero de equacoes a serem resolvidas pelo sistema.

  No arqivo de leitura eh preciso colocar:
  Os valores iniciais dos deslocamentos e velocidades.
  A frequencia da forca exitadora (se o sistema for autonomo, colocar 1.0, mas
  esse valor nao sera utilizado.
  O valor do tempo total Tmax onde o sistema sera integrado.

 */

#define _RKUTTA_C

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

 /******************Declaracoes principais ******************/
#include "Kutta_header_config.h"

double Wf, Gamma1, Xo[Nequ], Yo, Tf, fase;
double NC1, NC2, Step, Tmax;
int N, TimeDirection, Fluido;
double eta__1, eta__2, PL;
double PL_8C, PL_8S, PL_9C, PL_9S;

static char file_read[20], file_write[20];
static void Runge_Kutta(double *y);
static void Func(double *F, double *y, double t);

// variaveis de controle do modo de leitura
int modo_leitura = 0;

/*=========================  Energia =========================*/
double Energia(double *q)
{
	double ener, MapleGenVar1;

	MapleGenVar1 = 0.1630548083E-13 * pow(q[0], 11.0) + 0.1641490063E-12 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] - 0.3018213435E-17 * pow(q[0], 11.0) * q[1] * q[1] +
				   0.7982522165E-22 * pow(q[0], 28.0) + 0.2376379085E2 * q[0] * q[0] + 0.680810316E-15 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[1] * q[1] + 0.5392194389 * q[0] * q[0] * q[0] * q[0] - 0.2092179494E-8 * q[0] * q[0] * q[0] - 0.2746820957 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] +
				   0.1934730471E-9 * q[0] * q[0] * q[0] * q[0] * q[0] - 0.4434130223E-10 * pow(q[0], 18.0) +
				   0.4127131626E-8 * pow(q[0], 16.0) - 0.1038432528E-10 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] + 0.3952637072E2 * q[1] * q[1] + 0.1804997472E-4 * pow(q[0], 12.0) + 0.2004146219E-1 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] - 0.7487117755E-3 * pow(q[0], 10.0) +
				   0.3996304739E-12 * pow(q[0], 20.0) - 0.8635084537E-25 * pow(q[0], 23.0) +
				   0.1158550705E-27 * pow(q[0], 25.0);
	ener = MapleGenVar1 + 0.1968007156E-31 * pow(q[0], 27.0) - 0.2936197616E-14 * pow(q[0], 22.0) + 0.1582063466E-16 * pow(q[0], 24.0) - 0.5759667901E-13 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[1] * q[1] + 0.9073340007E-10 * q[0] * q[0] * q[0] * q[1] * q[1] - 0.3115906498E-6 * pow(q[0], 14.0) + 0.580287204E-12 * q[0] * q[0] * q[0] * q[0] * q[0] * q[1] * q[1] - 0.1494505046E-15 * pow(q[0], 13.0) - 0.1305366953E-16 * pow(q[0], 15.0) +
		   0.2090200076E-8 * pow(q[0], 12.0) * q[1] * q[1] + 0.3673920006E-18 * pow(q[0], 17.0) - 0.428015317E-20 * pow(q[0], 19.0) - 0.129361099E-2 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[1] * q[1] + 0.2642450569E-22 * pow(q[0], 21.0) + 0.4413792996E-4 * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[0] * q[1] * q[1] - 0.5283133452E-19 * pow(q[0], 26.0) - 0.4819277147E-6 * pow(q[0], 10.0) * q[1] * q[1] + 0.138867939E-1 * q[0] * q[0] * q[0] * q[0] * q[1] * q[1] - 0.3066493493E-8 * q[0] * q[1] * q[1] + 0.12561619E1 * q[0] * q[0] * q[1] * q[1];

	return (ener);
}

/*===========================  NewData  ===========================*/

void NewData(void)
{
	FILE *fdread;
	int i;

	/* Get the new file name from the user and open it. */
	// printf( " Entre com o nome do arquivo de leitura: " );
	// gets( file_read );

	fdread = fopen("kutta.dat", "r");
	if (fdread == NULL)
	{
		printf("\n Nao foi possivel abrir arquivo de leitura!\n");
		exit(0);
		return;
	}

	rewind(fdread);
	for (i = 0; i < Nequ; i++)
		fscanf(fdread, "%lf  ", &Xo[i]);
	fscanf(fdread, "\n");
	fscanf(fdread, "%lf", &Wf);
	fscanf(fdread, "%lf %lf", &eta__1, &eta__2);
	fscanf(fdread, "%lf %lf %lf %lf %lf\n", &PL_8C, &PL_8S, &PL_9C, &PL_9S, &PL);
	fscanf(fdread, "%lf %d %d\n", &Tmax, &TimeDirection, &Fluido);

	Tf = 2 * Pi / Wf;
	Step = Tf / Ndiv;
	fase = fase * Pi / 180;
	//Step = 0.001;
	fclose(fdread);
	return;
}

/*===========================  Func  ===========================*/
void Func(double *F, double *y, double t)
{
	double omega;
	omega = Wf;

#include "arquivo_equacoes_exp4.h"

	return;

	return;
}

/*===========================  Runge_Kutta  ===========================*/
static void Runge_Kutta(double *y)
{

	FILE *fdwrite, *fd;

	double y0[Nequ], k0[Nequ], k1[Nequ], k2[Nequ], k3[Nequ], t;
	double ener_sela, Ponto_sela[2];
	int i, count;

	fdwrite = fopen("kutta.txt", "w");
	if (fdwrite == NULL)
	{
		printf("\n Nao foi possivel abrir arquivo de saida!\n");
		exit(0);
		return;
	}

	fd = fopen("poincare.txt", "w");
	if (fd == NULL)
	{
		printf("\n Nao foi possivel abrir arquivo de saida!\n");
		exit(0);
		return;
	}

	for (i = 1; i < Nequ; i++)
	{
		k0[i] = 0.0;
		k1[i] = 0.0;
		k2[i] = 0.0;
		k3[i] = 0.0;
	}

	/* Se a integra��o eh tempo crescente TimeDirection = 1   
     Se a integra��o eh tempo decrescente TimeDirection = -1   */
	if (TimeDirection == -1)
	{
		Step = -Step;
	}

	t = 0.0;
	count = 1;

	//Ponto_sela[0] = 3.15;
	//Ponto_sela[1] = 0.00;
	//Ponto_sela[2] = -0.81;
	//Ponto_sela[3] = 0.00;
	//ener_sela=Energia(Ponto_sela);

	/* Integra��o no tempo */
	while (fabs(t) >= 0.0 && fabs(t) <= fabs(Tmax))
	{
		Func(k0, y, t);
		for (i = 0; i < Nequ; i++)
		{
			y0[i] = y[i];
			y[i] = y0[i] + k0[i] * 0.5 * Step;
		}
		t = t + Step * 0.5;
		Func(k1, y, t);
		for (i = 0; i < Nequ; i++)
		{
			y[i] = y0[i] + k1[i] * 0.5 * Step;
		}
		Func(k2, y, t);
		for (i = 0; i < Nequ; i++)
		{
			y[i] = y0[i] + k2[i] * Step;
		}
		t = t + Step * 0.5;
		Func(k3, y, t);
		for (i = 0; i < Nequ; i++)
		{
			y[i] = y0[i] + (k0[i] + 2.0 * (k1[i] + k2[i]) + k3[i]) / 6.0 * Step;
		}

		/*  Imprime resposta no tempo  */
		fprintf(fdwrite, "%lf ", t);
		for (i = 0; i < Nequ; i++)
		{
			fprintf(fdwrite, ",%25.20lf ", y[i]);
		}
		//fprintf(fdwrite,"%25.20lf %25.20lf",ener_sela,Energia(y));

		fprintf(fdwrite, "\n");

		/*  Imprime secao de Poincare  */
		if (count == Ndiv) //fmod(t,Tf)<=1.0e-5) {
		{
			fprintf(fd, "%lf ", t);
			for (i = 0; i < Nequ; i++)
				fprintf(fd, ",%25.20lf ", y[i]);
			fprintf(fd, "\n");
			count = 0;
		}
		count++;
	}
	fclose(fdwrite);
	fclose(fd);
	return;
}

/*===========================  MAIN  ===========================*/
int main(void)
{
	double valor_condicao_inicial = 0;
	int linha_poincare = 0;
	int linha_force = 0;
	int id_eq = 0;

	printf("\n Escolha o modo de leitura do arquivo Kutta.dat :\n\n");
	printf("\t [1]:leitura direta do arquivo Kutta.dat\n");
	printf("\t [2]:leitura do arquivo Kutta.dat e definicao das condicoes inicias dada pelo user\n");
	printf("\t [3]:leitura do arquivo Kutta.dat e definicao das condicoes inicias dada pela leitura do arquivo poincare.txt\n");
	printf("\t [4]:leitura do arquivo Kutta.dat e definicao das condicoes inicias dada pela leitura do arquivo force.txt\n");
	printf("-------------------------------------------------------------\n");
	printf(" Escolha uma opcao : ");
	scanf("%d", &modo_leitura);

	NewData();

	if (modo_leitura == 2)
	{
		printf("\n\n Entre com o valor da condicao inicial : ");
		scanf("%lf", &valor_condicao_inicial);
		for (int id_eq = 0; id_eq < Nequ; id_eq++)
		{
			Xo[id_eq] = valor_condicao_inicial;
		}
	}	else if (modo_leitura == 3)
	{
		printf("\n\n Entre com o valor da linha a ser lida no arquivo poincare.txt : ");
		scanf("%d", &linha_poincare);

		FILE *fd_poincare;
		char str[256];		
		int linha_countador = 0;
		double lixo;

		fd_poincare = fopen("poincare.txt", "r");
		if (fd_poincare == NULL)
		{
			printf("\n Nao foi possivel abrir arquivo de saida!\n");
			exit(0);
			return 0;
		}
		// testa se chegou no fim do arquivo
		while (!feof(fd_poincare))
		{
			linha_countador++;
			if (linha_countador != linha_poincare) // não chegou na linha desejada
			{
				while ( getc(fd_poincare) != 10) 
				// le todos os caracteres ate encontrar um salto de linha "\n"
				// que na tabela ASCII tem valor 10
				{
					
				}
			} else {
				fscanf(fd_poincare, "%lf ,", &lixo);
				for (id_eq =0 ; id_eq < (Nequ-1); id_eq++)
					fscanf(fd_poincare, "%lf ,", &Xo[id_eq]);
				fscanf(fd_poincare, "%lf \n", &Xo[id_eq++]);
				break;
			}

		}
		fclose(fd_poincare);
	} else if (modo_leitura == 4)
	{
		printf("\n\n Entre com o valor da linha a ser lida no arquivo force.txt : ");
		scanf("%d", &linha_force);

		FILE *fd_force;
		char str[256];		
		int linha_countador = 0;
		double lixo;
		int lixo_d;

		fd_force = fopen("force.txt", "r");
		if (fd_force == NULL)
		{
			printf("\n Nao foi possivel abrir arquivo de saida!\n");
			exit(0);
			return 0;
		}
		// testa se chegou no fim do arquivo
		while (!feof(fd_force))
		{
			linha_countador++;
			if (linha_countador != linha_force) // não chegou na linha desejada
			{
				while ( getc(fd_force) != 10) 
				// le todos os caracteres ate encontrar um salto de linha "\n"
				// que na tabela ASCII tem valor 10
				{
					
				}
			} else {
				fscanf(fd_force, "%lf ,", &lixo);
				fscanf(fd_force, "%lf ,", &Wf);
				for (id_eq =0 ; id_eq < Nequ; id_eq = id_eq + 2)
					fscanf(fd_force, "%lf , %lf , %lf , %lf ,", &Xo[id_eq], &Xo[id_eq + 1], &lixo, &lixo);
				fscanf(fd_force, "%d \n", &lixo_d);
				break;
			}

		}
		fclose(fd_force);
	}

	// imprime dados finais que serao utilizados para o processamento do Runge Kutta
	printf("\n\n Numero de equacoes do sistema =  %d \n", Nequ);
	for (int id_eq = 0; id_eq < Nequ; id_eq++)
	{
		printf(" Xo[%3d] = %25.20lf \n", id_eq, Xo[id_eq]);
	}
	printf(" Wf      = %25.20lf \n", Wf);
	printf(" eta_1   = %25.20lf \n eta_2   = %25.20lf \n", eta__1, eta__2);
	printf(" PL_8C   = %25.20lf \n PL_8S   = %25.20lf \n PL_9C   = %25.20lf \n PL_9S   = %25.20lf \n", 
					PL_8C, PL_8S, PL_9C, PL_9S);
	printf(" PL      = %25.20lf \n", PL);
	printf(" Tmax    = %25.5lf \n", Tmax);

	printf("\n\n PRESSIONE PARA SALVAR Kutta.dat E INICIAR O RUNGE KUTTA.....\n");
	system("pause");

	//salva Kutta.dat
	if (1) //sempre verdadeiro, fiz isso para declarar variaveis dentro deste escopo
	{
		FILE *fd_kutta;

		fd_kutta = fopen("Kutta.dat", "w");
		if (fd_kutta == NULL)
		{
			printf("\n Nao foi possivel abrir arquivo de saida!\n");
			exit(0);
			return 0;
		}
		
		for (id_eq =0 ; id_eq < (Nequ-1); id_eq++)
			fprintf(fd_kutta, "\t %25.20lf", Xo[id_eq]);
		fprintf(fd_kutta, "\t %25.20lf \n", Xo[id_eq++]);
		fprintf(fd_kutta, "\t %lf \n", Wf);
		fprintf(fd_kutta, "\t %lf \t %lf ", eta__1, eta__2);
		fprintf(fd_kutta, "\t %lf \t %lf \t %lf \t %lf \t %lf \n", 
					PL_8C, PL_8S, PL_9C, PL_9S, PL);
		fprintf(fd_kutta, "\t %lf \t %d \t %d \n", Tmax, TimeDirection, Fluido);
		fclose(fd_kutta);		

	}


	Runge_Kutta(Xo);
	return 0;
}
