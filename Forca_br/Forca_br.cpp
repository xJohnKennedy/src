/*    
FORCA BRUTA

  Tirado da xerox de Nayfeh.
  Algoritmo que calcula diagrmas de bifurcacao de um sistema de equacoes diferenciais,
  para cada caso eh necessario mudar as rotinas:
  A partir de uma condicao inicial integra-se o sistema utilizando o metodo de
  Runge - Kutta de quarta ordem.
  O criterio eh integrar a equacao por um munero de iteracoes Nt (considerado transiente)
  onde os dados gerados nao sao levados em conta, seguidamente continua-se
  com a integracao do sistema por um numero de interacoes Nss, considerando que
  o sistema ja esta no estado permanente, em cada integracao sao guardados em arquivo.
  logo:
  Nt  = numero de iteracoes do transiente.
  Nss = numero de iteracoes do estado permanente.

  FUNC: Rotina onde sera escrito o sistema de equacoes, o parametro de
  variacao eh o alpha.

  Eh necessario mudar tambem (para cada caso) o valor de Neq, que define o mumero
  de equacoes a serem resolvidas.
   
	 No arquivo FORCA.dat eh necessario ingressar com:
	 As condicoes iniciais;
	 O valor de alphamin (valor minimo do parametro de controle)  e alphamax
	 (valor maximo do parametro de controle);
	 O valor de K, que eh o numero de vezes que o parametro sera dividido.
	 A variavel id foi colocada para imprimir um determinado valor na solucao, mas
	 pode adotar qq valor porque nao eh utilisada no programa.
	 O valor de Wf que eh a frequencia da forca excitador num sistema nao autonomo.

  Quando analisa-se um sistema sem forca excitadora, pode-se colocar qualquer valor a Wf,
  mas devera mudar-se o valor da variavel Step, colocando-se o passo de tempo,
  desejado para a integracao.

  A saida dos resultados eh feita no arquivo FORCE.TXT

 */


# define _FORCA_BR_C

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io.h>
#include <ctime>

/***************  Definicao de numero de equacoes  ********************/
#include "_config_modelo/Nequ_config.h"
/**********************************************************************/

double  xo[Nequ], x[Nequ], x_old[Nequ], y_max[Nequ];
double  Tf, Wf, Step;
double  alphamin, alphamax;
int     K, Nt, Nss, Param_freq;
double eta__1, eta__2, PL;
double PL_8C, PL_8S, PL_9C, PL_9S;

double  Pi   = 3.141592654;
double  erro = 1.0e-5;
int     Ndiv = 250;

static char file_read[20];
static void Runge_Kutta(double *y, double alpha);
static void Func(double *F, double *y, double t, double Parametro);


/*===========================  NewData  ===========================*/

void NewData( void )
{
 

 FILE    *fdread;
 int i ;

/* Get the new file name from the user and open it. 
 printf( " Entre com o nome do arquivo de leitura: " );
 gets( file_read );  */

 fdread = fopen( "forca.dat", "r" );
 if( fdread == NULL ) {
  printf( "\n Nao foi possivel abrir arquivo de leitura!\n" );
  return;
 }
 rewind(fdread);
 for (i=0;i<Nequ;i++)
	 fscanf( fdread,"%lf", &xo[i] );
 fscanf( fdread,"\n");
 fscanf( fdread,"%lf %lf\n", &alphamin, &alphamax );
 fscanf( fdread,"%d \n", &K );
 fscanf( fdread,"%d %d\n", &Nt, &Nss );
 fscanf( fdread,"%d %lf\n", &Param_freq, &Wf );
 fscanf( fdread,"%lf %lf", &eta__1, &eta__2 );
 fscanf( fdread,"%lf %lf %lf %lf %lf\n", &PL_8C, &PL_8S, &PL_9C, &PL_9S, &PL );
 fclose( fdread );
 return;
}


/*===========================  Func  ===========================*/
#include "_config_modelo/arquivo_equacoes.h"


