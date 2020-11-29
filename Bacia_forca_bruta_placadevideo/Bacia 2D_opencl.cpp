/* PROGRAMA Bac_Din_o1.CPP
   
  Programa para o calculo da bacia de atracao de um sistema de equacoes de moviemnto.

  Utiliza o metodo da forca bruta para calcular o atrator de cada ponto.

  Para cada ponto, vai integrando cada periodo e avaliando se converge para um
  atrator, contando tambem a periodicidade da solucao.

  
*/

#define _BACIA_C

// inclusao dos cabecalhos do Directx
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <Windows.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>

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


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3D11Device*               g_pDevice = nullptr;
ID3D11DeviceContext*        g_pContext = nullptr;
ID3D11ComputeShader*        g_pCS = nullptr;

ID3D11Buffer*               g_pBufferX = nullptr;
ID3D11Buffer*               g_pBufferY = nullptr;
ID3D11Buffer*               g_pBufferK1 = nullptr;
ID3D11Buffer*				g_pBufferK2 = nullptr;
ID3D11Buffer*				g_pBufferK3 = nullptr;
ID3D11Buffer*				g_pBufferK4 = nullptr;


ID3D11Buffer*               g_pConstantBuffer = nullptr;


ID3D11UnorderedAccessView*  g_pBufferX_UAV = nullptr;
ID3D11UnorderedAccessView*  g_pBufferY_UAV = nullptr;
ID3D11UnorderedAccessView*  g_pBufferK1_UAV = nullptr;
ID3D11UnorderedAccessView*  g_pBufferK2_UAV = nullptr;
ID3D11UnorderedAccessView*  g_pBufferK3_UAV = nullptr;
ID3D11UnorderedAccessView*  g_pBufferK4_UAV = nullptr;


int numGroupThreads = 2;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
HRESULT CreateComputeDevice(_Outptr_ ID3D11Device** ppDeviceOut, _Outptr_ ID3D11DeviceContext** ppContextOut, _In_ bool bForceRef);
HRESULT CreateComputeShader(_In_z_ LPCWSTR pSrcFile, _In_z_ LPCSTR pFunctionName,
	_In_ ID3D11Device* pDevice, _Outptr_ ID3D11ComputeShader** ppShaderOut);
HRESULT CreateRawBuffer(_In_ ID3D11Device* pDevice, _In_ UINT uSize, _In_reads_(uSize) void* pInitData, _Outptr_ ID3D11Buffer** ppBufOut);
HRESULT CreateBufferUAV(_In_ ID3D11Device* pDevice, _In_ ID3D11Buffer* pBuffer, _Outptr_ ID3D11UnorderedAccessView** pUAVOut);
ID3D11Buffer* CreateAndCopyToDebugBuf(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pd3dImmediateContext, _In_ ID3D11Buffer* pBuffer);
void RunComputeShader(_In_ ID3D11DeviceContext* pd3dImmediateContext,
	_In_ ID3D11ComputeShader* pComputeShader,
	_In_ UINT nNumViewsSRV, _In_reads_(nNumViewsSRV) ID3D11ShaderResourceView** pShaderResourceViews,
	_In_opt_ ID3D11Buffer* pCBCS, _In_reads_opt_(dwNumDataBytes) void* pCSData, _In_ DWORD dwNumDataBytes,
	_In_ UINT nNumViewsURV, _In_reads_(nNumViewsURV) ID3D11UnorderedAccessView** pUnorderedAccessView,
	_In_ UINT X, _In_ UINT Y, _In_ UINT Z);
HRESULT FindDXSDKShaderFileCch(_Out_writes_(cchDest) WCHAR* strDestPath,
	_In_ int cchDest,
	_In_z_ LPCWSTR strFilename);

void Salva_log_Rkutta(ID3D11Device *p_Device, ID3D11DeviceContext *p_Context, ID3D11Buffer * p_Buffer, FILE *fd, int ncellToPrint, char* cabecalho);

// definicao das estruturas de dados que compoe o CONSTANT_BUFFER
// Constant Buffer Layout
struct CS_CONSTANT_BUFFER
{
	float cb_PL;
	float cb_omega;
	float cb_PL_8C;
	float cb_PL_8S;
	float cb_PL_9C;
	float cb_PL_9S;
	float cb_eta__1;
	float cb_eta__2;
	float cb_t;
	// como o constant buffer deve ser composto em múltiplo de 32*4 bits
	// sao adicionadas estas variaveis reservas que poderao ter uso futuro no codigo
	int cb_NumCel;
	float cb_Step;
	float cb_variavelReserva3;
} typedef CS_CONSTANT_BUFFER;

// declara constant buffer global
CS_CONSTANT_BUFFER ConstantBuffer;


