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

/***************  Definicao de numero de equacoes  ********************/
#include "Nequ_config.h"
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
void Func(double *F, double *y, double t, double Parametro)
{
	double  omega;
	omega = Wf;

	#include "arquivo_equacoes_exp4.h"

	return;

}


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


/*===========================  MAIN  ===========================*/
void main( void )
{
	int k,i,j, ij, flag, periodo, retorno;
	double alpha, derro;
	FILE *fd;

	fd=fopen("force.txt","w");

	NewData( );
	printf("\n\nSai de New data\n");
	printf("Calculando forca bruta \n");

	for(i=0;i<Nequ;i++)
		x[i]=xo[i];
		
	for(k=1;k<=K;k++)
	{
		printf("%d  / %d  \r",k,K);

		/* Perturbacao para cada iteracao  */
		for(i=0;i<Nequ;i++){
			x[i]=x[i]+1e-6;
			y_max[i]=0;
		}

		/* Calculo do parametro de controle */
		alpha = alphamin + (k-1)*(alphamax - alphamin)/(K-1);

		/* Calculo do periodo do sistema quando o parametro eh a frequencia ==> Param_freq = 1 */
		if(Param_freq) 
		{
			Wf = alpha;   
			Tf = 2*Pi / Wf;
		}
		else
		{
			Tf = 2*Pi / Wf;
		}
		Step = Tf / Ndiv;   

		/* Iteracoes para eliminar a parte transiente  */
		for(i=1;i<=Nt;i++)
			Runge_Kutta(x, alpha);

		/* Guarda valor final de x */
		for(ij=0; ij<Nequ; ij++)
			x_old[ij] = x[ij];

		/* Imprime ponto fixo calculado  */
		fprintf(fd,"%d,  %10.6e,  ",k,alpha);
		for(j=0;j<Nequ;j=j+2)
			fprintf(fd,"%16.12e,  %16.12e,  %16.12e,  %16.12e,  ", x_old[j] ,x_old[j+1], y_max[j], y_max[j+1]);
		fprintf(fd,"1 \n");

		/* Integra para calcular a periodicidade da resposta  */
		periodo = 1;
		flag = 1;
		
		while(flag)
		{
			/* Calcula mais um ponto de integracao  */
			Runge_Kutta(x, alpha);

			/* Avalia se o ponto x calculado � igual a x_old */
			retorno = 0;
			for(ij=0; ij<Nequ; ij++)
			{
				derro = fabs(x[ij]-x_old[ij]);
				if(derro>1.0e-5)
					retorno++;
			}

			/* Se nao � igual retorna a uma nova integracao, caso contrario sai do loop do while */
			if(retorno != 0)
			{
				periodo++;
				
				/* Imprime ponto fixo calculado  */
				fprintf(fd,"%d,  %10.6e,  ",k,alpha);
				for(j=0;j<Nequ;j=j+2)
					fprintf(fd,"%16.12e,  %16.12e,  %16.12e,  %16.12e,  ", x[j], x[j+1], y_max[j], y_max[j+1]);
				fprintf(fd,"%d",periodo);
				fprintf(fd,"\n");
			}
			else
			{
				flag = 0;
			}

		
			/* Avalia limte de periodicidade */
			if(periodo>Nss)
				flag = 0;
		}
	}
	fclose(fd);
	return;
}