/*===========================  Runge_Kutta  ===========================*/
void Runge_Kutta(double *y, double alpha)
{
  double y0[Nequ], k0[Nequ], k1[Nequ], k2[Nequ], k3[Nequ], t;
  int i, j;

  for (i=0; i<Nequ; i++)
  {
	 k0[i] = 0.0;
	 k1[i] = 0.0;
	 k2[i] = 0.0;
	 k3[i] = 0.0;
  }

  t = 0.0;
  j = 1;

	 while (j <= Ndiv)
	 {
		 Func(k0, y, t, alpha);
		 for (i=0; i<Nequ; i++)
		 {
			 y0[i] = y[i];
			 y[i] = y0[i] + k0[i]*0.5*Step;
		 }
		 t = t + Step*0.5;
		 Func(k1, y, t, alpha);
		 for (i=0; i<Nequ; i++)
			 y[i] = y0[i] + k1[i]*0.5*Step;
		 Func(k2, y, t, alpha);
		 for (i=0; i<Nequ; i++)
			 y[i] = y0[i] + k2[i]*Step;
		 t = t + 0.5*Step;
		 Func(k3, y, t, alpha);
		 for (i=0; i<Nequ; i++)
			 y[i] = y0[i] + (k0[i]+2.0*(k1[i]+k2[i])+k3[i])/6.0*Step;
		 
		 for (i=0; i<Nequ; i++)
		 {
			 if(fabs(y_max[i])<fabs(y[i]))
			 {
				 y_max[i]=fabs(y[i]);
			 }
		 }

		 
		 j++;
		 
	 }

  return;
}

void hora_atual()
{
	std::time_t result = std::time(nullptr);
	printf("==> %s", std::ctime(&result));
}