/******************Declaracoes das funcoes ******************/
int Runge_Kutta(double Step, int Num_passosPorStep, int Total_Periodos,
	std::vector<ID3D11ComputeShader*> vetorPonteiroComputeShader,
	std::vector<ID3D11ComputeShader*> vP_CS_AtualizacaoRK, FILE *fd_log, FILE *fd, int ncellToPrint);
void NewData(void);
void CellsTrajec(void);
void CellsTrajec_core(int const nome_thread, int const cell_inicio, int const cell_fim);


/*===========================  Func  ===========================*/


/*===========================  Runge_Kutta  ===========================*/
int Runge_Kutta(double Step, int Num_passosPorStep, int Total_Periodos,
	std::vector<ID3D11ComputeShader*> vetorPonteiroComputeShader, 
	std::vector<ID3D11ComputeShader*> vP_CS_AtualizacaoRK, FILE *fd_log, FILE *fd, int ncellToPrint)
{

	int ii, j;

	

#ifdef DEBUG
	printf("\nStep = %f\n", Step);
#endif // DEBUG

	ConstantBuffer.cb_Step = (float)Step;

	float t = 0.0f;
	ii = 0;

	while (ii < Total_Periodos)
	{
		t = 0.0f;
		j = 0;
		/* Integracao de Runge-Kutta para um periodo */
		while (j < Num_passosPorStep)
		{
#ifdef DEBUG
			fprintf(fd_log, "\nTempo = %f", t + ConstantBuffer.cb_Step);
			Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferX, fd_log, ncellToPrint, "\ny_inicial\n");
#endif // DEBUG
			//--------------------
			// calcula valor de k1
			ConstantBuffer.cb_t = t;
			for (size_t i = 0; i < Nequ / 2; i++)
			{
				ID3D11UnorderedAccessView* aRViews[2] = { g_pBufferX_UAV, g_pBufferK1_UAV };
				RunComputeShader(g_pContext, vetorPonteiroComputeShader[i], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 2, aRViews, numGroupThreads, 1, 1);
			}
#ifdef DEBUG
			Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferK1, fd_log, ncellToPrint, "calculo_k1\n");
#endif // DEBUG

			//--------------------
			// calcula correcao para o proximo passo baseado no valor de k1
			{
				ID3D11UnorderedAccessView* aRViews[3] = { g_pBufferX_UAV, g_pBufferK1_UAV , g_pBufferY_UAV };
				RunComputeShader(g_pContext, vP_CS_AtualizacaoRK[0], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 3, aRViews, numGroupThreads, 1, 1);
#ifdef DEBUG
				Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferY, fd_log, ncellToPrint, "novo_y_correcao_baseado_em_k1\n");
#endif // DEBUG
			}

			//--------------------
			// calcula valor de k2
			ConstantBuffer.cb_t = t + ConstantBuffer.cb_Step / 2;
			for (size_t i = 0; i < Nequ / 2; i++)
			{
				ID3D11UnorderedAccessView* aRViews[2] = { g_pBufferY_UAV, g_pBufferK2_UAV };
				RunComputeShader(g_pContext, vetorPonteiroComputeShader[i], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 2, aRViews, numGroupThreads, 1, 1);
			}
#ifdef DEBUG
			Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferK2, fd_log, ncellToPrint, "calculo_k2\n");
#endif // DEBUG

			//--------------------
			// calcula correcao para o proximo passo baseado no valor de k2
			{
				ID3D11UnorderedAccessView* aRViews[3] = { g_pBufferX_UAV, g_pBufferK2_UAV , g_pBufferY_UAV };
				RunComputeShader(g_pContext, vP_CS_AtualizacaoRK[0], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 3, aRViews, numGroupThreads, 1, 1);
#ifdef DEBUG
				Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferY, fd_log, ncellToPrint, "novo_y_correcao_baseado_em_k2\n");
#endif // DEBUG
			}

			//--------------------
			// calcula valor de k3
			// ConstantBuffer.cb_t = t + ConstantBuffer.cb_Step / 2;
			for (size_t i = 0; i < Nequ / 2; i++)
			{
				ID3D11UnorderedAccessView* aRViews[2] = { g_pBufferY_UAV, g_pBufferK3_UAV };
				RunComputeShader(g_pContext, vetorPonteiroComputeShader[i], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 2, aRViews, numGroupThreads, 1, 1);
			}
#ifdef DEBUG
			Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferK3, fd_log, ncellToPrint, "calculo_k3\n");
#endif // DEBUG

			//--------------------
			// calcula correcao para o proximo passo baseado no valor de k3
			{
				ID3D11UnorderedAccessView* aRViews[3] = { g_pBufferX_UAV, g_pBufferK3_UAV , g_pBufferY_UAV };
				RunComputeShader(g_pContext, vP_CS_AtualizacaoRK[1], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 3, aRViews, numGroupThreads, 1, 1);
#ifdef DEBUG
				Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferY, fd_log, ncellToPrint, "novo_y_correcao_baseado_em_k3\n");
