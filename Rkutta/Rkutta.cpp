
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
#include <chrono>

 /******************Declaracoes principais ******************/
#include "_config_modelo/Kutta_header_config.h"

double Wf, Gamma1, Xo[Nequ], Yo, Tf, fase;
double NC1, NC2, Step, Tmax;
int N, TimeDirection, Fluido;
double eta__1, eta__2, PL;
double PL_8C, PL_8S, PL_9C, PL_9S;

static char file_read[20], file_write[20];
static void Runge_Kutta(double *y);
static void Func(double *F, double *y, double t, double Parametro);

// variaveis de controle do modo de leitura
int modo_leitura = 0;

/*=========================  Energia =========================*/

/* ===========================  Tempo  ===========================*/
struct Tempo
{
	std::chrono::time_point<std::chrono::steady_clock> inicio, fim;
	std::chrono::duration<double> duracao;

public:
	//constroi objeto
	Tempo()
	{
		inicio = std::chrono::high_resolution_clock::now();
	}
	// destroi objeto
	~Tempo()
	{
		fim = std::chrono::high_resolution_clock::now();
		duracao = fim - inicio;

		printf("\n->>Tempo de calculo do Runge Kutta : %f s\n", duracao);
	}
};


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
#include "_config_modelo/arquivo_equacoes.h"

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
		Func(k0, y, t, Wf);
		for (i = 0; i < Nequ; i++)
		{
			y0[i] = y[i];
			y[i] = y0[i] + k0[i] * 0.5 * Step;
		}
		t = t + Step * 0.5;
		Func(k1, y, t, Wf);
		for (i = 0; i < Nequ; i++)
		{
			y[i] = y0[i] + k1[i] * 0.5 * Step;
		}
		Func(k2, y, t, Wf);
		for (i = 0; i < Nequ; i++)
		{
			y[i] = y0[i] + k2[i] * Step;
		}
		t = t + Step * 0.5;
		Func(k3, y, t, Wf);
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
int main(int argc, char** argv)
{
	double valor_condicao_inicial = 0;
	int linha_poincare = 0;
	int linha_force = 0;
	int id_eq = 0;

	printf("\n Escolha o modo de leitura do arquivo Kutta.dat :\n\n");
	printf("\t [1]:leitura direta do arquivo Kutta.dat\n");
	printf("\t [2]:leitura do Kutta.dat e definicao das condicoes inicias dada pelo user\n");
	printf("\t [3]: ... definicao das condicoes inicias dada pela leitura do arquivo poincare.txt\n");
	printf("\t [4]: ... definicao das condicoes inicias dada pela leitura do arquivo force_log.txt\n");
	printf("\t [5]: ... definicao das condicoes inicias dada pela leitura do arquivo force.txt\n");
	printf("\t [6]: ... definicao das condicoes inicias dada pela leitura do arquivo bacia_results.txt\n");
	printf("-------------------------------------------------------------\n");
	printf(" Escolha uma opcao : ");
	if (argc == 1)
	{
		scanf("%d", &modo_leitura);
	}
	else
	{
		//extrai de argv o argumento do modo de leitura das condicoes inicias
		sscanf(argv[1], "%d", &modo_leitura);
	}

	NewData();

	if (modo_leitura == 2)
	{
		printf("\n\n Entre com o valor da condicao inicial : ");
		if (argc > 2)
		{
			sscanf(argv[2], "%lf", &valor_condicao_inicial);
			printf("%lf\n", valor_condicao_inicial);
		}
		else {
			scanf("%lf", &valor_condicao_inicial);
		}
		
		for (int id_eq = 0; id_eq < Nequ; id_eq++)
		{
			Xo[id_eq] = valor_condicao_inicial;
		}
	}
	else if (modo_leitura == 3)
	{
		printf("\n\n Entre com o valor da linha a ser lida no arquivo poincare.txt : ");
		if (argc > 2)
		{
			sscanf(argv[2], "%d", &linha_poincare);
			printf("%d\n", linha_poincare);
		}
		else {
			scanf("%d", &linha_poincare);
		}

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
	} 
	else if (modo_leitura == 4)
	{
		printf("\n\n Entre com o valor da linha a ser lida no arquivo force_log.txt : ");
		if (argc > 2)
		{
			sscanf(argv[2], "%d", &linha_force);
			printf("%d\n", linha_force);
		}
		else {
			scanf("%d", &linha_force);
		}

		FILE *fd_force;
		char str[256];		
		int linha_countador = 0;
		double lixo;
		int lixo_d;

		fd_force = fopen("force_log.txt", "r");
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
	else if (modo_leitura == 5)
	{
		printf("\n\n Entre com o valor da linha a ser lida no arquivo force.txt : ");
		if (argc > 2)
		{
			sscanf(argv[2], "%d", &linha_force);
			printf("%d\n", linha_force);
		}
		else {
			scanf("%d", &linha_force);
		}

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
				while (getc(fd_force) != 10)
					// le todos os caracteres ate encontrar um salto de linha "\n"
					// que na tabela ASCII tem valor 10
				{

				}
			}
			else {
				fscanf(fd_force, "%lf ,", &lixo);
				fscanf(fd_force, "%lf ,", &Wf);
				for (id_eq = 0; id_eq < Nequ; id_eq = id_eq + 2)
					fscanf(fd_force, "%lf , %lf , %lf , %lf ,", &Xo[id_eq], &Xo[id_eq + 1], &lixo, &lixo);
				fscanf(fd_force, "%d \n", &lixo_d);
				break;
			}

		}
		fclose(fd_force);
	}
	else if (modo_leitura == 6)
	{
	printf("\n\n Entre com o valor da linha a ser lida no arquivo bacia_results.txt : ");
	if (argc > 2)
	{
		sscanf(argv[2], "%d", &linha_force);
		printf("%d\n", linha_force);
	}
	else {
		scanf("%d", &linha_force);
	}

	FILE *fd_bacia;
	char str[256];
	int linha_countador = 0;
	double lixo;
	int lixo_d;

	fd_bacia = fopen("bacia_results.txt", "r");
	if (fd_bacia == NULL)
	{
		printf("\n Nao foi possivel abrir arquivo de saida!\n");
		exit(0);
		return 0;
	}
	// testa se chegou no fim do arquivo
	while (!feof(fd_bacia))
	{
		linha_countador++;
		if (linha_countador != linha_force) // não chegou na linha desejada
		{
			while (getc(fd_bacia) != 10)
				// le todos os caracteres ate encontrar um salto de linha "\n"
				// que na tabela ASCII tem valor 10
			{

			}
		}
		else {
			fscanf(fd_bacia, "%d ,", &lixo);
			fscanf(fd_bacia, "%lf , %lf , %lf , %lf ,", &lixo, &lixo, &lixo, &lixo);
			for (id_eq = 0; id_eq < Nequ; id_eq = id_eq + 2)
				fscanf(fd_bacia, "%lf , %lf ,", &Xo[id_eq], &Xo[id_eq + 1]);
			fscanf(fd_bacia, "%d \n", &lixo_d);
			break;
		}

	}
	fclose(fd_bacia);
	}

	// atualiza o periodo e Step caso nas leituras do force_log.txt ou poincare.txt
	// o usuario ler um ponto fixo com Wf diferente do existente em Kutta.dat
	Tf = 2 * Pi / Wf;
	Step = Tf / Ndiv;

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
	printf(" Periodo = %25.5lf \n", Tf);
	printf(" Step    = %25.5lf \n", Step);

	if (argc < 2)
	{
		printf("\n\n PRESSIONE PARA SALVAR Kutta.dat E INICIAR O RUNGE KUTTA.....\n");
		system("pause");
	}

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

	{
		Tempo timer;
		Runge_Kutta(Xo);
	}
	return 0;
}