/*===========================  MAIN  ===========================*/
void main( void )
{
	int k,i,j, ij, flag, periodo, retorno;
	double alpha, derro;
	double alpha_final;
	FILE *fd, *fd_log;

	// buffer de armazenamento  do output
	// 30 caracteres * (2 * Nequ + 4) * (21 linhas)
	const int tamanho_buffer = 30 * (2 * Nequ + 4) * 21;
	char buffer_temp[tamanho_buffer];
	int cx = 0; // numero de caracteres gravados no buffe temporario


	fd=fopen("force.txt","w");
	fd_log = fopen("force_log.txt", "w");

	NewData( );
	printf("\n\nSai de New data\n");
	printf("Calculando forca bruta \n");

	for(i=0;i<Nequ;i++)
		x[i]=xo[i];
		
	for(k=1;k<=K;k++)
	{
		hora_atual();
		printf("Ponto: %d  / %d  \n",k,K);

		/* Perturbacao para cada iteracao  */
		for(i=0;i<Nequ;i++){
			x[i]=x[i]+1e-6;
			y_max[i] = 0;
		}

		/* Calculo do parametro de controle */
		alpha = alphamin + (k-1)*(alphamax - alphamin) / (K - 1);
		printf("alpha: %.6f \r", alpha);

		int num_iter = 0;
		/* Imprime ponto inicial  de cada iteracao para possivel reproducao do passo com runge kutta  */
		fprintf(fd_log, "%d,  %10.6e,  ", k, alpha);
		for (j = 0; j < Nequ; j = j + 2)
			fprintf(fd_log, "%16.12e,  %16.12e,  %16.12e,  %16.12e,  ", x[j], x[j + 1], y_max[j], y_max[j + 1]);
		fprintf(fd_log, "1 , %d\n", num_iter);

		/* Calculo do periodo do sistema quando o parametro eh a frequencia ==> Param_freq = 1 */
		//parametro de controle é a frequencia Wf entao Param_freq == 1
		//parametro de controle é a carga PL entao  Param_freq == 2
		if(Param_freq == 1) 
		{
			Wf = alpha;   
			Tf = 2*Pi / Wf;
			alpha_final = alpha;
			//printf("Parametro de controle e a frequencia! \n");
		}
		else
		{
			Tf = 2 * Pi / Wf;

			if (Param_freq == 2)
			{
				// como o runge kutta foi escrito tomando como base que o parametro de controle é a frequencia
				// faz-se necessario reorganizar as variaveis globais que estao sendo alteradas neste caso o PL
				PL = alpha;
				alpha_final = PL;
				alpha = Wf;
				//printf("Parametro de controle e a carga PL! \n");
			}
			else 
			{
				printf("\n Nao foi possivel reconhecer o parametro de controle (%d) lido!\n", Param_freq);
				return;
			}
		}
		Step = Tf / Ndiv;   

		int flag_orbita = 1;
		int flag_Nt = 1;
		periodo = 1;

		/* Iteracoes para eliminar a parte transiente  */
		// Regras de saída do while-loop
		// flag_orbita = 0			-> encontrou uma órbita fechada
		// ou
		// flag_Nt = 0				-> excedeu o número máximo de iteracoes
		while (flag_orbita != 0 && flag_Nt != 0)
		{
			cx = 0; // seta cx para o inicio do buffer_temporario

			for (i = 0 + periodo; i <= Nt_temp; i++)
				Runge_Kutta(x, alpha);
			
			num_iter = num_iter + i - 1;
			if (num_iter >= Nt)
			{
				// sai do while-loop na proxima iteracao pois estourou o limite Nt
				flag_Nt = 0;
			}

			/* Guarda valor final de x */
			for (ij = 0; ij < Nequ; ij++)
				x_old[ij] = x[ij];


			/* Integra para calcular a periodicidade da resposta  */
			periodo = 1;
			flag = 1;

			// zera y_max para armazenar valor maximo da orbita apos o periodo transiente
			for (i = 0; i < Nequ; i++) {
				y_max[i] = 0;
			}

			while (flag)
			{
				/* Calcula mais um ponto de integracao  */
				Runge_Kutta(x, alpha);

				/* Avalia se o ponto x calculado � igual a x_old */
				retorno = 0;
				for (ij = 0; ij < Nequ; ij++)
				{
					derro = fabs(x[ij] - x_old[ij]);
					if (derro > 1.0e-5)
						retorno++;
				}

				/* Se nao � igual retorna a uma nova integracao, caso contrario sai do loop do while */
				if (retorno != 0)
				{

					/* Imprime ponto fixo calculado  */
					cx = sprintf(buffer_temp + cx, "%d,  %10.6e,  ", k, alpha_final) + cx;
					for (j = 0; j < Nequ; j = j + 2)
						cx = sprintf(buffer_temp + cx, "%16.12e,  %16.12e,  %16.12e,  %16.12e,  ", x[j], x[j + 1], y_max[j], y_max[j + 1]) + cx;
					cx = sprintf(buffer_temp + cx, "%d, %d", periodo, num_iter + periodo) + cx;
					cx = sprintf(buffer_temp + cx, "\n") + cx;

					periodo++;
				}
				else
				{
					flag = 0;
					flag_orbita = 0;

					/* Imprime ponto fixo calculado  */
					cx = sprintf(buffer_temp + cx, "%d,  %10.6e,  ", k, alpha_final) + cx;
					for (j = 0; j < Nequ; j = j + 2)
						cx = sprintf(buffer_temp + cx, "%16.12e,  %16.12e,  %16.12e,  %16.12e,  ", x[j], x[j + 1], y_max[j], y_max[j + 1]) + cx;

					cx = sprintf(buffer_temp + cx, "%d, %d", periodo, num_iter + periodo) + cx;
					cx = sprintf(buffer_temp + cx, "\n") + cx;
				}


				/* Avalia limte de periodicidade */
				if (periodo > Nss)
				{
					flag = 0;
				}
			}
		}

		fputs(buffer_temp,fd);
	}
	hora_atual();
	fclose(fd);
	return;
}