#endif // DEBUG
			}

			//--------------------
			// calcula valor de k4
			ConstantBuffer.cb_t = t + ConstantBuffer.cb_Step;
			for (size_t i = 0; i < Nequ / 2; i++)
			{
				ID3D11UnorderedAccessView* aRViews[2] = { g_pBufferY_UAV, g_pBufferK4_UAV };
				RunComputeShader(g_pContext, vetorPonteiroComputeShader[i], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 2, aRViews, numGroupThreads, 1, 1);
			}
#ifdef DEBUG
			Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferK4, fd_log, ncellToPrint, "calculo_k4\n");
#endif // DEBUG

			//--------------------
			// calcula correcao final baseado no valor de y0, k1, k2, k3, k4
			{
				ID3D11UnorderedAccessView* aRViews[6] = { g_pBufferX_UAV, g_pBufferK1_UAV, g_pBufferK2_UAV, g_pBufferK3_UAV , g_pBufferK4_UAV, g_pBufferY_UAV };
				RunComputeShader(g_pContext, vP_CS_AtualizacaoRK[2], 0, nullptr, g_pConstantBuffer, &ConstantBuffer, sizeof(ConstantBuffer), 6, aRViews, numGroupThreads, 1, 1);
#ifdef DEBUG
				Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferY, fd_log, ncellToPrint, "novo_y_correcao_baseado_em_k1_k2_k3_k4\n");
#endif // DEBUG
			}

			// atualiza t para o proximo Step
			t = t + (float)Step;
			j++;

#if false
			{
				char str_temp[50];
				sprintf(str_temp, "%f, ", t);
				Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferY, fd, ncellToPrint, str_temp);
			}
#endif


			/* Apos completar o step do runge-kutta o buffer g_pBufferY contem todos os pontos iniciais
			para o proximo step, sendo entao necessário atualizar o buffer g_pBufferX, entao aproveitando
			o fato que ambos sao ponteiros de endereço de memoria muda-se o endereco que eles apontam

			g_pBufferX => 0x00ff
			g_pBufferY => 0x0011
			g_pBufferTemp = g_pBufferY => 0x0011

			g_pBufferY = g_pBufferX => 0x00ff
			g_pBufferX = g_pBufferTemp => 0x0011			
			*/
			{
				ID3D11Buffer *g_pBufferTemp = g_pBufferY;
				g_pBufferY = g_pBufferX;
				g_pBufferX = g_pBufferTemp;
				g_pBufferTemp = nullptr;


				// tambem devem ser trocadas as janelas de acessos que apontavam para os enderecos antigos
				ID3D11UnorderedAccessView*  g_pBufferTemp_UAV = g_pBufferY_UAV;
				g_pBufferY_UAV = g_pBufferX_UAV;
				g_pBufferX_UAV = g_pBufferTemp_UAV;
				g_pBufferTemp_UAV = nullptr;
			}
		}
		// atualiza o contador de Periodos
		ii++;

// imprime secoes de poincare de uma celula, usar apemas para debug ou profile
#if true
		{
			char str_temp[50];
			sprintf(str_temp, "poincare cell %d t=%f, ", ncellToPrint, t);
			Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferX, fd_log, ncellToPrint, str_temp);
		}
#endif
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
	Passo = Tf / Ndiv;

	/* Alocacao na pilha de vetores para o tamanho da celula */
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

/* ===========================  Tempo  ===========================*/
struct Tempo
{
	std::chrono::time_point<std::chrono::steady_clock> inicio, fim;
	std::chrono::duration<double> duracao;
	int nome_thd;

public:
	//constroi objeto
	Tempo(int const i)
	{
		nome_thd = i;
		inicio = std::chrono::high_resolution_clock::now();
	}
	// destroi objeto
	~Tempo()
	{
		fim = std::chrono::high_resolution_clock::now();
		duracao = fim - inicio;

		printf("->>Tempo de calculo da thread %d : %f s\n", nome_thd, duracao);
	}
} typedef sTempo;

