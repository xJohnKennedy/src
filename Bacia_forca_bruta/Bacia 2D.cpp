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
#include "bacia_dinamica.h"

double G0, G1;
double Y1min,Y1max,Y2min,Y2max;
int Num_cel,Cor1,Cor2;
double *q1,*q1p,*q2,*q2p;
double k1[2], k2[2], k3[2], k4[2], g1[2], g2[2], g3[2], g4[2]; //MODIFICAR

/*===========================  Func  ===========================*/
void Func(double *F, double *y, double t)
{
		double  beta, xi, C1, alpha, delta;
 C1=136100; alpha=0.05922; delta=1.15;
beta=G1;  xi=0.01;

 /*MR2 delta=1.2 */
F[0] = y[1];
F[1] = -0.25E3*(0.4E-2*alpha*C1*pow(delta+y[0],3.0)-0.4E-2*alpha*C1/pow(
delta+y[0],3.0)+0.4E-2*C1*(delta+y[0])-0.4E-2*C1/pow(delta+y[0],5.0)-0.4E-2*C1/
(delta*delta*delta*delta*delta*delta*delta)*(alpha*(delta*delta*delta*delta*
delta*delta*delta*delta-1.0*delta*delta)+delta*delta*delta*delta*delta*delta
-1.0)*(1.0+beta*cos(Wf*t))*pow(delta+y[0],2.0)+0.8000000004E-2*sqrt(C1*(
alpha*(delta*delta+5.0/(delta*delta*delta*delta))+7.0/(delta*delta*delta*delta*
delta*delta)-1.0))*sqrt(C1*(0.6E1*alpha+0.6E1))*xi*y[1])/C1/(alpha*(delta*delta
+5.0/(delta*delta*delta*delta))+7.0/(delta*delta*delta*delta*delta*delta)-1.0);
	
	return;
		
}


/*===========================  Runge_Kutta  ===========================*/
int Runge_Kutta(double *y, double *x, double Step, int Total)
{
  
  int i, j;
  double t1, t2, t3, t4, t;
  int count;

  /*y0 = (double*) calloc(M,sizeof(double));
  y1 = (double*) calloc(M,sizeof(double));
  k1 = (double*) calloc(M,sizeof(double));
  k2 = (double*) calloc(M,sizeof(double));
  k3 = (double*) calloc(M,sizeof(double));
  k4 = (double*) calloc(M,sizeof(double));
  g1 = (double*) calloc(M,sizeof(double));
  g2 = (double*) calloc(M,sizeof(double));
  g3 = (double*) calloc(M,sizeof(double));
  g4 = (double*) calloc(M,sizeof(double));*/

  for (i=0; i<=M-1; i++)
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

  for(i=0; i<=M-1; i++)
	  y[i] = x[i];
 
  t = 0.0;
  j = 1;

  /* Integracao de Runge-Kutta para um periodo */
  while (j <= Total)
  {
	  t1 = t;
	  for (i=0; i<M; i++)
	  {
		 g1[i] = y[i];
	  }
	  Func(k1, g1, t1);


	  t2 = t + Step*0.5;   
	  for (i=0; i<M; i++)
	  {
		 g2[i] = y[i] + k1[i]*0.5*Step;
	  }
	  Func(k2, g2, t2);

	  t3 = t + Step*0.5;   
	  for (i=0; i<M; i++)
	  {
		 g3[i] = y[i] + k2[i]*0.5*Step;
	  }
	  Func(k3, g3, t3);

  	  t4 = t + Step;   
	  for (i=0; i<M; i++)
	  {
		 g4[i] = y[i] + k3[i]*Step;
	  }
	  Func(k4, g4, t4);


	  for (i=0; i<M; i++)
	  {
		 y[i] = y[i] + (k1[i]+2.0*(k2[i]+k3[i])+k4[i])/6.0*Step;
	  }

	  t = t + Step;
	  j++;
  }
  
  /* Assigna valor final para o vetor y  */
  /*for(i=0;i<=M-1;i++)
		  y[i] = y1[i];
  */
  /*free(y0);
  free(y1);
  free(k1);
  free(k2);
  free(k3);
  free(k4);
  free(g1);
  free(g2);
  free(g3);
  free(g4);*/

  return(1);
 }