void inicializaVariaveisdeEntrada(int const total_celulas, int const cell_inicio, float* x, float* y_old, float* xo, float* y, CS_CONSTANT_BUFFER* ConstantBuffer) {

	//printf("\n Inicializando as variaveis de entrada. \n");
	//inicializa os dados de entrada de todas as celulas
	for (int i = 0; i < total_celulas; i++)
	{

		// inicializa todas as variaveis 
		for (int idx = 0; idx < Nequ; idx++)
		{
			x[idx*Num_cel + i] = 1e-4;
		}

		/* Coordenada de cada celula */
		x[Cor1*Num_cel + i] = (float)q1[i + cell_inicio];
		x[Cor2*Num_cel + i] = (float)q1p[i + cell_inicio];

		/* Coordenadas iniciais */
		for (int ij = 0; ij < Nequ; ij++)
		{
			y_old[ij*Num_cel + i] = x[ij*Num_cel + i];
			xo[ij*Num_cel + i] = x[ij*Num_cel + i];
			y[ij*Num_cel + i] = 0.0f;
		}
	}

	// atualiza os valores que serão constantes para o calculo de cada celula
	ConstantBuffer->cb_PL = static_cast<float>(PL);
	ConstantBuffer->cb_omega = static_cast<float>(Wf);
	ConstantBuffer->cb_PL_8C = static_cast<float>(PL_8C);
	ConstantBuffer->cb_PL_8S = static_cast<float>(PL_8S);
	ConstantBuffer->cb_PL_9C = static_cast<float>(PL_9C);
	ConstantBuffer->cb_PL_9S = static_cast<float>(PL_9S);
	ConstantBuffer->cb_eta__1 = static_cast<float>(eta__1);
	ConstantBuffer->cb_eta__2 = static_cast<float>(eta__2);
	ConstantBuffer->cb_t = 0.0f;
	ConstantBuffer->cb_NumCel = Num_cel;
}

/* ===========================  CellsTrajec  ===========================*/
void CellsTrajec(void)
{
	FILE *fd, *fd_log;

	int num_cells_thread;
	int n_max_thread;
	long int cellnum;
	int numMaxPeriodos = 3000;
	double criterioConvergencia = 1.0e-2;

	/* Abre arquivo de impressao   */
	fd = fopen("bacia_results.txt", "w");
	if (fd == NULL) {
		printf("\n Nao foi possivel abrir arquivo de bacia_results.txt !\n");
		exit(0);
		return;
	}

	/* Abre arquivo de impressao   */
	fd_log = fopen("rkutta_directx_log.txt", "w");
	if (fd == NULL) {
		printf("\n Nao foi possivel abrir arquivo de bacia_results.txt !\n");
		exit(0);
		return;
	}




	// inicializa threads de execucao
	printf("-->>>> Nao exceder a qtde de nucleos fisicos\n");
	printf("-->>>> Numero de threads : ");
#if false
	scanf("%d", &n_max_thread);
	printf("\n\n");

	if (n_max_thread < 1)
	{
		printf("\n Numero maximo de threads menor que 1!\n");
		exit(0);
	}
#endif
	n_max_thread = 1;

	/* Imprime cabecalho na tela */
	printf("Thread  Numero            q1_atr               q1p_atr               Tempo   Periodicidade\n");

	int nome_thread = 1, cell_inicio = 0, cell_fim = Num_cel - 1;

	// inicializa timer
		// como ele esta limitado ao escopo da funcao executada pela thread
		// logo que a thread terminar e o escopo do timer acabar ele imprime a duracao
	sTempo tempo(1);

	//imprime executanto thread
	printf("Executando thread %2d: processo: %6d cell_init: %6d cell_fim: %6d\n",
		nome_thread, std::this_thread::get_id(), cell_inicio, cell_fim);

	//int const nome_thread = nome sequencial da thread utilizado para criar arquivo de saida
	//int const cell_inicio = valor de busca da primeira celula no vetor q1 e q1p
	//int const cell_fim = valor de busca da ultima celula no vetor q1 e q1p

	// declaracao das variaveis locais de controle
	bool flag;
	int i, j, retorno, Periodo, PeriodoBack, flag_periodica;
	int value, ij, Tempo;
	//declaracao dos dados do host
	float*	x = new float [Nequ*Num_cel];
	float*	y = new float[Nequ*Num_cel];
	float*	y_old = new float[Nequ*Num_cel];
	float*	xo = new float[Nequ*Num_cel];
	// declaracao dos atratores
	float derro = 0.0f, zo0atr = 0.0f, zo1atr = 0.0f;

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

	std::vector<ID3D11ComputeShader*> vetorPonteiroComputeShader;
	std::vector<ID3D11ComputeShader*> vP_CS_AtualizacaoRK;

	printf("Inicializando variaveis das celulas e buffer constante...");
	inicializaVariaveisdeEntrada(total_celulas, cell_inicio, x, y_old, xo, y, &ConstantBuffer);
	printf("OK\n");

	printf("Criando Dispositivo...");
	if (FAILED(CreateComputeDevice(&g_pDevice, &g_pContext, false)))
		exit(0);
	printf("OK\n");

	printf("Criando Compute Shader...\n");
	{
		char buffer_name[50];
		wchar_t* name_WC;
		int numCaracteresEscritos = 0;
		for (size_t i = 0; i < Nequ/2; i++)
		{
			//armazena nome do kernel
			printf("Compilando kernel %d...", i+1);
			numCaracteresEscritos = sprintf(buffer_name, "kernel_f%d.hlsl", i+1);
			name_WC = (wchar_t *)malloc((numCaracteresEscritos + 1) * sizeof(wchar_t));
			mbstowcs(name_WC, buffer_name, numCaracteresEscritos +1);
			if (FAILED(CreateComputeShader(name_WC, "Func", g_pDevice, &g_pCS)))
				exit(0);
			free(name_WC);
			vetorPonteiroComputeShader.push_back(g_pCS);
		}
		printf("OK\n");
		printf("Compilando kernel atualizacao RK meio passo...");
		if (FAILED(CreateComputeShader(L"atualiza_RK_meioPasso.hlsl", "Func", g_pDevice, &g_pCS)))
			exit(0);
		vP_CS_AtualizacaoRK.push_back(g_pCS);
		printf("OK\n");
		printf("Compilando kernel atualizacao RK inteiro passo...");
		if (FAILED(CreateComputeShader(L"atualiza_RK_InteiroPasso.hlsl", "Func", g_pDevice, &g_pCS)))
			exit(0);
		vP_CS_AtualizacaoRK.push_back(g_pCS);
		printf("OK\n");
		printf("Compilando kernel atualizacao RK final passo...");
		if (FAILED(CreateComputeShader(L"atualiza_RK_FinalPasso.hlsl", "Func", g_pDevice, &g_pCS)))
			exit(0);
		vP_CS_AtualizacaoRK.push_back(g_pCS);
		printf("OK\n");
	}
	


	printf("Criando os buffers na memoria da placa de video...");
	//buffer que contem as variaveis do sistema no inico de cada passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &x[0], &g_pBufferX);
	//buffer que armazena as variaveis do sistema no fim do passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &y[0], &g_pBufferY);
	//buffer que armazena as variaveis da primeira interacao do passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &y[0], &g_pBufferK1);
	//buffer que armazena as variaveis da segunda interacao do passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &y[0], &g_pBufferK2);
	//buffer que armazena as variaveis da terceira interacao do passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &y[0], &g_pBufferK3);
	//buffer que armazena as variaveis da quarta interacao do passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &y[0], &g_pBufferK4);
	printf("OK\n");

	printf("Criando o buffer constante na memoria da placa de video...");
	// Cria Constante Buffer
	{
		D3D11_BUFFER_DESC constant_buffer_desc;
		constant_buffer_desc.ByteWidth = sizeof(CS_CONSTANT_BUFFER);
		constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constant_buffer_desc.MiscFlags = 0;
		constant_buffer_desc.StructureByteStride = 0;
		HRESULT sucesso = g_pDevice->CreateBuffer(&constant_buffer_desc, nullptr, &g_pConstantBuffer);
		if (FAILED(sucesso))
			exit(0);
		printf("OK\n");
	}

	printf("Criando janelas de buffer de memoria...");
	CreateBufferUAV(g_pDevice, g_pBufferX, &g_pBufferX_UAV);
	CreateBufferUAV(g_pDevice, g_pBufferY, &g_pBufferY_UAV);
	CreateBufferUAV(g_pDevice, g_pBufferK1, &g_pBufferK1_UAV);
	CreateBufferUAV(g_pDevice, g_pBufferK2, &g_pBufferK2_UAV);
	CreateBufferUAV(g_pDevice, g_pBufferK3, &g_pBufferK3_UAV);
	CreateBufferUAV(g_pDevice, g_pBufferK4, &g_pBufferK4_UAV);
	printf("OK\n");

	printf("Executando Compute Shader...");
	{
		// executa o RK em pararelo para todas as celulas por um periodo suficientemente longo
		sTempo timer(2);
		Runge_Kutta(Passo, Ndiv, 3000,
			vetorPonteiroComputeShader,
			vP_CS_AtualizacaoRK, fd_log, fd, 2);
	}
	printf("OK\n");

#if true
	Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferX, fd_log, 1, "ultimo ponto");
	Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferX, fd_log, 2, "ultimo ponto");