/* ===========================  NewData  ===========================*/
void NewData( void )
{
 FILE    *fdread;
 long int i;
 double PI = 3.14159265358979323846;
 double a,b,c,d;

 /* Get the new file name from the user and open it. */
 fdread = fopen( "bacia.dat", "r" );
 if( fdread == NULL ) {
  printf( "\n Nao foi possivel abrir arquivo de leitura!\n" );
  exit(0);
  return;
 }

 rewind(fdread);

 /* Dimension doof space */
 fscanf( fdread,"%d  %d\n", &M, &PeriodoMaximo );  
 
  /* Frequencia e amplitude da forca excitadora */
 fscanf( fdread,"%lf  %lf\n", &Wf, &G1 );

 /* Pre-carregamento estatico e parametros da equação */
 //fscanf( fdread,"%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", &G0,  &epsilon, &lambda, &Q20, &eta, &delta, &psi, &beta, &alpha);

 /* Numero de células */
 fscanf( fdread,"%d\n", &Num_cel);

 /* Coordenadas de analise */
 fscanf( fdread,"%d %d\n", &Cor1,&Cor2);

 /* Periodo da forca excitadora */
 Tf    = 2*PI / Wf;        
 
 /* Passo para integracao no tempo */
 Passo = Tf / 500;        

 /* Alocaao de vetores para o tamanho da celula */
 q1 = (double*)calloc(Num_cel,sizeof(double));
 q1p = (double*)calloc(Num_cel,sizeof(double));
  
 
 /* Valor das coordenadas fixas para a analise */
 for(i=0;i<Num_cel;i++)
 {
	 //fscanf( fdread,"%lf %lf %lf %lf\n", &a, &b, &c, &d );
	 fscanf( fdread,"%lf %lf\n", &a, &b );
	 q1[i]  = a;
	 q1p[i] = b;

 }
 /*Valor dos limites*/
 fscanf(fdread, "%lf %lf %lf %lf\n",&a,&b,&c,&d);
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

  int i, j, retorno, Periodo, PeriodoBack, flag;
  int value, ij, Tempo; 
  long int cellnum;
  double x[2],y[2], y_old[2], xo[2], derro,zo0atr, zo1atr; //MODIFICAR

  /* Abre arquivo de impressao   */
  fd = fopen("select.txt","w");
  if( fd == NULL ) {
	  printf( "\n Nao foi possivel abrir arquivo de ravcells.txt !\n" );
	  exit(0);
	  return;
  }
  
  /* Imprime cabecalho de arquivo de saida */
  /*fprintf(fd,"Numero   q1    q1p   q1atr   q1patr   Tempo   Periodicidade\n");
  */
  if(x==NULL||y==NULL||y_old==NULL)
  {
	  printf("memoria insuficiente \n");
	  exit(0);
  } 

  /* Aloca vetores de cada celula  */
     /* y = (double*) calloc(M,sizeof(double));
	  x = (double*) calloc(M,sizeof(double));
      xo = (double*) calloc(M,sizeof(double));
      y_old = (double*) calloc(M,sizeof(double)); */     


  /* Integracao no tempo para cada celula do espaco  */
  cellnum = 1;
  
  for(i=0;i<Num_cel;i++)
  {
	  /* Contador de Tempo e de periodicidade da solucao */
	  Tempo = 0;
	  Periodo = 0;
	  /* Coordenada de cada celula */
	  x[0]=q1[i];
	  x[1]=q1p[i];
	  /*x[2]=1e-6;
	  x[3]=1e-6;
	  x[4]=1e-4;
	  x[5]=1e-4;
	  x[6]=1e-4;
	  x[7]=1e-4;
	  x[8]=1e-4;
	  x[9]=1e-4;
	  //MODIFICAR
	  /* x[10]=1e-4;
	  x[11]=1e-4;
	  x[12]=1e-4;
	  x[13]=1e-4;
	  x[14]=1e-4;
	  x[15]=1e-4;
	  x[16]=1e-4;
	  x[17]=1e-4;
	  x[18]=1e-4;
	  x[19]=1e-4;
	  x[20]=1e-4;
	  x[21]=1e-4;*/
	  /* Coordenadas iniciais */
	  for(ij=0; ij<=M-1; ij++)
	  {
		  y_old[ij] = x[ij];
		  xo[ij] = x[ij];
		  y[ij] = 0;
	  }
	  flag = 1;


	  while(flag && y[Cor1] < 1e10)
	  {
		  /* Integracao no tempo */ 
		  value = Runge_Kutta( y, x, Passo, 500 );
		  /* Conta numero de periodos de integracao e contador para periodicidade da solucao */
		  Tempo++;
		  Periodo++;
		  /* Avalia se ponto calculado é o mesmo ao inicial  */
		  retorno = 0;
		  for(ij=0; ij<=M-1; ij++)
		  {
			  derro = fabs(y[ij]-y_old[ij]);
			  if(derro>1.0e-8)
				  retorno++;
			  PeriodoBack = Periodo;
		  }
		  /*  Avalia retorno */
		  if(retorno==0)
		  {
			  flag = 0;
			  continue;
		  }
		  /* Reinicia contador de periodos */
		  if(Periodo==PeriodoMaximo)
		  {
			  Periodo = 0;
			  /* Coordenadas iniciais para o novo ponto de analise (vetor x)*/
			  for(ij=0; ij<=M-1; ij++)
				  y_old[ij] = y[ij];
		  }
		  /* Novo valor para vetor x  */
		  for(ij=0; ij<=M-1; ij++)
			  x[ij] = y[ij];
		  /* Limita o numero de integracoes, caso nao exista solucao periodica  */
		  if(Tempo>8000)
			  flag = 0;
	  }
	  /* Celula mapeada (atrator)  */
	  zo0atr = y[Cor1];
	  zo1atr = y[Cor2];
	  
	  printf("%d / %d",i,Num_cel);
	  printf("   %15.12lf %15.12lf %d  %d  \r",y[Cor1],y[Cor2],Tempo,Periodo);
	  /* Imprime resultados do atrator da celula */
	  if(Tempo<8000)
	  {
		  if(Y1min<=y[Cor1])
		  //if(Y2min<=y[Cor1])
		  {
			  if(y[Cor1]<=Y1max)
			  //if(y[Cor1]<=Y2max)
			  {
				  if(y[Cor2]>=Y2min)
				  //if(y[Cor2]>=Y1min)
				  {
					  if(y[Cor2]<=Y2max)
					  //if(y[Cor2]<=Y1max)
					  {
						  fprintf(fd,"%10.7lf %10.7lf %10.7lf %10.7lf\n", xo[Cor1], xo[Cor2], y[Cor1], y[Cor2]);
						  
					  }
				  }
			  }
		  }
	  }
	  cellnum++;
	  
  }
  /*free(x);
  free(y_old);
  free(xo); 
  free(y);*/
  fclose(fd);
  return;
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