#endif

	printf("Executando pesquisa de periodicidade da resposta...");
	//buffer de memoria que armazena as condicoes iniciais para o teste de covergencia
	ID3D11Buffer* g_pBufferX_inicial = CreateAndCopyToDebugBuf(g_pDevice, g_pContext, g_pBufferX);
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	//variavel de memoria que armazena as condicoes iniciais para o teste de covergencia
	float *X0;
	g_pContext->Map(g_pBufferX_inicial, 0, D3D11_MAP_READ, 0, &MappedResource);
	//copia do buffer de memoria da GPU para a memoria da CPU
	X0 = (float*)MappedResource.pData;

	//buffer de memoria que armazena as condicoes finais apos o calculo de 1 periodo para o teste de covergencia
	ID3D11Buffer* g_pBufferX_novo = nullptr;
	//variavel de memoria que armazena as condicoes finais apos o calculo de 1 periodo para o teste de covergencia
	float *X_novo = nullptr;


	//variavel que armazena as periodicidades de cada celula
	// 0 = nao convergiu
	// valores diferentes de zero sao os periodos do RK convergido
	int* tabelaPeriodos = new int[total_celulas];
	for (size_t i = 0; i < total_celulas; i++)
	{
		tabelaPeriodos[i] = 0;
	}
	//contador de periodos para teste de convergencia
	Periodo = 0;

	for (size_t i_per = 0; i_per < PeriodoMaximo; i_per++)
	{
		//calcula RK em 1 periodo
		Runge_Kutta(Passo, Ndiv, 1,
			vetorPonteiroComputeShader,
			vP_CS_AtualizacaoRK, fd_log, fd, 2);
#if true
		Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferX, fd_log, 1, "ultimo ponto");
		Salva_log_Rkutta(g_pDevice, g_pContext, g_pBufferX, fd_log, 2, "ultimo ponto");
#endif

		//copia dados da GPU para a CPU
		g_pBufferX_novo = CreateAndCopyToDebugBuf(g_pDevice, g_pContext, g_pBufferX);
		//mapeia os dados copiados
		g_pContext->Map(g_pBufferX_novo, 0, D3D11_MAP_READ, 0, &MappedResource);
		//cast dados copiados para float
		X_novo = (float*)MappedResource.pData;

		//loop que percorre todas as celulas para verificar quais convergiram
		for (size_t i_celula = 0; i_celula < total_celulas; i_celula++)
		{
			//testa se a celula ja havia convergido, caso nao, verifica se convergiu no passo atual
			if (tabelaPeriodos[i_celula] == 0)
			{
				// verifica se os atratores Cor1 e Cor2 nao divergiram ao infinito
				if (X_novo[Cor1*total_celulas + i_celula] < 1e10 &&
					X_novo[Cor2*total_celulas + i_celula] < 1e10)
				{
					retorno = 0;
					//verifica se o ponto calculado é o mesmo do inicial e atende o criterio de convergencia
					for (int i_var = 0; i_var < Nequ; i_var++)
					{
						derro = fabs(X0[i_var*total_celulas + i_celula] - X_novo[i_var*total_celulas + i_celula]);
						if (derro > criterioConvergencia)
							retorno++;
					}
					// avalia retorno se convergiu
					if (retorno == 0)
					{
						//salva na tabela de periodos a periodidade da celula
						tabelaPeriodos[i_celula] = i_per + 1;
						//Se a celula convergiu verifica se ela não excedeu os limites estabelecidos pelos usuario
						if (Y1min <= X_novo[Cor1*total_celulas + i_celula] &&
							Y1max >= X_novo[Cor1*total_celulas + i_celula])
						{
							if (Y2min <= X_novo[Cor2*total_celulas + i_celula] &&
								Y2max >= X_novo[Cor2*total_celulas + i_celula])
							{
								// imprime no arquivo txt com formatacao periodica
								fprintf(fd, "%d, %16.12e, %16.12e, %16.12e, %16.12e,",
									(i_celula + 1), x[Cor1*total_celulas + i_celula], x[Cor2*total_celulas + i_celula],
									X_novo[Cor1*total_celulas + i_celula], X_novo[Cor2*total_celulas + i_celula]);
								for (int j = 0; j < Nequ; j++)
								{
									fprintf(fd, "%16.12e,  ", X_novo[j*total_celulas + i_celula]);
								}
								fprintf(fd, "%5d\n", numMaxPeriodos+ i_per+1);
							}
						}
					}

				}

			}
		}
	}
	//apos pesquisar pela periodicidade em cada celula pelo maximo de tentativas
	//salva aqueles que nao convergiram como pontos não periodicos
	printf("OK\n");


	printf("Salvando respostas nao periodicas...");
	//loop que percorre todas as celulas para verificar quais nao convergiram
	for (size_t i_celula = 0; i_celula < total_celulas; i_celula++)
	{
		//testa se a celula nao convergiu
		if (tabelaPeriodos[i_celula] == 0)
		{
			// imprime no arquivo txt com formatacao periodica
			fprintf(fd, "%d, %16.12e, %16.12e, %16.12e, %16.12e,",
				(-i_celula - 1), x[Cor1*total_celulas + i_celula], x[Cor2*total_celulas + i_celula],
				X_novo[Cor1*total_celulas + i_celula], X_novo[Cor2*total_celulas + i_celula]);
			for (int j = 0; j < Nequ; j++)
			{
				fprintf(fd, "%16.12e,  ", X_novo[j*total_celulas + i_celula]);
			}
			fprintf(fd, "%5d\n", numMaxPeriodos + PeriodoMaximo + 1);
		}
	}
	printf("OK\n");


	return;
	SAFE_RELEASE(g_pBufferK1_UAV);
	SAFE_RELEASE(g_pBufferK2_UAV);
	SAFE_RELEASE(g_pBufferK3_UAV);
	SAFE_RELEASE(g_pBufferK4_UAV);
	SAFE_RELEASE(g_pBufferX_UAV);
	SAFE_RELEASE(g_pBufferY_UAV);
	SAFE_RELEASE(g_pBufferK1);
	SAFE_RELEASE(g_pBufferK2);
	SAFE_RELEASE(g_pBufferK3);
	SAFE_RELEASE(g_pBufferK4);
	SAFE_RELEASE(g_pBufferX);
	SAFE_RELEASE(g_pBufferY);
	SAFE_RELEASE(g_pConstantBuffer);
	SAFE_RELEASE(g_pCS);
	SAFE_RELEASE(g_pContext);
	SAFE_RELEASE(g_pDevice);
	SAFE_RELEASE(g_pBufferX_novo);
	SAFE_RELEASE(g_pBufferX_inicial);
}




/* ===========================  main  ===========================*/
void main(void)
{

  printf("==============Calculo de Bacias de Atracao============\n");
  printf("=================Metodo  da Forca Bruta===============\n");
  printf("===============Execucao Distribuida em GPU============\n\n\n");
  NewData(  );

  //printf("entrei em Cells\n");
  CellsTrajec();
 
}


//--------------------------------------------------------------------------------------
// Create the D3D device and device context suitable for running Compute Shaders(CS)
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateComputeDevice(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut, bool bForceRef)
{
	*ppDeviceOut = nullptr;
	*ppContextOut = nullptr;

	HRESULT hr = S_OK;

	UINT uCreationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	uCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL flOut;
	static const D3D_FEATURE_LEVEL flvl[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	bool bNeedRefDevice = false;
	if (!bForceRef)
	{
		hr = D3D11CreateDevice(nullptr,                        // Use default graphics card
			D3D_DRIVER_TYPE_HARDWARE,    // Try to create a hardware accelerated device
			nullptr,                        // Do not use external software rasterizer module
			uCreationFlags,              // Device creation flags
			flvl,
			sizeof(flvl) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,           // SDK version
			ppDeviceOut,                 // Device out
			&flOut,                      // Actual feature level created
			ppContextOut);              // Context out

		if (SUCCEEDED(hr))
		{
			// A hardware accelerated device has been created, so check for Compute Shader support

			// If we have a device >= D3D_FEATURE_LEVEL_11_0 created, full CS5.0 support is guaranteed, no need for further checks
			if (flOut < D3D_FEATURE_LEVEL_11_0)
			{
				// Otherwise, we need further check whether this device support CS4.x (Compute on 10)
				D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
				(*ppDeviceOut)->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
				if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
				{
					bNeedRefDevice = true;
					printf("No hardware Compute Shader capable device found, trying to create ref device.\n");
				}
			}
		}
	}

	if (bForceRef || FAILED(hr) || bNeedRefDevice)
	{
		// Either because of failure on creating a hardware device or hardware lacking CS capability, we create a ref device here

		SAFE_RELEASE(*ppDeviceOut);
		SAFE_RELEASE(*ppContextOut);

		hr = D3D11CreateDevice(nullptr,                        // Use default graphics card
			D3D_DRIVER_TYPE_REFERENCE,   // Try to create a hardware accelerated device
			nullptr,                        // Do not use external software rasterizer module
			uCreationFlags,              // Device creation flags
			flvl,
			sizeof(flvl) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,           // SDK version
			ppDeviceOut,                 // Device out
			&flOut,                      // Actual feature level created
			ppContextOut);              // Context out
		if (FAILED(hr))
		{
			printf("Reference rasterizer device create failure\n");
			return hr;
		}
	}

	return hr;
}

void Salva_log_Rkutta(ID3D11Device *p_Device, ID3D11DeviceContext *p_Context, ID3D11Buffer * p_Buffer, FILE *fd, int ncellToPrint, char* cabecalho) {

	ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(p_Device, p_Context, p_Buffer);
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	float *p;
	p_Context->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

	// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
	// This is also a common trick to debug CS programs.
	p = (float*)MappedResource.pData;

	fprintf(fd, "%s", cabecalho);
	for (int j = 0; j < Nequ; j++)
	{
		fprintf(fd, "%25.20f,  ", p[j*Num_cel + ncellToPrint -1]);
	}
	fprintf(fd, "\n");

	p_Context->Unmap(debugbuf, 0);

	SAFE_RELEASE(debugbuf);
}

//--------------------------------------------------------------------------------------
// Compile and create the CS
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName,
	ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut)
{
	if (!pDevice || !ppShaderOut)
		return E_INVALIDARG;

	// Finds the correct path for the shader file.
	// This is only required for this sample to be run correctly from within the Sample Browser,
	// in your own projects, these lines could be removed safely
	WCHAR str[MAX_PATH];
	HRESULT hr = FindDXSDKShaderFileCch(str, MAX_PATH, pSrcFile);
	if (FAILED(hr))
		return hr;

	DWORD dwShaderFlags = D3DCOMPILE_SKIP_OPTIMIZATION;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		nullptr, nullptr
	};

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlob = nullptr;
	hr = D3DCompileFromFile(str, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pFunctionName, pProfile,
		dwShaderFlags, 0, &pBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		SAFE_RELEASE(pErrorBlob);
		SAFE_RELEASE(pBlob);

		return hr;
	}

	hr = pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, ppShaderOut);

	SAFE_RELEASE(pErrorBlob);
	SAFE_RELEASE(pBlob);

#if defined(_DEBUG) || defined(PROFILE)
	if (SUCCEEDED(hr))
	{
		(*ppShaderOut)->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pFunctionName), pFunctionName);
	}
#endif

	return hr;
}

//--------------------------------------------------------------------------------------
// Tries to find the location of the shader file
// This is a trimmed down version of DXUTFindDXSDKMediaFileCch.
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT FindDXSDKShaderFileCch(WCHAR* strDestPath,
	int cchDest,
	LPCWSTR strFilename)
{
	if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
		return E_INVALIDARG;

	// Get the exe name, and exe path
	WCHAR strExePath[MAX_PATH] =
	{
		0
	};
	WCHAR strExeName[MAX_PATH] =
	{
		0
	};
	WCHAR* strLastSlash = nullptr;
	GetModuleFileName(nullptr, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;
	strLastSlash = wcsrchr(strExePath, TEXT('\\'));
	if (strLastSlash)
	{
		wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

		// Chop the exe name from the exe path
		*strLastSlash = 0;

		// Chop the .exe from the exe name
		strLastSlash = wcsrchr(strExeName, TEXT('.'));
		if (strLastSlash)
			*strLastSlash = 0;
	}

	// Search in directories:
	//      .\
    //      %EXE_DIR%\..\..\%EXE_NAME%

	wcscpy_s(strDestPath, cchDest, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	swprintf_s(strDestPath, cchDest, L"%s\\_kernel_directx\\%s", strExePath, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	// On failure, return the file as the path but also return an error code
	wcscpy_s(strDestPath, cchDest, strFilename);

	return E_FAIL;
}


//--------------------------------------------------------------------------------------
// Create Raw Buffer
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateRawBuffer(ID3D11Device* pDevice, UINT uSize, void* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = nullptr;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = uSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, ppBufOut);
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		// This is a Structured Buffer

		desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
		desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

// --------------------------------------------------------------------------------------
// Run CS
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
void RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
	ID3D11ComputeShader* pComputeShader,
	UINT nNumViewsSRV, ID3D11ShaderResourceView** pShaderResourceViews,
	ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
	UINT nNumViewsURV,
	ID3D11UnorderedAccessView** pUnorderedAccessView,
	UINT X, UINT Y, UINT Z)
{


	pd3dImmediateContext->CSSetShader(pComputeShader, nullptr, 0);
	pd3dImmediateContext->CSSetShaderResources(0, nNumViewsSRV, pShaderResourceViews);
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, nNumViewsURV, pUnorderedAccessView, nullptr);
	if (pCBCS && pCSData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pd3dImmediateContext->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pCSData, dwNumDataBytes);
		pd3dImmediateContext->Unmap(pCBCS, 0);
		ID3D11Buffer* ppCB[1] = { pCBCS };
		pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCB);
	}

	pd3dImmediateContext->Dispatch(X, Y, Z);

	pd3dImmediateContext->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewnullptr[100] = { nullptr };
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, nNumViewsURV, ppUAViewnullptr, nullptr);

	ID3D11ShaderResourceView* ppSRVnullptr[100] = { nullptr };
	pd3dImmediateContext->CSSetShaderResources(0, nNumViewsSRV, ppSRVnullptr);

	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCBnullptr);
}


//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc = {};
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
	{
#if defined(_DEBUG) || defined(PROFILE)
		debugbuf->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Debug") - 1, "Debug");
#endif

		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}


